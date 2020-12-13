// -*- C++ -*-
#ifndef symbols_h
#define symbols_h

#include <cstddef>
#include <map>
#include <string>
#include <vector>

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
  const std::string &operator[](std::size_t index) const {
    return names[index]->first;
  }
  auto operator[](const std::string &n) { return intern(std::string(n)); }
  auto operator[](std::string &&name) { return intern(std::move(name)); }
  auto size() const { return std::size(names); }
};

#endif
