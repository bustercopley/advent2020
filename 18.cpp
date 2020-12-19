#include "precompiled.h"

auto regex1 = re::regex(R"(^([^:]*)(?::(\d+),(\d+))?$)");
auto regex2 = re::regex(R"(^ *(?:(\+)|(\*)|(\()|(\))|(\d+))(.*)$)");

using ll = std::int64_t;

enum class token_type {
  plus,   // 0
  times,  // 1
  lparen, // 2
  rparen, // 3
  number, // 4
};

struct term {
  ll number_value;
  token_type opcode;
};

struct oper {
  token_type opcode;
  int level;
};

void pop_to(
  std::vector<oper> &opstack, std::vector<term> &expression, int level) {
  while (!std::empty(opstack) && level <= opstack.back().level) {
    expression.push_back(term{0, opstack.back().opcode});
    opstack.pop_back();
  }
}

void part(std::istream &stream, int part) {
  bool test = false;
  ll result = 0;
  ll expected = 0;
  ll value = 0;

  int precedence[2]{};
  if (part == 2) {
    // lower the precedence of + from 0 to 1
    precedence[0] = 1;
  }

  for (std::string line; std::getline(stream, line);) {
    if (auto m = match(regex1, line)) {
      if (matched(m, 2)) {
        test = true;
        expected = string_to<ll>(match_view(m, part == 1 ? 2 : 3, line));
      }

      auto rest = match_view(m, 1, line);
      std::vector<term> expression;
      std::vector<oper> opstack;
      int level = 1;

      // Use a stack of operators to translate the infix expression to postfix
      while (auto m = match(regex2, rest)) {

        token_type tt;
        if (matched(m, 1)) {
          tt = token_type::plus;
        } else if (matched(m, 2)) {
          tt = token_type::times;
        } else if (matched(m, 3)) {
          tt = token_type::lparen;
        } else if (matched(m, 4)) {
          tt = token_type::rparen;
        } else if (matched(m, 5)) {
          tt = token_type::number;
        } else {
          std::abort();
        }

        switch (tt) {
        case token_type::plus:
          pop_to(opstack, expression, 2 * level + precedence[0]);
          opstack.push_back(oper{token_type::plus, 2 * level + precedence[0]});
          break;

        case token_type::times:
          pop_to(opstack, expression, 2 * level + precedence[1]);
          opstack.push_back(oper{token_type::times, 2 * level + precedence[1]});
          break;

        case token_type::lparen:
          ++level;
          break;

        case token_type::rparen:
          pop_to(opstack, expression, 2 * level);
          --level;
          break;

        case token_type::number:
          expression.push_back(
            term{string_to<ll>(match_view(m, 5, rest)), token_type::number});
          break;
        }

        rest = match_view(m, 6, rest);
      }

      pop_to(opstack, expression, -1);

      for (auto x : expression) {
        if (x.opcode == token_type::number) {
        } else if (x.opcode == token_type::plus) {
        } else if (x.opcode == token_type::times) {
        } else {
          std::abort();
        }
      }

      // Use a stack of operands to evaluate the postfix expression
      std::vector<ll> stack;
      for (auto x : expression) {

        if (x.opcode == token_type::number) {
        } else if (x.opcode == token_type::plus) {
        } else if (x.opcode == token_type::times) {
        } else {
          std::abort();
        }

        switch (x.opcode) {
        case token_type::number:
          stack.push_back(x.number_value);
          break;

        case token_type::plus: {
          auto a = stack.back();
          stack.pop_back();
          auto b = stack.back();
          stack.pop_back();
          stack.push_back(a + b);
          break;
        }

        case token_type::times: {
          auto a = stack.back();
          stack.pop_back();
          auto b = stack.back();
          stack.pop_back();
          stack.push_back(a * b);
          break;
        }

        default:
          std::abort();
        }
      }

      value = stack.back();
      stack.pop_back();
      assert(std::empty(stack));

      if (test) {
        if (value != expected) {
          std::cout << "Fail, got " << value << ", expected " << expected
                    << std::endl;
          std::exit(1);
        }
      } else {
        result += value;
      }
    }
  }
  if (!test) {
    std::cout << result << std::endl;
  }
}

int main() {
  std::cout << "Day 18, Part One" << std::endl;
  for (auto filename : {"test/18", "input/18"}) {
    if (std::ifstream stream(filename); stream) {
      part(stream, 1);
    }
  }
  std::cout << "Day 18, Part Two" << std::endl;
  for (auto filename : {"test/18", "input/18"}) {
    if (std::ifstream stream(filename); stream) {
      part(stream, 2);
    }
  }
  return 0;
}
