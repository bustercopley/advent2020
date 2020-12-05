#include "precompiled.h"

bool valid_year(std::string_view s, int first, int last) {
  if (std::size(s) != 4) {
    return false;
  }
  int year;
  auto begin = std::data(s), end = std::data(s) + 4;
  auto [p, e] = std::from_chars(begin, end, year);
  return e == std::errc{} && p == end && first <= year && year <= last;
}

template <std::size_t N> bool all(bool (&arr)[N]) {
  return std::accumulate(
    std::begin(arr), std::end(arr), true, std::logical_and{});
}

void parts(std::istream &stream, int part) {
  auto re1 = re::regex(R"(^(?:(\d+),(\d+))?$)");
  auto re2 = re::regex(
    R"(^(?:(byr)|(iyr)|(eyr)|(hgt)|(hcl)|(ecl)|(pid)|cid):(\S+)(?:\s|$)(.*))");
  re::code field_regex[] = {
    re::regex(R"(^(\d+)(?:(cm)|in)$)"),
    re::regex(R"(^#[0-9a-f]{6}$)"),
    re::regex(R"(^(?:amb|blu|brn|gry|grn|hzl|oth)$)"),
    re::regex(R"(^\d{9}$)"),
  };
  bool test = false;
  int result = 0;
  int index = 0;

  std::string line;
  bool field_ok[7];
  while (std::getline(stream, line)) {
    if (auto m = match(re1, line)) {
      // blank line (or end of test)
      result += all(field_ok);
      if (matched(m, 1)) {
        test = true;
        int test_part = string_to<int>(match_view(m, 1, line));
        int expected = string_to<int>(match_view(m, 2, line));
        if (part == test_part && result != expected) {
          std::cout << "Fail, index " << index << ", result " << result
                    << ", expected " << expected << std::endl;
        }
        ++index;
        result = 0;
      }
      std::memset(field_ok, '\0', sizeof field_ok);
    } else {
      std::string_view rest_of_line(line);
      while (auto m = match(re2, rest_of_line)) {
        for (int i = 0; i != 7; ++i) {
          if (matched(m, i + 1)) {
            if (part == 1) {
              field_ok[i] = true;
            } else {
              auto value = match_view(m, 8, rest_of_line);
              switch (i) {
              case 0:
                field_ok[0] |= valid_year(value, 1920, 2002);
                break;
              case 1:
                field_ok[1] |= valid_year(value, 2010, 2020);
                break;
              case 2:
                field_ok[2] |= valid_year(value, 2020, 2030);
                break;
              case 3: {
                if (auto m = match(field_regex[0], value)) {
                  auto n = std::stoi(match_string(m, 1));
                  if (matched(m, 2) ? (150 <= n && n <= 193)
                                    : (59 <= n && n <= 76)) {
                    field_ok[3] = true;
                  }
                }
                break;
              }
              default:
                field_ok[i] |= match(field_regex[i - 3], value) != nullptr;
                break;
              }
            }
          }
        }
        rest_of_line = match_view(m, 9, rest_of_line);
      }
    }
  }
  if (!test) {
    // no blank line at end of input
    result += all(field_ok);
    std::cout << result << std::endl;
  }
}

int main() {
  std::cout << "Day 4, Part One" << std::endl;
  for (auto filename : {"test/04", "input/04"}) {
    if (std::ifstream stream(filename); stream) {
      parts(stream, 1);
    }
  }

  std::cout << "Day 4, Part Two" << std::endl;
  for (auto filename : {"test/04", "input/04"}) {
    if (std::ifstream stream(filename); stream) {
      parts(stream, 2);
    }
  }

  return 0;
}
