#include "precompiled.h"
#include "split.h"
#include "stopwatch.h"
#include <list>

using z = std::size_t;
using ll = std::int64_t;
constexpr bool verbose = false;

using z = std::size_t;

void permute(std::list<int> &list, z size, z iterations) {
  stopwatch timer;
  // Build list index (map content to iterator)
  std::vector<std::list<int>::const_iterator> index(size);
  for (auto iter = std::begin(list); iter != std::end(list); ++iter) {
    index[*iter - 1] = iter;
  }
  // Do the dance
  for (z i = 0; i != iterations; ++i) {
    auto pick_begin = std::next(std::begin(list));
    auto pick_end = std::next(pick_begin, 3);
    // Find destination
    int dest = list.front();
    do {
      dest = 1 + (dest + (size - 2)) % size;
    } while (std::find(pick_begin, pick_end, dest) != pick_end);
    auto dest_iter = index[dest - 1];
    // Move picked cups to after destination
    list.splice(std::next(dest_iter), list, pick_begin, pick_end);
    // Move new current cup to front
    list.splice(std::begin(list), list, std::next(std::begin(list)), std::end(list));
  }
  // Move 1 to back
  list.splice(std::begin(list), list, std::next(index[0]), std::end(list));
  std::cout << "Permutation done, " << timer.stamp() << std::endl;
}

std::string part_one(auto &&input) {
  std::list<int> list;
  std::ranges::transform(input, std::back_insert_iterator(list),
    [](char c) -> int { return c - '0'; });
  permute(list, 9, 100);
  std::string result;
  std::ranges::transform(list | std::ranges::views::take(8),
    std::back_insert_iterator(result), [](char c) -> int { return '0' + c; });
  return std::string(result);
}

ll part_two(auto &&input) {
  std::list<int> list;
  std::ranges::transform(input, std::back_insert_iterator(list),
    [](char c) -> int { return c - '0'; });
  for (int i = 10; i != 1'000'001; ++i) {
    list.push_back(i);
  }
  permute(list, 1'000'000, 10'000'000);
  return (ll)(*std::begin(list)) * (ll)(*std::next(std::begin(list)));
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
              std::cout << "Fail, got " << result << ", expected " << expected
                        << std::endl;
            }
          }
        }
      }
    }
  }

  // Part Two
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
