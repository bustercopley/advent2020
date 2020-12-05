#include "precompiled.h"

void parts(std::istream &stream, int part) {
  auto re = re::regex(R"(^(\w+)(?::(\d+),(\d+),(\d+))?$)");
  bool test = false;
  int result = 0;
  std::set<int> set;

  std::string line;
  while (std::getline(stream, line)) {
    if (auto m = re::match(re, line)) {
      // read two binary integers from string
      int row = 0;
      for (int bit = 1 << 6, i = 0; i != 7; ++i, bit >>= 1) {
        if (line[i] == 'B') {
          row += bit;
        }
      }
      int col = 0;
      for (int bit = 1 << 2, i = 0; i != 3; ++i, bit >>= 1) {
        if (line[7 + i] == 'R') {
          col += bit;
        }
      }

      int id = row * 8 + col;
      set.insert(id);
      result = std::max(result, id);

      test = matched(m, 2);
      if (test && part == 1) {
        if (row != std::stoi(match_string(m, 2)) ||
            col != std::stoi(match_string(m, 3)) ||
            id != std::stoi(match_string(m, 4))) {
          std::cout << "Fail, " << line << ", got row " << row << " col " << col
                    << std::endl;
        }
      }
    }
  }
  if (!test) {
    if (part == 1) {
      std::cout << result << std::endl;
    } else {
      int prev = std::numeric_limits<int>::min();
      for (auto n : set) {
        if (n - 1 == prev + 1) {
          std::cout << n - 1 << std::endl;
          break;
        }
        prev = n;
      }
    }
  }
}

int main() {
  std::cout << "Day 5, Part One" << std::endl;
  for (auto filename : {"test/05", "input/05"}) {
    if (std::ifstream stream(filename); stream) {
      parts(stream, 1);
    }
  }

  std::cout << "Day 5, Part Two" << std::endl;
  for (auto filename : {"test/05", "input/05"}) {
    if (std::ifstream stream(filename); stream) {
      parts(stream, 2);
    }
  }

  return 0;
}
