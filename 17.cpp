#include "precompiled.h"
#include "stopwatch.h"
#include <future>
#include <thread>

auto regex = re::regex(R"(^(\d+),(\d+)$)");

template <bool Parallel = false> struct executor {};

template <> struct executor<true> {
  template <typename F> void operator()(F &&f) {
    threads.emplace_back(std::forward<F>(f));
  }

  void join() {
    for (auto &thread : threads) {
      thread.join();
    }
    threads.clear();
  }

  std::vector<std::thread> threads;
};

template <> struct executor<false> {
  template <typename F> void operator()(F &&f) { f(); }
  void join() {}
};

void inflate(auto &grid, std::size_t size) {
  constexpr bool dimension_ge_2 = requires { grid[0][0]; };
  constexpr bool dimension_ge_3 = requires { grid[0][0][0]; };
  std::size_t layers_at_front = (dimension_ge_3 ? 1 : size);
  grid.reserve(std::size(grid) + size + layers_at_front);

  // ensure empty layer at end, recursively inflate next lower dimension
  if constexpr (dimension_ge_2) {
    for (auto &layer : grid) {
      inflate(layer, size);
    }
    grid.emplace_back(std::size(grid[0]), grid[0][0]);
  } else {
    grid.push_back('.');
  }

  // insert empty layers before and after
  grid.insert(std::begin(grid), layers_at_front, grid.back());
  grid.insert(std::end(grid), size - 1, grid.back());
}

template <std::size_t Dimensions> auto increase_dimension(const auto &plane) {
  if constexpr (Dimensions == 0) {
    return plane;
  } else {
    return std::vector(1, increase_dimension<Dimensions - 1>(plane));
  }
}

// 'subcript(sequence, i, j, k)' means 'sequence[i][j][k]', etc.
auto &subscript(auto &atom) { return atom; }
auto &subscript(auto &sequence, std::size_t i, auto... j) {
  return subscript(sequence[i], j...);
}

// call 'f(grid, i...)' for each atom 'subscript(grid, i...)' in the
// multidimensional sequence 'grid'
template <bool Parallel = false, typename F>
void for_subscripts(auto &grid, F &&f, auto... i) {
  static_assert(!Parallel || sizeof...(i) == 0);
  if constexpr (requires { subscript(grid, i...)[0]; }) {
    std::size_t size = std::size(subscript(grid, i...));
    if constexpr (Parallel) {
      executor<true> exec;
      for (std::size_t j = 0; j != size; ++j) {
        exec([&grid, &f, j] { for_subscripts(grid, std::forward<F>(f), j); });
      }
      exec.join();
    } else {
      for (std::size_t j = 0; j != size; ++j) {
        for_subscripts(grid, std::forward<F>(f), i..., j);
      }
    }
  } else {
    f(grid, i...);
  }
}

// make a zero-initialized multidimensional array of the same dimension and
// extents as 'grid'
auto make_zeros(const auto &grid) {
  if constexpr (requires { grid[0]; }) {
    return std::vector(std::size(grid), make_zeros(grid[0]));
  } else {
    return (std::uint8_t)0;
  }
}

// for each atom in 'grid', set the corresponding element of 'counts' to the
// number of adjacent atoms in 'grid' equal to '#'; neighbours in layer 2 of
// atoms in layer 1 count double in dimensions 3 and greater
template <bool Parallel = false>
void count_neighbours(const auto &grid, auto &counts) {
  constexpr bool dimension_ge_2 = requires { grid[0][0]; };
  constexpr bool dimension_ge_3 = requires { grid[0][0][0]; };

  if constexpr (dimension_ge_2) {
    executor<Parallel> exec;
    // zero out layer 1 of counts
    exec([&c = counts[1]]() {
      for_subscripts(
        c, [](auto &layer, auto... j) { subscript(layer, j...) = 0; });
    });

    // overwrite layers 2... with counts for layers 1... in the lower dimension
    for (std::size_t i = 1; i != std::size(grid) - 1; ++i) {
      exec([&g = grid[i], &c = counts[i + 1]]() {
        count_neighbours(g, c);
      });
    }
    exec.join();
    // combine layers to get the counts for this dimension (in correct layer)
    // don't parallelize this loop...
    for (std::size_t i = 1; i != std::size(grid) - 1; ++i) {
      int multiplier = (dimension_ge_3 && i == 1) ? 2 : 1;
      if (i + 2 == std::size(grid)) {
        multiplier = 0; // because there is no counts[i + 2]
      }
      // ...parallelize this one instead
      for_subscripts<Parallel>(counts[i], [multiplier](auto &layer, auto... j) {
        auto &cell = subscript(layer, j...);
        cell += subscript((&layer)[1], j...);
        if (multiplier) {
          cell += multiplier * subscript((&layer)[2], j...);
        }
      });
    }
  } else {
    for (std::size_t i = 1; i != std::size(grid) - 1; ++i) {
      counts[i] =
        (grid[i - 1] == '#') + (grid[i] == '#') + (grid[i + 1] == '#');
    }
  }
}

