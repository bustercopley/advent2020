// -*- C++ -*-
#ifndef string_to_h
#define string_to_h

#include <charconv>
#include <concepts>
#include <span>
#include <string_view>

// Wrapper for from_chars with signature like stoi etc.
// Prints error message and throws on failure.

// auto i = string_to<int>(s); // s a string, string_view or span of chars

template <std::integral Integer, typename String>
Integer string_to(String &&s, std::size_t *pos = nullptr,
  const std::experimental::source_location &location =
    std::experimental::source_location::current()) {

  const char *data = std::data(s);
  std::size_t size = std::size(s);
  const char *sentinel = (size == std::dynamic_extent ? nullptr : data + size);

  Integer result;
  auto [p, e] = std::from_chars(data, sentinel, result);

  if (e == std::errc::invalid_argument) {
    std::cerr << location.file_name() << ":" << location.line()
              << ": invalid argument\n";
    throw std::invalid_argument{"invalid_argument"};
  }

  if (e == std::errc::result_out_of_range) {
    std::cerr << location.file_name() << ":" << location.line()
              << ": out of range\n";
    throw std::out_of_range{"out_of_range"};
  }

  if (pos) {
    *pos = p - data;
  }
  return result;
}

#endif
