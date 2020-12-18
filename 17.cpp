#include "precompiled.h"

auto regex = re::regex(R"(^(\d+),(\d+)$)");

void inflate(auto &grid, std::size_t size) {
  grid.reserve(std::size(grid) + 2 * size);
  if constexpr (requires { grid[0][0]; }) {
    for (auto &layer : grid) {
      inflate(layer, size);
    }
    grid.emplace_back(std::size(grid[0]), grid[0][0]);
  } else {
    grid.push_back('.');
  }
  grid.insert(std::begin(grid), size, grid.back());
  grid.insert(std::end(grid), size - 1, grid.back());
}

template <std::size_t Dimensions> auto make_grid(const auto &plane) {
  if constexpr (Dimensions == 0) {
    return plane;
  } else {
    return std::vector(1, make_grid<Dimensions - 1>(plane));
  }
}

std::size_t nearby(const auto &grid, bool offcentre, std::size_t i, auto... j) {
  std::size_t count = 0;
  if (i != 0 && i != std::size(grid) - 1) {
    for (std::size_t ii = 0; ii != 3; ++ii) {
      if constexpr (sizeof...(j) != 0) {
        count += nearby(grid[i + ii - 1], offcentre || ii != 1, j...);
      } else {
        count += (offcentre || ii != 1) && grid[i + ii - 1] == '#';
      }
    }
  }
  return count;
}

auto &subscript(auto &atom) { return atom; }
auto &subscript(auto &sequence, std::size_t i, auto... j) {
  return subscript(sequence[i], j...);
}

template <typename F> void for_subscripts(auto &grid, F &&f, auto... i) {
  if constexpr (requires { subscript(grid, i...)[0]; }) {
    for (std::size_t j = 0; j != std::size(subscript(grid, i...)); ++j) {
      for_subscripts(grid, std::forward<F>(f), i..., j);
    }
  } else {
    f(grid, i...);
  }
}

auto zeroes(const auto &grid) {
  if constexpr (requires { grid[0]; }) {
    return std::vector(std::size(grid), zeroes(grid[0]));
  } else {
    return 0;
  }
}

void count_neighbours(auto &grid, auto &counts) {
  if constexpr (requires { grid[0][0]; }) {
    for (std::size_t i = 1; i != std::size(grid) - 1; ++i) {
      count_neighbours(grid[i], counts[i + 1]);
    }
    for (std::size_t i = 1; i != std::size(grid) - 1; ++i) {
      auto &a = counts[i];
      const auto &b = counts[i + 1];
      const auto &c = counts[(i + 2) % std::size(grid)];
      for_subscripts(a, [&b, &c](auto &layer, auto... i) {
        subscript(layer, i...) += subscript(b, i...) + subscript(c, i...);
      });
    }
    auto &last = counts[std::size(counts) - 1];
    const auto &first = counts[0];
    for_subscripts(last, [first](auto &last, auto... i) {
      subscript(last, i...) = subscript(first, i...);
    });
  } else {
    for (std::size_t i = 1; i != std::size(grid) - 1; ++i) {
      counts[i] =
        (grid[i - 1] == '#') + (grid[i] == '#') + (grid[i + 1] == '#');
    }
  }
}

void step(auto &grid, auto &counts) {
  count_neighbours(grid, counts);

  for_subscripts(grid, [&counts](auto &grid, auto... i) {
    auto &cell = subscript(grid, i...);
    auto count = subscript(counts, i...);
    if (cell == '#') {
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

  std::size_t iterations = 6;
  auto grid = make_grid<Part>(plane);
  inflate(grid, iterations + 1);
  auto counts = zeroes(grid);
  for (std::size_t i = 0; i != iterations; ++i) {
    step(grid, counts);
  }

  // count nonempty cells
  for_subscripts(grid, [&result](const auto &grid, auto... i) {
    result += subscript(grid, i...) == '#';
  });

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
  for (auto filename : {"test/17", "input/17"}) {
    part<2>(std::ifstream(filename));
  }

  std::cout << "Day 17, Upping the Ante" << std::endl;
  for (auto filename : {"test/17"}) {
    part<3>(std::ifstream(filename));
    part<4>(std::ifstream(filename));
    part<5>(std::ifstream(filename));
    // part<6>(std::ifstream(filename));
  }

  return 0;
}
