#include "precompiled.h"
#include "split.h"
#include "symbols.h"

auto regex1 = re::regex(R"(^(?:([^,.0-9]+) bags contain(.*)|(\d+),(\d+))$)");
auto regex2 = re::regex(R"(^ (\d+) ([^,.0-9]+) bags?[,.](.*)$)");

void parts(std::istream &stream, int part) {
  bool test = false;
  std::size_t result = 0;
  std::size_t expected = 0;

  symbols colors;
  colors["shiny gold"];

  std::vector<std::map<int, std::size_t>> rules;

  for (std::string line; std::getline(stream, line);) {
    if (auto m = re::match(regex1, line)) {
      if (matched(m, 1)) {
        int outer_color = colors[match_string(m, 1)];
        for (auto [a, b] : split<2>(match_view(m, 2, line), regex2)) {
          auto count = string_to<std::size_t>(a);
          auto inner_color = colors[std::string(b)];
          rules.resize(std::size(colors));
          if (part == 1) {
            rules[inner_color][outer_color] = count;
          } else {
            rules[outer_color][inner_color] = count;
          }
        }
      } else {
        test = true;
        expected =
          string_to<std::size_t>(match_view(m, part == 1 ? 3 : 4, line));
      }
    }
  }

  std::vector<std::size_t> set(std::size(colors));
  if (part == 1) {
    set[0] = 1;
    bool done = false;
    while (!done) {
      done = true;
      result = -1;
      for (std::size_t inner = 0; inner != std::size(colors); ++inner) {
        if (set[inner]) {
          ++result;
          for (auto [outer, count] : rules[inner]) {
            if (!set[outer]) {
              done = false;
              set[outer] = 1;
            }
          }
        }
      }
    }
  } else {
    while (!set[0]) {
      for (int outer = 0; outer != (int)std::size(colors); ++outer) {
        if (!set[outer]) {
          std::size_t counts_for = 1;
          bool all_set = true;
          for (auto [inner, count] : rules[outer]) {
            if (!set[inner]) {
              all_set = false;
            } else {
              counts_for += count * set[inner];
            }
          }
          if (all_set) {
            set[outer] = counts_for;
          }
        }
      }
    }
    result = set[0] - 1;
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

int main() {
  std::cout << "Day 7, Part One" << std::endl;
  for (auto filename : {"test/07", "input/07"}) {
    if (std::ifstream stream(filename); stream) {
      parts(stream, 1);
    }
  }

  std::cout << "Day 7, Part Two" << std::endl;
  for (auto filename : {"test/07", "test/07b", "input/07"}) {
    if (std::ifstream stream(filename); stream) {
      parts(stream, 2);
    }
  }

  return 0;
}
