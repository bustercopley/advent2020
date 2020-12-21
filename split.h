// -*- C++ -*-
#ifndef split_h
#define split_h

struct split_sentinel {};

template <std::size_t N = 1> struct split_iterator {
  split_iterator(std::string_view subject, const re::code &regex)
    : regex(regex), subject(subject), m(match(regex, subject)) {}

  bool operator!=(split_sentinel) const { return static_cast<bool>(m); }
  auto operator*() const {
    std::array<std::string_view, N> result;
    for (std::size_t n = 0; n != N; ++n) {
      result[n] = match_view(m, n + 1, subject);
    }
    return result;
  }
  split_iterator &operator++() {
    subject = match_view(m, N + 1, subject);
    m = match(regex, subject);
    return *this;
  }

private:
  const re::code &regex;
  std::string_view subject;
  re::match_data m;
};

template <std::size_t N = 1> struct split {
  split(std::string_view subject, const re::code &regex)
    : regex(regex), subject(subject) {}

  split_iterator<N> begin() { return split_iterator<N>(subject, regex); }
  split_sentinel end() { return split_sentinel{}; }

private:
  const re::code &regex;
  std::string_view subject;
};

#endif