// count atoms in 'grid' equal to '#'; atoms not in layer 1 count double in
// dimensions 3 and greater
template <bool Parallel = false> std::size_t count_cubes(const auto &grid) {
  constexpr bool dimension_ge_2 = requires { grid[0][0]; };
  constexpr bool dimension_ge_3 = requires { grid[0][0][0]; };
  if constexpr (dimension_ge_2) {
    std::size_t result = 0;
    if constexpr (Parallel && dimension_ge_3) {
      std::vector<std::future<std::size_t>> futures;
      for (std::size_t i = 1; i != std::size(grid) - 1; ++i) {
        futures.push_back(
          std::async([&g = grid[i]]() { return count_cubes(g); }));
      }
      for (std::size_t multiplier = 1; auto &future : futures) {
        result += multiplier * future.get();
        multiplier = 2;
      }
    } else {
      for (std::size_t i = 1; i != std::size(grid) - 1; ++i) {
        int multiplier = (!dimension_ge_3 || i == 1) ? 1 : 2;
        result += multiplier * count_cubes(grid[i]);
      }
    }
    return result;
  } else {
    return std::count(std::begin(grid), std::end(grid), '#');
  }
}

// apply the rules of life and death to each atom of 'grid'
void step(auto &grid, auto &counts) {
  count_neighbours<true>(grid, counts);
  for_subscripts(grid, [&counts](auto &grid, auto... i) {
    auto &cell = subscript(grid, i...);
    auto count = subscript(counts, i...);
    if (cell == '#') {
      // the cell itself is counted as one of its own neighbours
      if (count != 3 && count != 4) {
        cell = '.';
      }
    } else if (count == 3) {
      cell = '#';
    }
  });
}

template <std::size_t Part> void part(std::istream &&stream) {
  if (!stream) {
    return;
  }

  bool test = false;
  std::size_t result = 0;
  std::size_t expected = 0;

  // read the plane
  std::vector<std::string> plane;
  for (std::string line; std::getline(stream, line);) {
    if (auto m = re::match(regex, line)) {
      if (Part <= 2) {
        test = true;
        expected = string_to<std::size_t>(match_view(m, Part, line));
      }
    } else {
      plane.push_back(line);
    }
  }

  stopwatch timer;
  constexpr bool verbose = true;
  if constexpr (verbose) {
    std::cout << "Dimension " << Part + 2 << ", reset timer" << std::endl;
  }
  std::size_t iterations = 6;
  auto grid = increase_dimension<Part>(plane);
  inflate(grid, iterations + 1);
  auto counts = make_zeros(grid);
  std::cout << "Elapsed " << timer.stamp() << ", allocation done" << std::endl;
  for (std::size_t i = 0; i != iterations; ++i) {
    step(grid, counts);
    if constexpr (verbose) {
      result = count_cubes<true>(grid);
      std::cout << "Elapsed " << timer.stamp() << ", iteration " << i
                << ", cubes " << result << std::endl;
    }
  }

  // count nonempty cells
  if constexpr (!verbose) {
    result = count_cubes<true>(grid);
  }

  if (test) {
    if (result != expected) {
      std::cout << "Fail, got " << result << ", expected " << expected
                << std::endl;
    }
  } else {
    std::cout << result << std::endl;
  }
}

int main() {
  std::cout << "Day 17, Part One" << std::endl;
  for (auto filename : {"test/17", "input/17"}) {
    part<1>(std::ifstream(filename));
  }

  std::cout << "Day 17, Part Two" << std::endl;
  for (auto filename : {"test", "input/17"}) {
    part<2>(std::ifstream(filename));
  }

  std::cout << "Day 17, Upping the Ante" << std::endl;
  for (auto filename : {"input/17"}) {
    part<3>(std::ifstream(filename)); // 5 dimensions
    part<4>(std::ifstream(filename)); // 6 dimensions
    part<5>(std::ifstream(filename)); // ...
    part<6>(std::ifstream(filename));
    //part<7>(std::ifstream(filename)); // if you have ~16 GB
    //part<8>(std::ifstream(filename)); // if you have ~128 GB and ~45 minutes
  }

  return 0;
}