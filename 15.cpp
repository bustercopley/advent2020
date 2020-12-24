#include "precompiled.h"
#include "split.h"

auto regex1 = re::regex(R"(^([^:]+)(?::(\d+),(\d+))?$)");
auto regex2 = re::regex(R"(^(\d+)(?:,|$)(.*)$)");
auto times = new int[30000000];

void parts(std::istream &stream, int part) {
  const int limit = part == 1 ? 2020 : 30000000;
  for (std::string line; std::getline(stream, line);) {
    if (auto m = match(regex1, line)) {
      std::memset(times, '\0', limit * sizeof times[0]);
      int result = 0;
      int expected = 0;
      int time = 0;
      bool test = matched(m, 2);
      if (test) {
        expected = string_to<int>(match_view(m, part == 1 ? 2 : 3, line));
      }
      for (auto [sv] : split(match_view(m, 1, line), regex2)) {
        times[result] = time++;
        result = string_to<int>(sv);
      }
      while (time != limit) {
        int previous_result = result;
        result = times[previous_result] ? time - times[previous_result] : 0;
        times[previous_result] = time++;
      }
      if (test) {
        if (result != expected) {
          std::cout << "Fail, got " << result << ", expected " << expected
                    << std::endl;
        }
      } else {
        std::cout << result << std::endl;
      }
    }
  }
}

int main() {
  std::cout << "Day 15, Part One" << std::endl;
  for (auto filename : {"test/15", "input/15"}) {
    if (std::ifstream stream(filename); stream) {
      parts(stream, 1);
    }
  }
  std::cout << "Day 15, Part Two" << std::endl;
  for (auto filename : {"test/15", "input/15"}) {
    if (std::ifstream stream(filename); stream) {
      parts(stream, 2);
    }
  }
  return 0;
}
