#include "precompiled.h"

void parts(std::istream &stream, int part) {
  auto re = re::regex(R"(^(\d+),(\d+)$)");
  bool test = false;
  std::size_t result = 0, expected = -1;

  std::vector<std::string> v;
  for (std::string line; std::getline(stream, line);) {
    if (auto m = re::match(re, line)) {
      test = true;
      expected = std::stoi(match_string(m, part == 1 ? 1 : 2));
    } else {
      v.push_back(line);
    }
  }

  result = 0;
  for (std::size_t j = 0; const auto &s : v) {
    result += s[j] == '#';
    j = (j + 3) % std::size(s);
  }

  if (part == 2) {
    using slopes = std::pair<std::size_t, std::size_t>[];
    for (auto [dx, dy] : slopes{{1, 1}, {5, 1}, {7, 1}, {1, 2}}) {
      std::size_t n = 0;
      for (std::size_t i = 0, j = 0; i < std::size(v); i += dy) {
        n += v[i][j] == '#';
        j = (j + dx) % std::size(v[i]);
      }
      result *= n;
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
  std::cout << "Day 3, Part One" << std::endl;
  for (auto filename : {"test/03", "input/03"}) {
    if (std::ifstream stream(filename); stream) {
      parts(stream, 1);
    }
  }

  std::cout << "Day 3, Part Two" << std::endl;
  for (auto filename : {"test/03", "input/03"}) {
    if (std::ifstream stream(filename); stream) {
      parts(stream, 2);
    }
  }

  return 0;
}
