#include "precompiled.h"
#include "split.h"

auto regex1 = re::regex(R"(^(?:(\d+),(\d+)|([nesw]+))$)");
auto regex2 = re::regex(R"(^([ns]?[ew])(.*)$)");

//   nw   ne
//     \ /          -1 0 1 2
//  w --*-- e       -/-/-/-/-  -1
//     / \         -/-/-/-/-    0
//   sw   se      -/-/-/-/-     1
//

std::map<std::string, std::tuple<int, int>> directions = {
  {"e", {1, 0}},
  {"ne", {0, 1}},
  {"nw", {-1, 1}},
  {"w", {-1, 0}},
  {"sw", {0, -1}},
  {"se", {1, -1}},
};

using z = std::uint16_t;

void parts(std::istream &stream) {
  bool test = false;
  z result[2]{};
  z expected[2]{};

  // grid[x][y] = [color, neighbors]
  std::map<int, std::map<int, std::pair<bool, int>>> grid;

  for (std::string line; std::getline(stream, line);) {
    if (auto m = match(regex1, line)) {
      if (matched(m, 1)) {
        test = true;
        expected[0] = string_to<int>(match_view(m, 1, line));
        expected[1] = string_to<int>(match_view(m, 2, line));
      } else if (matched(m, 3)) {
        // Go to reference tile
        int x = 0;
        int y = 0;
        // Follow directions
        for (auto [direction] : split(match_view(m, 3, line), regex2)) {
          auto [dx, dy] = directions[std::string(direction)];
          x += dx;
          y += dy;
        }
        // Flip tile
        auto &[color, neighbors] = grid[x][y];
        color = !color;
      }
    }
  }

  // Part One result, count tiles with 'color == true'
  for (const auto &[x, diagonal] : grid) {
    for (const auto &[y, tile] : diagonal) {
      auto [color, neighbors] = tile;
      result[0] += color;
    }
  }

  // Part Two
  for (int i = 0; i != 100; ++i) {
    // For each existing tile...
    for (auto &[x, diagonal] : grid) {
      for (auto &[y, tile] : diagonal) {
        // ...reset neighbor count to zero
        auto &[color, neighbors] = tile;
        neighbors = 0;
      }
    }
    // For each existing tile...
    for (const auto &[x, diagonal] : grid) {
      for (const auto &[y, tile] : diagonal) {
        auto [color, neighbors] = tile;
        if (color) {
          //...increment neighbor count of each neighbor
          for (const auto &[name, direction] : directions) {
            auto [dx, dy] = direction;
            auto &[neighbor_color, neighbor_neighbors] = grid[x + dx][y + dy];
            ++neighbor_neighbors;
          }
        }
      }
    }
    // For each existing tile...
    for (auto &[x, diagonal] : grid) {
      for (auto &[y, tile] : diagonal) {
        // ...apply rules of life and death
        auto &[color, neighbors] = tile;
        color = (color && neighbors == 1) || neighbors == 2;
      }
    }
  }

  // Part Two result, count tiles with 'color == true' again
  for (const auto &[x, diagonal] : grid) {
    for (const auto &[y, tile] : diagonal) {
      const auto &[color, neighbors] = tile;
      result[1] += color;
    }
  }

  for (int part = 0; part != 2; ++part) {
    if (test) {
      if (result[part] != expected[part]) {
        std::cout << "Fail, got " << result[part] << ", expected "
                  << expected[part] << std::endl;
        std::exit(1);
      }
    } else {
      std::cout << result[part] << std::endl;
    }
  }
}

int main() {
  for (auto filename : {"test/24", "input/24"}) {
    if (std::ifstream stream(filename); stream) {
      parts(stream);
    }
  }
  return 0;
}
