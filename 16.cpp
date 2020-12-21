#include "precompiled.h"
#include "split.h"

auto regex1 = re::regex(
  R"(^(?:expected: (\d+),(\d+)|([a-z ]+):(?: (\d+)-(\d+) or (\d+)-(\d+))?|\d+(?:,\d+)*)$)");
auto regex2 = re::regex(R"(^(\d+)(?:,|$)(.*)$)");

void parts(std::istream &stream, int part) {
  bool test = false;
  std::int64_t result = 0;
  int expected = 0;
  int state = 0;

  std::vector<std::string> fields;
  std::vector<std::array<int, 4>> ranges;
  std::vector<int> my_ticket;
  std::vector<std::vector<int>> invalid_fields;

  for (std::string line; std::getline(stream, line);) {
    if (auto m = match(regex1, line)) {
      if (matched(m, 1)) {
        test = true;
        expected = string_to<int>(match_view(m, part, line));
      }
      if (matched(m, 4)) {
        fields.emplace_back(match_string(m, 3));
        ranges.push_back(
          std::array<int, 4>{string_to<int>(match_view(m, 4, line)),
            string_to<int>(match_view(m, 5, line)),
            string_to<int>(match_view(m, 6, line)),
            string_to<int>(match_view(m, 7, line))});
      } else if (matched(m, 3)) {
        if (match_view(m, 3, line) == "nearby tickets") {
          state = 1;
          invalid_fields.resize(std::size(ranges));
          for (auto &row : invalid_fields) {
            row.resize(std::size(my_ticket));
          }
          assert(std::size(ranges) == std::size(my_ticket));
        }
      } else {
        std::vector<int> ticket;
        for (auto [sv] : split(std::string_view(line), regex2)) {
          auto value = string_to<int>(sv);
          ticket.push_back(value);
        }
        if (state == 0) {
          ticket.swap(my_ticket);
        } else {
          bool valid = true;
          for (auto value : ticket) {
            auto iter = std::find_if(std::begin(ranges), std::end(ranges),
              [value](const std::array<int, 4> &range) {
                auto [a, b, c, d] = range;
                return (a <= value && value <= b) || (c <= value && value <= d);
              });
            if (iter == std::end(ranges)) {
              // Value is not valid for any field
              valid = false;
              if (part == 1) {
                result += value;
              }
            }
          }
          if (part == 2 && valid) {
            for (int j = 0; auto value : ticket) {
              for (int i = 0; auto [a, b, c, d] : ranges) {
                if (!((a <= value && value <= b) ||
                      (c <= value && value <= d))) {
                  invalid_fields[i][j] = 1;
                }
                ++i;
              }
              ++j;
            }
          }
        }
      }
    }
  }
  if (part == 2) {
    std::vector<int> field_position(std::size(my_ticket));
    bool done = false;
    while (!done) {
      done = true;
      for (int i = 0; auto &row : invalid_fields) {
        int possible_positions = 0;
        int position;
        for (std::size_t j = 0; j != std::size(row); ++j) {
          if (!row[j]) {
            ++possible_positions;
            position = j;
          }
        }
        if (possible_positions == 1) {
          field_position[i] = position;
          for (auto &cell : row) {
            cell = 1;
          }
          for (auto &row : invalid_fields) {
            row[position] = 1;
          }
          done = false;
        }
        ++i;
      }
    }
    result = 1;
    for (int i = 0; const auto &name : fields) {
      if (name.substr(0, 9) == "departure") {
        result *= my_ticket[field_position[i]];
      }
      ++i;
    }
  }
  if (test) {
    if (part == 1 && result != expected) {
      std::cout << "Fail, got " << result << ", expected " << expected
                << std::endl;
      std::exit(1);
    }
  } else {
    std::cout << result << std::endl;
  }
}

int main() {
  for (auto filename : {"test/16", "input/16"}) {
    if (std::ifstream stream(filename); stream) {
      parts(stream, 1);
    }
  }

  for (auto filename : {"test/16", "input/16"}) {
    if (std::ifstream stream(filename); stream) {
      parts(stream, 2);
    }
  }
  return 0;
}
