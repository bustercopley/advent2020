#include "precompiled.h"

auto regex = re::regex(R"(^(\d+),(\d+)$)");

using grid_t = std::vector<std::string>;

std::size_t nearby(const grid_t &grid, std::size_t i, std::size_t j, int part) {
  std::size_t rows = std::size(grid);
  std::size_t cols = std::size(grid[i]);
  std::size_t count = 0;

  for (int di = -1; di != 2; ++di) {
    for (int dj = -1; dj != 2; ++dj) {
      if (di || dj) {
        auto ii = i, jj = j;
        do {
          ii += di;
          jj += dj;
        } while (part == 2 && ii < rows && jj < cols && grid[ii][jj] == '.');
        count += ii < rows && jj < cols && grid[ii][jj] == '#';
      }
    }
  }
  return count;
}

bool step(const grid_t &grid, grid_t &next, int part) {
  bool result = false;
  next = grid;

  for (std::size_t i = 0; i != std::size(grid); ++i) {
    for (std::size_t j = 0; j != std::size(grid[i]); ++j) {
      auto count = nearby(grid, i, j, part);

      if (grid[i][j] == 'L') {
        if (count == 0) {
          next[i][j] = '#';
          result = true;
        }
      } else if (grid[i][j] == '#') {
        if (count >= (part == 1 ? 4 : 5)) {
          next[i][j] = 'L';
          result = true;
        }
      }
    }
  }
  return result;
}

void part(std::istream &stream, int part) {
  bool test = false;
  std::size_t result = 0;
  std::size_t expected = 0;
  grid_t grid;

  // read grid
  for (std::string line; std::getline(stream, line);) {
    if (auto m = re::match(regex, line)) {
      test = true;
      expected = string_to<std::size_t>(match_view(m, part, line));
    } else {
      grid.push_back(line);
    }
  }

  // step until stable
  for (grid_t next; step(grid, next, part);) {
    std::swap(grid, next);
  }

  // count nonempty seats
  for (auto line: grid) {
    for (auto cell: line) {
      result += (cell == '#');
    }
  }

  if (test) {
    if (result != expected) {
      std::cout << "Fail, got " << result << ", expected " << expected
                << std::endl;
    }
  } else {
    std::cout << "Result " << result << std::endl;
  }
}

int main() {
  std::cout << "Day 11, Part One" << std::endl;
  for (auto filename : {"test/11", "input/11"}) {
    if (std::ifstream stream(filename); stream) {
      part(stream, 1);
    }
  }

  std::cout << "Day 11, Part Two" << std::endl;
  for (auto filename : {"test/11", "input/11"}) {
    if (std::ifstream stream(filename); stream) {
      part(stream, 2);
    }
  }

  return 0;
}
