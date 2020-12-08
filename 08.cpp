#include "precompiled.h"
#include "symbols.h"

auto regex =
  re::regex(R"(^(?:(?:(acc)|(jmp)|nop) (?:\+|(-))(\d+)|(\d+),(\d+))$)");

enum opcode {
  acc,
  jmp,
  nop,
};

struct op {
  opcode code;
  int operand;
  std::size_t visited;
};

std::pair<bool, int> run(std::vector<op> program, std::size_t time) {
  std::size_t pc = 0;
  int accumulator = 0;
  while (pc < std::size(program) && program[pc].visited != time) {
    auto &[code, operand, visited] = program[pc];
    switch (code) {
    case acc:
      accumulator += operand;
      ++pc;
      break;
    case jmp:
      pc += operand;
      break;
    case nop:
      ++pc;
      break;
    }
    visited = time;
  }
  return {pc >= std::size(program), accumulator};
}

void parts(std::istream &stream, int part) {
  bool test = false;
  int result = 0;
  int expected = 0;

  std::vector<op> program;

  for (std::string line; std::getline(stream, line);) {
    if (auto m = re::match(regex, line)) {
      if (matched(m, 4)) {
        opcode code = matched(m, 1) ? acc : matched(m, 2) ? jmp : nop;
        int operand =
          (matched(m, 3) ? -1 : 1) * string_to<int>(match_view(m, 4, line));
        program.push_back(op{code, operand, 0});
      } else {
        test = true;
        expected =
          string_to<std::size_t>(match_view(m, part == 1 ? 5 : 6, line));
      }
    }
  }

  if (part == 1) {
    result = run(program, 1).second;
  } else {
    for (auto &[code, operand, visited] : program) {
      visited = std::numeric_limits<std::size_t>::max();
    }
    for (std::size_t i = 0; i != std::size(program); ++i) {
      if (program[i].code != acc) {
        auto &changed_code = program[i].code;
        changed_code = changed_code == jmp ? nop : jmp;
        auto[halt, accumulator] = run(program, i);
        if (halt) {
          result = accumulator;
          break;
        }
        changed_code = changed_code == jmp ? nop : jmp;
      }
    }
  }

  if (test) {
    if (result != expected) {
      std::cout << "Fail, got " << result << ", expected " << expected
                << std::endl;
    }
  } else {
    std::cout << result << std::endl;
  }
}

int main() {
  std::cout << "Day 8, Part One" << std::endl;
  for (auto filename : {"test/08", "input/08"}) {
    if (std::ifstream stream(filename); stream) {
      parts(stream, 1);
    }
  }

  std::cout << "Day 8, Part Two" << std::endl;
  for (auto filename : {"test/08", "test/08b", "input/08"}) {
    if (std::ifstream stream(filename); stream) {
      parts(stream, 2);
    }
  }

  return 0;
}
