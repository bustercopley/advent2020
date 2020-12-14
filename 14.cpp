#include "precompiled.h"

auto regex = re::regex(R"(^(?:(\d+)|mask = ([X01]+)|mem\[(\d+)] = (\d+))$)");

void part(std::istream &stream, int part) {
  bool test = false;
  std::size_t result = 0;
  std::size_t expected = 0;

  std::uint64_t mask_on = 0;
  std::uint64_t mask_off = -1;
  std::uint64_t mask_float = 0;
  std::map<std::uint64_t, std::uint64_t> mem;
  for (std::string line; std::getline(stream, line);) {
    if (auto m = re::match(regex, line)) {
      if (matched(m, 1)) {
        test = true;
        expected = string_to<std::size_t>(match_view(m, 1, line));
      } else if (matched(m, 2)) {
        mask_on = 0;
        mask_off = -1;
        mask_float = 0;
        auto view = match_view(m, 2, line);
        for (std::size_t bit = 1ull << 35; char c : view) {
          if (c == '0') {
            mask_off &= ~bit;
          } else if (c == '1') {
            mask_on |= bit;
          } else {
            mask_float |= bit;
          }
          bit >>= 1;
        }
      } else if (part) {
        auto address = string_to<std::uint64_t>(match_view(m, 3, line));
        auto value = string_to<std::uint64_t>(match_view(m, 4, line));
        if (part == 1) {
          mem[address] = (value & mask_off) | mask_on;
        } else {
          std::uint64_t n = 1ull << std::popcount(mask_float);
          for (std::uint64_t i = 0; i != n; ++i) {
            auto a = address;
            auto m = mask_float;
            for (std::uint64_t j = i, bit = 1; m; bit <<= 1, m >>= 1) {
              if (m & 1) {
                if (j & 1) {
                  a |= bit;
                } else {
                  a &= ~bit;
                }
                j >>= 1;
              }
            }
            mem[a | mask_on] = value;
          }
        }
      }
    }
  }
  for (auto [address, value] : mem) {
    result += value;
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
  std::cout << "Day 14, Part One" << std::endl;
  for (auto filename : {"test/14", "input/14"}) {
    if (std::ifstream stream(filename); stream) {
      part(stream, 1);
    }
  }

  std::cout << "Day 14, Part Two" << std::endl;
  for (auto filename : {"test/14b", "input/14"}) {
    if (std::ifstream stream(filename); stream) {
      part(stream, 2);
    }
  }

  return 0;
}
// 26876878660844
// 6386593869035
