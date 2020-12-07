#include "precompiled.h"

auto regex1 = re::regex(R"(^(?:([^,.0-9]+) bags contain(.*)|(\d+),(\d+))$)");
auto regex2 = re::regex(R"(^ (\d+) ([^,.0-9]+) bags?[,.](.*)$)");

struct symbols {
private:
  using map = std::map<std::string, std::size_t>;
  map symbols;
  std::vector<map::const_iterator> names;
  auto intern(const std::string &&name) {
    auto [iter, emplaced] =
      symbols.try_emplace(std::move(name), std::size(symbols));
    if (emplaced) {
      names.push_back(iter);
    }
    return iter->second;
  }

public:
  const std::string &operator[](std::size_t index) {
    return names[index]->first;
  }
  auto operator[](const std::string &n) { return intern(std::string(n)); }
  auto operator[](std::string &&name) { return intern(std::move(name)); }
  auto size() const { return std::size(names); }
};

void parts(std::istream &stream, int part) {
  bool test = false;
  std::size_t result = 0;
  std::size_t expected = 0;

  symbols colors;
  std::vector<std::map<int, std::size_t>> rules;

  for (std::string line; std::getline(stream, line);) {
    if (auto m = re::match(regex1, line)) {
      if (matched(m, 1)) {
        int outer_color = colors[match_string(m, 1)];
        auto rest_of_line = match_view(m, 2, line);
        while (auto m = re::match(regex2, rest_of_line)) {
          auto count = string_to<std::size_t>(match_view(m, 1, rest_of_line));
          auto inner_color = colors[match_string(m, 2)];
          rules.resize(std::size(colors));
          rules[inner_color][outer_color] = count;
          rest_of_line = match_view(m, 3, rest_of_line);
        }
      } else {
        test = true;
        expected =
          string_to<std::size_t>(match_view(m, part == 1 ? 3 : 4, line));
      }
    }
  }

  if (part == 1) {
    std::vector<int> set(std::size(colors));
    set[colors["shiny gold"]] = 1;
    bool done = false;
    while (!done) {
      done = true;
      result = 0;
      for (std::size_t i = 0; i != std::size(colors); ++i) {
        if (auto level = set[i]) {
          result += level > 1;
          for (auto pair : rules[i]) {
            if (!set[pair.first]) {
              done = false;
              set[pair.first] = level + 1;
            }
          }
        }
      }
    }
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
  return 0;
}
