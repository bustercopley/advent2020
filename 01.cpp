#include "precompiled.h"

void parts(std::istream &stream, int part) {
  auto re = re::regex(R"(^(\d+)(?:,(\d+))?$)");
  bool test = false;
  std::int64_t result = -1, expected = -1;
  std::vector<int> entries;

  std::string line;
  while (std::getline(stream, line)) {
    if (auto m = re::match(re, line)) {
      if (re::matched(m, 2)) {
        test = true;
        expected = std::stoi(match_string(m, part));
      } else {
        entries.push_back(std::stoi(match_string(m, 1)));
      }
    }
  }

  if (part == 1) {
    for (std::size_t i = 0; i != std::size(entries) - 1; ++i) {
      for (std::size_t j = i + 1; j != std::size(entries); ++j) {
        if (entries[i] + entries[j] == 2020) {
          result = entries[i] * entries[j];
          break;
        }
      }
    }
  } else {
    for (std::size_t i = 0; i != std::size(entries) - 2; ++i) {
      for (std::size_t j = i + 1; j != std::size(entries) - 1; ++j) {
        for (std::size_t k = j + 1; k != std::size(entries); ++k) {
          if (entries[i] + entries[j] + entries[k] == 2020) {
            result = entries[i] * entries[j] * entries[k];
            break;
          }
        }
      }
    }
  }

  if (!test) {
    std::cout << result << std::endl;
  } else if (result != expected) {
    std::cout << "Fail, result " << result << ", expected " << expected
              << std::endl;
  }
}

int main() {
  std::cout << "Day 1, Part One" << std::endl;
  for (auto filename : {"test/01", "input/01"}) {
    if (std::ifstream stream(filename); stream) {
      parts(stream, 1);
    }
  }

  std::cout << "Day 1, Part Two" << std::endl;
  for (auto filename : {"test/01", "input/01"}) {
    if (std::ifstream stream(filename); stream) {
      parts(stream, 2);
    }
  }

  return 0;
}
