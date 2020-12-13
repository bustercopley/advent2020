#include "precompiled.h"
#include "egcd.h"

auto regex1 = re::regex(R"(^(?:(\d+)|([0-9x,]+)(?::(?:(\d+),)?(\d+))?)$)");
auto regex2 = re::regex(R"(^(?:x|(\d+))(?:,|$)(.*)$)");

// Bus id 'n' departs at time 'a' if a === 0 (mod n).
// Input:
//   arrival time a,
//   list (n_i: i in I) of bus ids, for some set I of non-negative integers.

// Part One:
// Let d(n) be the earliest departure of bus n not earlier than time a.
// Find n_i such that d(n_i) is minimized.
// Solution is n_i * d(n_i).

// Part Two:
// Find the least t such that t + i === 0 (mod n_i) for all i in I.
// Assume (i) 0 is in I, and (ii) the n_i are pairwise coprime.

// Signed modular multiplication, avoiding overflow.
std::int64_t modmul(std::int64_t a, std::int64_t b, std::int64_t mod) {
  std::int64_t result = 0;
  a = a * ((b > 0) - (b < 0)); // Account for sign of b, ignored below
  while (b) {
    if (b % 2) {
      result = (result + a) % mod;
    }
    a = (a * 2) % mod;
    b /= 2;
  }
  return result;
}

void parts(std::istream &stream) {
  std::int64_t arrival = 0;
  std::vector<std::pair<int64_t, int64_t>> remainders;
  for (std::string line; std::getline(stream, line);) {
    if (auto m = match(regex1, line)) {
      if (matched(m, 1)) {
        arrival = string_to<std::int64_t>(line);
      } else {
        bool test_part_one = matched(m, 3);
        bool test_part_two = matched(m, 4);
        bool test = test_part_one || test_part_two;
        if (!test || test_part_one) {
          std::int64_t result = 0;
          std::int64_t min_wait = std::numeric_limits<std::int64_t>::max();
          auto rest_of_line(match_view(m, 2, line));
          while (auto m = match(regex2, rest_of_line)) {
            if (matched(m, 1)) {
              auto view = match_view(m, 1, rest_of_line);
              std::int64_t id = string_to<std::int64_t>(view);
              std::int64_t wait = id - (arrival % id);
              if (wait < min_wait) {
                min_wait = wait;
                result = wait * id;
              }
            }
            rest_of_line = match_view(m, 2, rest_of_line);
          }
          if (test) {
            auto expected = string_to<std::int64_t>(match_view(m, 3, line));
            if (result != expected) {
              std::cout << "Fail, got " << result << ", expected " << expected
                        << std::endl;
            }
          } else {
            std::cout << "Day 13, Part One" << std::endl;
            std::cout << result << std::endl;
          }
        }
        if (!test || test_part_two) {
          std::int64_t r1 = 0;
          std::int64_t m1 = 1;
          std::int64_t r2 = 0;
          auto rest_of_line(match_view(m, 2, line));
          while (auto m = match(regex2, rest_of_line)) {
            if (matched(m, 1)) {
              auto view = match_view(m, 1, rest_of_line);
              std::int64_t m2 = string_to<std::int64_t>(view);
              // Find the gcd d of m1 and m2 and find integers u, v such that
              // d = u * m1 + v * m2, using the extended Euclidean algorithm.
              auto [gcd, coefficients] = egcd(m1, m2);
              auto [u, v] = coefficients;
              assert(gcd == 1); // Reminder: 'make CONFIG=debug' to enable.
              auto m = m1 * m2;
              // One of the steps of the constructive existence proof for the
              // Chinese remainder theorem.
              //   x = r1 * v * m2 + r2 * u * m1
              //     = r1 * (1 - u * m1) + r2 * u * m1
              // is congruent to r1 (mod m1),
              // and similarly x is congruent to r2 (mod m2).
              r1 = (modmul(v * m2, r1, m) + modmul(u * m1, r2 % m2, m)) % m;
              m1 = m;
            }
            rest_of_line = match_view(m, 2, rest_of_line);
            --r2;
          }
          if (r1 < 0) {
            r1 = r1 + m1;
          }
          auto result = r1;
          if (test) {
            auto expected = string_to<std::int64_t>(match_view(m, 4, line));
            if (result != expected) {
              std::cout << "Fail, got " << result << ", expected " << expected
                        << std::endl;
            }
          } else {
            std::cout << "Day 13, Part Two" << std::endl;
            std::cout << result << std::endl;
          }
        }
      }
    }
  }
}

int main() {
  for (auto filename : {"test/13", "input/13"}) {
    if (std::ifstream stream(filename); stream) {
      parts(stream);
    }
  }
  return 0;
}
