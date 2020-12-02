#include "precompiled.h"

void parts(std::istream &stream, int part) {
  auto re = re::regex(R"(^(\d+)-(\d+) ([a-z]): ([a-z]+)$|^(\d+),(\d)+$)");
  bool test = false;
  int result = 0, expected = -1;

  std::string line;
  while (std::getline(stream, line)) {
    if (auto m = re::match(re, line)) {
      if (matched(m, 1)) {
        int low = std::stoi(match_string(m, 1));
        int high = std::stoi(match_string(m, 2));
        char c = match_string(m, 3)[0];
        std::string pw = match_string(m, 4);

        if (part == 1) {
          int n = std::count(std::begin(pw), std::end(pw), c);
          result += low <= n && n <= high;
        }
        else {
          result += (pw[low - 1] == c) + (pw[high - 1] == c) == 1;
        }
      } else {
        test = true;
        expected = std::stoi(match_string(m, part == 1 ? 5 : 6));
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
  std::cout << "Day 2, Part One" << std::endl;
  for (auto filename : {"test/02", "input/02"}) {
    if (std::ifstream stream(filename); stream) {
      parts(stream, 1);
    }
  }

  std::cout << "Day 2, Part Two" << std::endl;
  for (auto filename : {"test/02", "input/02"}) {
    if (std::ifstream stream(filename); stream) {
      parts(stream, 2);
    }
  }

  return 0;
}
