#include "precompiled.h"

auto regex1 = re::regex(R"(^(?:(\d+),(\d+)|Tile (\d+):|([.#]+)|)$)");

// 'subscript(sequence, i, j, k)' means 'sequence[i][j][k]', etc.
auto &subscript(auto &atom) { return atom; }
auto &subscript(auto &sequence, std::size_t i, auto... j) {
  return subscript(sequence[i], j...);
}

int from_binary(int length, const auto &sequence, auto... i) {
  int a = 0;
  int b = 0;
  for (const auto &element : sequence) {
    bool bit = subscript(element, i...) == '#';
    a = (a << 1) | bit;
    b = (b | (bit << length)) >> 1;
  }
  return std::min(a, b);
}

using image = std::vector<std::string>;
using edge_map = std::map<int, std::tuple<int, std::array<int, 2>>>;
using tile_map = std::map<int, std::tuple<std::array<int, 4>, image>>;

std::tuple<bool, int, int> find_tile(
  const edge_map &edges, const tile_map &tiles, int tile_id, int edge_index) {
  auto tile_iter = tiles.find(tile_id);
  auto &[tile_edges, tile_image] = tile_iter->second;
  int edge = tile_edges[edge_index];
  auto edge_iter = edges.find(edge);
  auto &[edge_size, edge_data] = edge_iter->second;
  if (edge_size == 2) {
    int other_tile_id = edge_data[edge_data[0] == tile_id];
    auto other_tile_iter = tiles.find(other_tile_id);
    auto &[other_tile_edges, other_tile_image] = other_tile_iter->second;
    auto other_edge_iter =
      std::find(std::begin(other_tile_edges), std::end(other_tile_edges), edge);
    int other_edge_index = other_edge_iter - std::begin(other_tile_edges);
    return {true, other_tile_id, other_edge_index};
  } else {
    return {false, 0, 0};
  }
}

