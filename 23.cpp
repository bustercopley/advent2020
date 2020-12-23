#include "precompiled.h"
#include "split.h"
#include <future>
#include <iomanip>

using z = std::size_t;

std::string part_one(std::string &&s, z iterations = 100) {
  for (z i = 0; i != iterations; ++i) {
    char curr = s[0];
    char dest = s[0];
    // find destination
    do {
      dest = char('1' + (dest - '1' + 9 - 1) % 9);
    } while (std::find(&s[1], &s[4], dest) != &s[4]);
    // move destination to front
    auto [mid, end] = std::ranges::rotate(s, std::ranges::find(s, dest));
    // move picked cups after destination
    std::rotate(std::begin(s) + 1, mid + 1, mid + 4);
    // move new current cup to front
    std::ranges::rotate(s, std::ranges::find(s, curr) + 1);
  }
  // move '1' to back
  std::ranges::rotate(s, std::ranges::find(s, '1') + 1);
  s.erase(9 - 1);
  return s;
}

int main() {
  auto regex = re::regex(R"(^([1-9]{9})(?:,([2-9]{8}),(\d+))?$)");
  for (auto filename : {"test/23", "input/23"}) {
    if (std::ifstream stream(filename); stream) {
      for (std::string line; std::getline(stream, line);) {
        if (auto m = match(regex, line)) {
          auto subject = match_view(m, 1, line);
          auto result = part_one(std::string(subject));
          if (!matched(m, 2)) {
            std::cout << result << std::endl;
          } else {
            auto expected = match_view(m, 2, line);
            if (result != expected) {
              std::cout << "Fail, " << subject << " -> " << result
                        << ", expected " << expected << std::endl;
            }
          }
        } else {
          std::cout << "Warning, line \"" << line << "\" failed to match"
                    << std::endl;
        }
      }
    }
  }
  return 0;
}
