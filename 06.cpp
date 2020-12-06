#include "precompiled.h"

auto regex = re::regex(R"(^(?:|([a-z]+)|(\d+),(\d+))$)");

void parts(std::istream &stream, int part) {
  bool test = false;
  int result = 0;
  int expected = 0;

  int group_size = 0;
  int group_result = 0;
  int questions[26]{};
  std::string line;

  do {
    std::getline(stream, line);
    if (auto m = re::match(regex, line)) {
      if (matched(m, 1)) {
        // normal line
        ++group_size;
        for (char c : line) {
          ++questions[c - 'a'];
        }
      } else if (matched(m, 2)) {
        // expected results (test file only)
        test = true;
        expected = std::stoi(match_string(m, part == 1 ? 2 : 3));
      } else {
        // empty line or end of stream
        for (auto n : questions) {
          group_result += n != 0 && (part == 1 || n == group_size);
        }
        result += group_result;
        std::memset(&questions, '\0', sizeof questions);
        group_size = 0;
        group_result = 0;
      }
    }
  } while (stream);

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
  std::cout << "Day 6, Part One" << std::endl;
  for (auto filename : {"test/06", "input/06"}) {
    if (std::ifstream stream(filename); stream) {
      parts(stream, 1);
    }
  }

  std::cout << "Day 6, Part Two" << std::endl;
  for (auto filename : {"test/06", "input/06"}) {
    if (std::ifstream stream(filename); stream) {
      parts(stream, 2);
    }
  }

  return 0;
}
