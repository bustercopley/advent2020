#include "precompiled.h"
#include "split.h"
#include <list>

using ll = std::int64_t;

void permute(std::vector<int> &permutation, int current, int iterations) {
  int size = std::size(permutation);
  // Do the dance
  for (int i = 0; i != iterations; ++i) {
    int a = permutation[current];
    int b = permutation[a];
    int c = permutation[b];
    // Find destination
    int destination = current;
    do {
      destination = (destination + (size - 1)) % size;
    } while (destination == a || destination == b || destination == c);
    // Move picked cups to after destination
    int t = permutation[destination];
    permutation[destination] = permutation[current];
    permutation[current] = permutation[c];
    permutation[c] = t;
    // Move head to new current cup
    current = permutation[current];
  }
}

std::string part_one(auto &&input) {
  std::vector<int> permutation(9);
  int index = input[8] - '1';
  for (char c : input) {
    index = permutation[index] = c - '1';
  }
  permute(permutation, input[0] - '1', 100);
  std::string result(8, '\0');
  index = 0;
  for (int pos = 0; pos != 8; ++pos) {
    index = permutation[index];
    result[pos] = '1' + index;
  }
  return result;
}

ll part_two(auto &&input) {
  std::vector<int> permutation(1'000'000);
  int index = 999'999;
  for (char c : input) {
    index = permutation[index] = c - '1';
  }
  for (int i = 9; i != 1'000'000; ++i) {
    index = permutation[index] = i;
  }
  permute(permutation, input[0] - '1', 10'000'000);
  int a = permutation[0];
  int b = permutation[a];
  return (ll)(a + 1) * (ll)(b + 1);
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
  for (auto filename : {"test/23", "input/23"}) {
    if (std::ifstream stream(filename); stream) {
      for (std::string line; std::getline(stream, line);) {
        if (auto m = match(regex, line)) {
          auto result = part_two(match_view(m, 1, line));
          if (!matched(m, 2)) {
            std::cout << result << std::endl;
          } else {
            auto expected = string_to<ll>(match_view(m, 3, line));
            if (result != expected) {
              std::cout << "Fail, got " << result << ", expected " << expected
                        << std::endl;
            }
          }
        }
      }
    }
  }
  return 0;
}
