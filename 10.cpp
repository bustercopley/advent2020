#include "precompiled.h"
#include "symbols.h"

auto regex = re::regex(R"(^(\d+)(?:,(\d+))?$)");

// I'm disappointed there are no runs of length greater than 5.
std::map<std::int64_t, std::int64_t> combinations{
  {0, 1}, {1, 1}, {2, 1}, {3, 2}, {4, 4}, {5, 7}};

void parts(std::istream &stream) {
  bool test = false;
  std::int64_t result_1 = 0;
  std::int64_t result_2 = 1;
  std::int64_t expected_1 = 0;
  std::int64_t expected_2 = 0;

  std::vector<std::int64_t> joltages;

  for (std::string line; std::getline(stream, line);) {
    if (auto m = re::match(regex, line)) {
      if (!matched(m, 2)) {
        joltages.push_back(string_to<std::int64_t>(match_view(m, 1, line)));
      } else {
        test = true;
        expected_1 = string_to<std::int64_t>(match_view(m, 1, line));
        expected_2 = string_to<std::int64_t>(match_view(m, 2, line));
      }
    }
  }

  std::sort(std::begin(joltages), std::end(joltages));
  joltages.push_back(joltages.back() + 3);
  std::int64_t n1 = 0, n3 = 0;
  std::int64_t joltage = 0;
  std::int64_t runlength = 1;
  for (std::size_t i = 0; i != std::size(joltages); ++i) {
    if (joltages[i] == joltage + 1) {
      ++runlength;
    } else {
      ++n3;
      n1 += runlength - 1;
      result_2 *= combinations[runlength];
      runlength = 1;
    }
    joltage = joltages[i];
  }
  result_1 = n1 * n3;

  if (test) {
    if (result_1 != expected_1) {
      std::cout << "Fail, got " << result_1 << ", expected " << expected_1
                << std::endl;
    }
    if (result_2 != expected_2) {
      std::cout << "Fail, got " << result_2 << ", expected " << expected_2
                << std::endl;
    }
  } else {
    std::cout << "Day 10, Part One\n"
              << result_1 << "\n"
              << "Day 10, Part Two\n"
              << result_2 << std::endl;
  }
}

int main() {
  for (auto filename : {"test/10", "test/10b", "input/10"}) {
    if (std::ifstream stream(filename); stream) {
      parts(stream);
    }
  }
  return 0;
}
