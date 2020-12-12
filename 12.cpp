#include "precompiled.h"

auto regex = re::regex(R"(^(?:[NSEWLRF](\d+))|(\d+),(\d+)$)");

void rotate(int &dx, int &dy, int angle) {
  const int ms[3][4] = {{0, -1, 1, 0}, {-1, 0, 0, -1}, {0, 1, -1, 0}};
  const auto &m = ms[angle - 1];
  int x = m[0] * dx + m[1] * dy;
  int y = m[2] * dx + m[3] * dy;
  dx = x;
  dy = y;
}

void part(std::istream &stream, int part) {
  bool test = false;
  std::size_t result = 0;
  std::size_t expected = 0;

  int x = 0;
  int y = 0;
  int d = 0;
  int dx = 10;
  int dy = 1;
  for (std::string line; std::getline(stream, line);) {
    if (auto m = re::match(regex, line)) {
      if (matched(m, 2)) {
        test = true;
        expected = string_to<std::size_t>(match_view(m, part + 1, line));
      } else {
        int a = string_to<int>(match_view(m, 1, line));
        int c = line[0];
        if (part == 1) {
          if (c == 'F') {
            c = "ENWS"[d];
          }
          switch (c) {
          case 'N': y += a; break;
          case 'S': y -= a; break;
          case 'E': x += a; break;
          case 'W': x -= a; break;
          case 'L': d = (d + a / 90) % 4; break;
          case 'R': d = (d + 4 - a / 90) % 4; break;
          default:
            std::cout << "Parse error" << std::endl;
            return;
          }
        }
        else {
          switch (c) {
          case 'N': dy += a; break;
          case 'S': dy -= a; break;
          case 'E': dx += a; break;
          case 'W': dx -= a; break;
          case 'L': rotate(dx, dy, a / 90); break;
          case 'R': rotate(dx, dy, 4 - a / 90); break;
          case 'F': x += a * dx; y += a * dy; break;
          default:
            std::cout << "Parse error" << std::endl;
            return;
          }
        }
      }
    }
  }
  result = std::abs(x) + std::abs(y);

  if (test) {
    if (result != expected) {
      std::cout << "Fail, got " << result << ", expected " << expected
                << std::endl;
      std::exit(1);
    }
  } else {
    std::cout << "Result " << result << std::endl;
  }
}

int main() {
  std::cout << "Day 12, Part One" << std::endl;
  for (auto filename : {"test/12", "input/12"}) {
    if (std::ifstream stream(filename); stream) {
      part(stream, 1);
    }
  }

  std::cout << "Day 12, Part Two" << std::endl;
  for (auto filename : {"test/12", "input/12"}) {
    if (std::ifstream stream(filename); stream) {
      part(stream, 2);
    }
  }

  return 0;
}
