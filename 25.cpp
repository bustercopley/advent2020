#include "precompiled.h"
#include "split.h"
#include "symbols.h"

using ll = std::int64_t;

auto regex1 = re::regex(R"(^(?:expected (\d+)|(\d+))$)");

ll modexp(ll base, ll exponent, ll modulus = 20201227) {
  // repeated squaring
  ll value = 1;
  while (exponent) {
    if (exponent & 1) {
      value = (value * base) % modulus;
    }
    base = (base * base) % modulus;
    exponent >>= 1;
  }
  return value;
}

ll modlog(ll base, ll exponential, ll modulus = 20201227) {
  // brute force search (Happy Christmas)
  int exponent = 0;
  while (modexp(base, exponent, modulus) != exponential) {
    ++exponent;
  }
  return exponent;
}

void solve(std::istream &&stream) {
  if (!stream) {
    return;
  }

  bool test = false;
  ll result = 0;
  ll expected = 0;
  ll cardkey = 0;
  ll doorkey = 0;

  for (std::string line; std::getline(stream, line);) {
    if (auto m = match(regex1, line)) {
      if (matched(m, 1)) {
        test = true;
        expected = string_to<ll>(match_view(m, 1, line));
      } else {
        assert(!doorkey || !cardkey);
        (!cardkey ? cardkey : doorkey) = string_to<ll>(match_view(m, 2, line));
      }
    }
  }

  result = modexp(doorkey, modlog(7, cardkey));
  // result = modexp(cardkey, modlog(7, doorkey));

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
  for (auto filename : {"test/25", "input/25"}) {
    solve(std::ifstream(filename));
  }
  return 0;
}
