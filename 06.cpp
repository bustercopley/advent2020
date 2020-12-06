#include "precompiled.h"

auto regex = re::regex(R"(^(?:(\d+),(\d+))?$)");

void parts(std::istream &stream, int part) {
  bool test = false;
  int result = 0;
  int block_size = 0;
  int block_count = 0;
  int expected = 0;
  int questions[26] = {};
  for (std::string line; std::getline(stream, line);) {
    if (auto m = re::match(regex, line)) {
      if (matched(m, 1)) {
        test = true;
        expected = std::stoi(match_string(m, part));
        break;
      }
      for (auto n : questions) {
        block_count += (part == 1 ? (n != 0) : (n == block_size));
      }
      result += block_count;
      std::memset(&questions, '\0', sizeof questions);
      block_size = 0;
      block_count = 0;
    } else {
      ++block_size;
      for (char c : line) {
        ++questions[c - 'a'];
      }
    }
  }

  for (auto n : questions) {
    block_count += (part == 1 ? n != 0 : n == block_size);
  }
  result += block_count;

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
