#include "precompiled.h"
#include "split.h"
#include "stopwatch.h"

using z = std::size_t;
using ll = std::int64_t;
constexpr bool verbose = false;

// adjust window so that [iter + 1, iter + 4) is not split
void adjust_window(std::span<int> &span, z &offset, auto &iter) {
  z ok = std::end(span) - iter;
  if (ok == 2 || ok == 3) {
    --ok;
    if (offset > ok) {
      // move window left
      std::copy(iter + 1, std::end(span), std::begin(span) - ok);
      span = {std::begin(span) - ok, std::size(span)};
      offset = offset - ok;
    } else {
      // move window right
      std::copy(std::begin(span), std::begin(span) + 3u - ok, std::end(span));
      span = {std::begin(span) + 3u - ok, std::size(span)};
      offset = offset + 3u - ok;
    }
  }
}

auto permute(std::span<int> &span, z iterations) {
  stopwatch timer;
  z size = std::size(span);
  z offset = 0;
  auto current = std::begin(span);
  for (z i = 0; i != iterations; ++i) {
    int dest = *current;
    adjust_window(span, offset, current);
    ++current;
    if (current == std::end(span)) {
      current = std::begin(span);
    }
    // find destination
    do {
      dest = (dest + (size - 1)) % size;
    } while (std::find(current, current + 3, dest) != current + 3);
    auto dest_iter = std::ranges::find(span, dest);
    // move current cups to after destination
    if (current < dest_iter) {
      std::rotate(current, current + 3, dest_iter + 1);
    } else {
      std::rotate(dest_iter + 1, current, current + 3);
      current += 3;
      if (current == std::end(span)) {
        current = std::begin(span);
      }
    }
    if constexpr (verbose) {
      if ((i % 65536) == 65535) {
        std::cout << "Iteration " << i << " done ("
                  << (i * (100.0 / iterations)) << " %), elapsed "
                  << timer.stamp() << std::endl;
      }
    }
  }
  if constexpr (verbose) {
    std::cout << "Permutation done, elapsed " << timer.stamp() << std::endl;
  }
  auto iter = std::ranges::find(span, 0);
  adjust_window(span, offset, iter);
  return iter;
}

std::string part_one(auto &&s) {
  int array[11];
  std::span span(std::data(array), 9);
  std::transform(std::begin(s), std::end(s), std::begin(span),
    [](char c) { return c - '1'; });
  auto iter = permute(span, 100);
  std::ranges::rotate(span, iter);
  std::string result(8, '\0');
  std::ranges::transform(
    span.last(8), std::data(result), [](int n) { return char('1' + n); });
  return result;
}

ll part_two(auto &&s) {
  auto array = std::make_unique<int[]>(1'000'002);
  std::span span(array.get(), 1'000'000);
  std::iota(std::transform(std::begin(s), std::end(s), std::begin(span),
              [](char c) { return c - '1'; }),
    std::end(span), 9);
  auto iter = permute(span, 10'000'000);
  return (ll)(*(iter + 1) + 1) * (ll)(*(iter + 2) + 1);
}

int main() {
  auto regex = re::regex(R"(^([1-9]{9})(?:,([2-9]{8}),(\d+))?$)");
  // Part One
  for (auto filename : {"test/23", "input/23"}) {
    if (std::ifstream stream(filename); stream) {
      for (std::string line; std::getline(stream, line);) {
        if (auto m = match(regex, line)) {
          auto result = part_one(match_view(m, 1, line));
          if (!matched(m, 2)) {
            std::cout << result << std::endl;
          } else {
            auto expected = match_view(m, 2, line);
            if (result != expected) {
              std::cout << "Fail, got " << result << ", expected "
                        << expected << std::endl;
            }
          }
        }
      }
    }
  }

  // Part Two (brute force, ~40 minutes)
  for (auto filename : {"input/23"}) {
    if (std::ifstream stream(filename); stream) {
      for (std::string line; std::getline(stream, line);) {
        if (auto m = match(regex, line)) {
          auto result = part_two(match_view(m, 1, line));
          std::cout << result << std::endl;
        }
      }
    }
  }
  return 0;
}