void parts(std::istream &stream) {
  bool test = false;
  std::int64_t result[2]{};
  std::int64_t expected[2]{};

  edge_map edges;
  tile_map tiles;
  std::vector<std::string> current_tile;
  int id = 0;

  for (std::string line; std::getline(stream, line);) {
    if (auto m = match(regex1, line)) {
      if (matched(m, 1)) {
        test = true;
        expected[0] = string_to<std::int64_t>(match_view(m, 1, line));
        expected[1] = string_to<std::int64_t>(match_view(m, 2, line));
      } else if (matched(m, 3)) {
        current_tile.clear();
        id = string_to<int>(match_view(m, 3, line));
      } else if (matched(m, 4)) {
        current_tile.push_back(std::move(line));
      } else {
        auto rows = std::size(current_tile);
        auto cols = std::size(current_tile[0]);
        auto &[tile_edges, image] = tiles[id];
        image = std::move(current_tile);
        tile_edges = {{
          // top, left, bottom, right
          from_binary(rows, image[0]),
          from_binary(cols, image, 0),
          from_binary(rows, image[rows - 1]),
          from_binary(cols, image, cols - 1),
        }};
        for (auto e : tile_edges) {
          auto &[size, data] = edges[e];
          data[size++] = id;
        }
      }
    }
  }

  int current = id;
  int rotate = 0;
  int flip = 0;
  result[0] = 1;
  for (const auto &[id, value] : tiles) {
    auto &[tile_edges, image] = value;
    std::uint32_t bits = 0;
    for (auto e : tile_edges) {
      auto &[size, data] = edges[e];
      bits = (bits << 1) | (size == 2);
    }
    if (std::popcount(bits) == 2) {
      result[0] *= (std::int64_t)id;
      current = id;
      switch (bits) {
      case 3: // edges 2 and 3 are shared
        rotate = 0;
        break;
      case 6: // edges 1 and 2 are shared
        rotate = 1;
        break;
      case 12: // edges 0 and 1 are shared
        rotate = 2;
        break;
      case 9: // edges 3 and 0 are shared
        rotate = 3;
        break;
      }
    }
  }

  if (test) {
    if (result[0] != expected[0]) {
      std::cout << "Fail Part One, got " << result << ", expected " << expected
                << std::endl;
      std::exit(1);
    }
  } else {
    std::cout << result[0] << std::endl;
  }

  // reassemble the image, step 1 - build 2x2 matrix of [id, rotate, flip]
  std::vector<std::vector<std::tuple<int, int, int>>> image_tiles;
  int i = 0;
  while (true) {
    image_tiles.emplace_back();
    auto &row = image_tiles.back();
    row.emplace_back(current, rotate, flip);
    int j = 0;
    while (true) {
      // find the tile that shares our right edge, if any
      int right = ((flip ? 5 : 3) - rotate) % 4;
      auto [ok1, next, left] = find_tile(edges, tiles, current, right);
      if (!ok1) {
        // not found, end of row
        break;
      }
      // flip it unless its upper edge matches the tile above, if any
      auto [ok2, up, bottom] = find_tile(edges, tiles, next, (left + 3) % 4);
      flip = (i == 0 && ok2) ||
             (i != 0 && (!ok2 || up != std::get<0>(image_tiles[i - 1][j + 1])));
      rotate = ((flip ? 3 : 5) - left) % 4;
      current = next;
      row.emplace_back(current, rotate, flip);
      ++j;
    }
    // find the tile that shares our bottom edge, if any
    int bottom;
    {
      auto [first, rotate, flip] = row[0];
      current = first;
      bottom = (6 - rotate) % 4;
    }
    auto [ok1, down, top] = find_tile(edges, tiles, current, bottom);
    if (!ok1) {
      // not found, end of image
      break;
    }
    // flip it if its left edge matches any tile
    auto [ok2, prev, right] = find_tile(edges, tiles, down, (top + 1) % 4);
    current = down;
    flip = ok2;
    rotate = (4 - top) % 4;
    ++i;
  }

  // reassemble the image, step 2 - build a vector of strings
  int tile_side = std::size(std::get<1>(std::begin(tiles)->second));
  std::vector<std::string> image(std::size(image_tiles) * (tile_side - 2),
    std::string(std::size(image_tiles[0]) * (tile_side - 2), ' '));

  static const int matrices[2][4][6] = {
    {
      {1, 0, 0, 1, 0, 0},
      {0, -1, 1, 0, 0, 1},
      {-1, 0, 0, -1, 1, 1},
      {0, 1, -1, 0, 1, 0},
    },
    {
      {1, 0, 0, -1, 0, 1},
      {0, -1, -1, 0, 1, 1},
      {-1, 0, 0, 1, 1, 0},
      {0, 1, 1, 0, 0, 0},
    },
  };

  for (std::size_t i = 0; i != std::size(image_tiles); ++i) {
    for (std::size_t j = 0; j != std::size(image_tiles[i]); ++j) {
      auto [id, rotate, flip] = image_tiles[i][j];
      auto &[tile_edges, tile_image] = tiles[id];
      auto [a, b, c, d, e, f] = matrices[flip][rotate];
      for (int ii = 0; ii != tile_side - 2; ++ii) {
        for (int jj = 0; jj != tile_side - 2; ++jj) {
          int iii = a * (ii + 1) + c * (jj + 1) + e * (tile_side - 1);
          int jjj = b * (ii + 1) + d * (jj + 1) + f * (tile_side - 1);
          image[i * (tile_side - 2) + ii][j * (tile_side - 2) + jj] =
            tile_image[iii][jjj];
        }
      }
    }
  }

  // search the image for sea monsters
  std::vector<std::string> monster = {
    "                  # ",
    "#    ##    ##    ###",
    " #  #  #  #  #  #   ",
  };

  bool found = false;
  for (int flip = 0; !found && flip != 2; ++flip) {
    for (int rotate = 0; !found && rotate != 4; ++rotate) {
      std::size_t height = std::size(monster);
      std::size_t width = std::size(monster[0]);
      for (std::size_t i = 0; i != std::size(image) - height; ++i) {
        for (std::size_t j = 0; j != std::size(image[0]) - width; ++j) {
          bool here = true;
          for (std::size_t ii = 0; here && ii != height; ++ii) {
            for (std::size_t jj = 0; here && jj != width; ++jj) {
              if (monster[ii][jj] == '#' && image[i + ii][j + jj] == '.') {
                here = false;
              }
            }
          }
          if (here) {
            found = true;
            for (std::size_t ii = 0; ii != height; ++ii) {
              for (std::size_t jj = 0; jj != width; ++jj) {
                if (monster[ii][jj] == '#') {
                  image[i + ii][j + jj] = 'O';
                }
              }
            }
          }
        }
      }

      // rotate the monster
      std::vector<std::string> new_monster(width, std::string(height, '\0'));
      for (std::size_t i = 0; i != height; ++i) {
        for (std::size_t j = 0; j != width; ++j) {
          new_monster[j][height - 1 - i] = monster[i][j];
        }
      }
      monster = new_monster;
    }

    // flip the monster
    for (auto &line : monster) {
      std::reverse(std::begin(line), std::end(line));
    }
  }

  if constexpr (false) {
    // dump image with sea monsters highlighted
    for (int i = 0; i != (int)std::size(image); ++i) {
      if (!(i % 8))
        std::cout << "\n";
      for (int j = 0; j != (int)std::size(image[i]); ++j) {
        if (!(j % 8))
          std::cout << " ";
        int ii [[maybe_unused]] = std::size(image) - 1 - i;
        int jj [[maybe_unused]] = std::size(image) - 1 - j;
        std::cout << subscript(image, i, j);
      }
      std::cout << "\n";
    }
    std::cout << std::endl;
  }

  // count non-monster rough sea squares
  for (const auto &s : image) {
    result[1] += std::count(std::begin(s), std::end(s), '#');
  }

  if (test) {
    if (result[1] != expected[1]) {
      std::cout << "Fail Part Two, got " << result[1] << ", expected "
                << expected[1] << std::endl;
      std::exit(1);
    }
  } else {
    std::cout << result[1] << std::endl;
  }
}

int main() {
  for (auto filename : {"test/20", "input/20"}) {
    if (std::ifstream stream(filename); stream) {
      parts(stream);
    }
  }
  return 0;
}
