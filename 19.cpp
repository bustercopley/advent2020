#include "precompiled.h"
#include "split.h"
#include <functional>

auto regex1 = re::regex(
  R"!(^(?:expected: (\d+),(\d+)|(\d+): (?:"([a-z])"|(.*))|([a-z]+))$)!");
auto regex2 = re::regex(R"(^([^|]+)(?: \| |$)(.*)$)");
auto regex3 = re::regex(R"!(^(\d+)(?: |$)(.*))!");

using z = std::size_t;

struct rule {
  char c = '\0';
  std::vector<std::vector<int>> disjunction;
};

// if any alternative matches a prefix of s, remove the prefix and return true
bool greedy_match(std::string_view &s, const std::vector<rule> &rules, int i) {
  if (std::empty(rules[i].disjunction)) {
    assert(rules[i].c != '\0');
    if (s[0] == rules[i].c) {
      s.remove_prefix(1);
      return true;
    }
    return false;
  }
  const auto &disjunction = rules[i].disjunction;
  return std::any_of(std::begin(disjunction), std::end(disjunction),
    [&s, &rules](const std::vector<int> &concatenation) {
      auto t = s;
      if (std::all_of(std::begin(concatenation), std::end(concatenation),
            [&t, &rules](int j) { return greedy_match(t, rules, j); })) {
        s = t;
        return true;
      }
      return false;
    });
}

void parts(std::istream &stream, int part) {
  bool test = false;
  int result = 0;
  int expected = 0;

  std::vector<rule> rules;

  for (std::string line; std::getline(stream, line);) {
    if (auto m = match(regex1, line)) {
      if (matched(m, 1)) {
        test = true;
        expected = string_to<int>(match_view(m, part, line));
      } else if (matched(m, 3)) {
        // got a rule id, ensure rules list is large enough
        std::size_t id = string_to<std::size_t>(match_view(m, 3, line));
        rules.resize(std::max(std::size(rules), id + 1));
        auto &rule = rules[id];
        if (matched(m, 4)) {
          // got a rule of the form "a"
          rule.c = match_view(m, 4, line)[0];
        } else {
          // got a disjunction of one or more concatenations of rule ids
          for (auto [terms] : split(match_view(m, 5, line), regex2)) {
            // consume one concatenation of one or more rule ids
            rule.disjunction.emplace_back();
            auto &concatenation = rule.disjunction.back();
            for (auto [term] : split(terms, regex3)) {
              // consume one rule id
              concatenation.push_back(string_to<int>(term));
            }
          }
        }
      } else {
        auto s = match_view(m, 6, line);
        if (part == 1) {
          result += greedy_match(s, rules, 0) && std::empty(s);
        } else {
          // Rule "8: 42 | 42 8" matches 42{n} if n >= 1
          // Rule "11: 42 31 | 42 11 31" matches 42{n} 31{n} if n >= 1
          // Rule "0: 8 11" matches 42{n} 31{m} if m >= 1 and n >= m + 1
          int n = 0;
          int m = 0;
          while (greedy_match(s, rules, 42)) {
            ++n;
          }
          while (greedy_match(s, rules, 31)) {
            ++m;
          }
          result += std::empty(s) && m >= 1 && n >= m + 1;
        }
      }
    }
  }

  if (test) {
    if (result != expected) {
      std::cout << "Fail, got " << result << ", expected " << expected
                << std::endl;
      std::exit(1);
    }
  } else {
    std::cout << result << std::endl;
  }
}

int main() {
  for (auto filename : {"test/19", "test/19b", "input/19"}) {
    if (std::ifstream stream(filename); stream) {
      parts(stream, 1);
    }
  }

  for (auto filename : {"test/19b", "input/19"}) {
    if (std::ifstream stream(filename); stream) {
      parts(stream, 2);
    }
  }
  return 0;
}
