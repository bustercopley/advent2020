// -*- C++ -*-
#ifndef re_h
#define re_h

// Crappy wrapper for PCRE2.

// Interface:

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
#include <memory>
#include <string>
#include <string_view>

namespace re {

#define SRCLOCDEF                                                              \
  const std::experimental::source_location &location =                         \
    std::experimental::source_location::current()

struct code_deleter {
  void operator()(pcre2_code *p) { pcre2_code_free(p); }
};

struct match_data_deleter {
  void operator()(pcre2_match_data *p) { pcre2_match_data_free(p); }
};

using code = std::unique_ptr<pcre2_code, code_deleter>;
using match_data = std::unique_ptr<pcre2_match_data, match_data_deleter>;

code regex(const char *pattern, SRCLOCDEF);
match_data match(const code &re, const std::string &subject, SRCLOCDEF);
match_data match(const code &re, std::string_view subject, SRCLOCDEF);
bool matched(const match_data &data, int n, SRCLOCDEF);
std::size_t matched_length(const match_data &data, int n, SRCLOCDEF);
std::string match_string(const match_data &data, int n, SRCLOCDEF);

std::pair<std::size_t, std::size_t> match_bounds(
  const re::match_data &data, int n, SRCLOCDEF);

std::string_view match_view(
  const match_data &data, int n, std::string_view subject, SRCLOCDEF);

std::string_view match_view(
  const match_data &data, int n, const std::string &subject, SRCLOCDEF);

#undef SRCLOCDEF

} // namespace re

#endif
