// -*- C++ -*-
#ifndef re_h
#define re_h

// Crappy wrapper for PCRE2.

// Interface:

// re::code re::regex(const char *pattern);
// re::match_data re::match(code, const char *subject);
// bool re::matched(re::match_data, int n);
// std::string re::match_string(re::match_data match_data, int n);

// Usage:

// void f(std::istream &stream) {
//   auto regex = re::regex(R"(^(\d+) + (\d+)(?::(\d+)))");
//
//   std::string line;
//   while (std::getline(stream, line)) {
//     if (auto match_data = re::match(regex, line)) {
//       int a = std::stoi(re::match_string(match_data, 1));
//       int b = std::stoi(re::match_string(match_data, 2));
//       int sum = a + b;
//       if (re::matched(match_data, 3)) {
//         int expected = std::stoi(re::match_string(match_data, 3));
//         if (sum != expected) {
//           std::cout << "Fail, got " << sum << ", expected " << expected
//                     << std::endl;
//         }
//       } else {
//         std::cout << sum << std::endl;
//       }
//     }
//   }
// }

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#include <experimental/source_location>

namespace re {

using std::experimental::source_location;

struct code_deleter {
  void operator()(pcre2_code *p) { pcre2_code_free(p); }
};

struct match_data_deleter {
  void operator()(pcre2_match_data *p) { pcre2_match_data_free(p); }
};

using code = std::unique_ptr<pcre2_code, code_deleter>;
using match_data = std::unique_ptr<pcre2_match_data, match_data_deleter>;

code regex(const char *pattern,
  const source_location &location = source_location::current());

match_data match(const code &re, const std::string &subject,
  const source_location &location = source_location::current());

std::pair<bool, std::size_t> get_matched(
  const re::match_data &data, int n, const re::source_location &location);

bool matched(const match_data &data, int n,
  const source_location &location = source_location::current());

std::size_t matched_length(const match_data &data, int n,
  const source_location &location = source_location::current());

std::string match_string(const match_data &data, int n,
  const source_location &location = source_location::current());
} // namespace re

#endif
