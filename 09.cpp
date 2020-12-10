#include "precompiled.h"
#include "symbols.h"

void parts(std::istream &stream, bool test, std::size_t window,
  std::int64_t expected_1, std::int64_t expected_2 [[maybe_unused]]) {

  std::vector<std::int64_t> history;
  auto check = [&history, window]() -> bool {
    std::size_t start = std::size(history) - window - 1;
    for (std::size_t i = 0; i != window - 1; ++i) {
      for (std::size_t j = i + 1; j != window; ++j) {
        if (history[start + i] + history[start + j] == history.back()) {
          return true;
        }
      }
    }
    return false;
  };

  std::int64_t day1 = -1;
  for (std::string line; std::getline(stream, line);) {
    history.push_back(string_to<std::int64_t>(line));
    if (std::size(history) > window) {
      if (day1 == -1 && !check()) {
        day1 = history.back();
      }
    }
  }

  if (test) {
    if ((day1 != expected_1) || day1 == -1) {
      std::cout << "Fail, got " << day1 << ", expected " << expected_1
                << std::endl;
      day1 = -1;
    }
  } else {
    std::cout << "Day 9, Part One\n" << day1 << std::endl;
  }

  if (day1 != -1) {
    for (std::size_t i = 0; i != std::size(history) - 1; ++i) {
      std::int64_t min = history[i];
      std::int64_t max = history[i];
      std::int64_t sum = history[i];
      for (std::size_t j = i + 1; j != std::size(history) && sum < day1; ++j) {
        min = std::min(min, history[j]);
        max = std::max(max, history[j]);
        sum = sum + history[j];
        if (sum == day1) {
          std::int64_t day2 = min + max;
          if (test) {
            if (day2 != expected_2) {
              std::cout << "Fail, got " << day2 << ", expected " << expected_2
                        << std::endl;
            }
          } else {
            std::cout << "Day 9, Part Two\n" << day2 << std::endl;
          }
          return;
        }
      }
    }
  }
  std::cout << "Fail, not found" << std::endl;
}

int main() {
  std::tuple<std::string, bool, std::size_t, std::int64_t, std::int64_t>
    cases[]{
      {"test/09", true, 5, 127, 62},
      {"input/09", false, 25, 0, 0},
    };

  for (auto [filename, test, window, expected_1, expected_2] : cases) {
    if (std::ifstream stream(filename); stream) {
      parts(stream, test, window, expected_1, expected_2);
    }
  }

  return 0;
}
