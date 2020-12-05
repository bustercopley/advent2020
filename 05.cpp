#include "precompiled.h"

auto regex = re::regex(R"(^[FBLR]{10}(?::(\d+))?$)");

void parts(std::istream &stream, int part) {
  bool test = false;
  int min = std::numeric_limits<int>::max();
  int max = std::numeric_limits<int>::min();
  int sum = 0;

  for (std::string line; std::getline(stream, line);) {
    if (auto m = re::match(regex, line)) {
      // read binary integer from string
      int id = 0;
      for (int i = 0; i != 10; ++i) {
        id = (id << 1) | (line[i] == 'B') | (line[i] == 'R');
      }

      max = std::max(max, id);
      min = std::min(min, id);
      sum += id;

      if (matched(m, 1)) {
        test = true;
        if (part == 1 && id != std::stoi(match_string(m, 1))) {
          std::cout << "Fail, " << line << ", got id " << id << std::endl;
        }
      }
    }
  }

  if (!test) {
    int result = part == 1 ? max : (min + max) * (max - min + 1) / 2 - sum;
    std::cout << result << std::endl;
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
