#include "precompiled.h"

auto regex1 = re::regex(R"(^(?:(\d+),(\d+)|Player \d:|(\d+))$)");

using z = std::uint16_t;

void parts(std::istream &stream) {
  bool test = false;
  z result[2]{};
  z expected[2]{};

  using state = std::array<std::vector<z>, 2>;
  state hands;
  z hand = 0;

  for (std::string line; std::getline(stream, line);) {
    if (auto m = match(regex1, line)) {
      if (matched(m, 1)) {
        test = true;
        expected[0] = string_to<int>(match_view(m, 1, line));
        expected[1] = string_to<int>(match_view(m, 2, line));
      } else if (matched(m, 3)) {
        assert(hand != 0);
        hands[hand - 1].push_back(string_to<z>(match_view(m, 3, line)));
      } else {
        ++hand;
        assert(hand != 3);
      }
    }
  }

  // [hands, history, round, game]
  std::vector<std::tuple<state, std::vector<state>, z, z>> stack;
  stack.push_back({hands, {}, 0, 0});

  // Part One

  z winner = 0;
  bool duplicate = false;
  std::vector<state> history;

  while (!std::empty(hands[0]) && !std::empty(hands[1])) {
    duplicate = std::ranges::find(history, hands) != std::end(history);
    if (duplicate) {
      break;
    }
    history.push_back(hands);
    winner = hands[1][0] > hands[0][0];
    hands[winner].push_back(hands[winner][0]);
    hands[winner].push_back(hands[!winner][0]);
    hands[0].erase(std::begin(hands[0]));
    hands[1].erase(std::begin(hands[1]));
  }

  if (!duplicate) {
    // calculate score
    for (z place = 1; auto face : std::views::reverse(hands[winner])) {
      result[0] += face * place;
      ++place;
    }
  }

  if (test) {
    if (result[0] != expected[0]) {
      std::cout << "Fail, got " << result[0] << ", expected " << expected[0]
                << std::endl;
      std::exit(1);
    }
  } else {
    std::cout << result[0] << std::endl;
  }

  // Part Two

  constexpr bool verbose = false;
  if constexpr (verbose) {
    if (test) {
      std::cout << "Part Two\n\n=== Game 1 ===" << std::endl;
    }
  }

  hands = std::move(std::get<0>(stack.back()));
  stack.pop_back();
  history.clear();
  duplicate = false;
  z round = 1;
  z current_game = 1;
  z next_game = 1;

  while (true) {
    if constexpr (verbose) {
      if (test) {
        std::cout << "\n-- Round " << round << " (Game " << current_game
                  << ") --\n";
        for (z player = 0; player != 2; ++player) {
          std::cout << "Player " << (player + 1) << "'s deck";
          char sep = ':';
          for (z card : hands[player]) {
            std::cout << sep << ' ' << card;
            sep = ',';
          }
          std::cout << std::endl;
        }
      }
    }
    auto iter = std::ranges::find(history, hands);
    if (iter != std::end(history)) {
      if constexpr (verbose) {
        if (test) {
          std::cout << "Duplicate hand detected!" << std::endl;
        }
      }
      hands[1].clear();
    } else {
      history.push_back(hands);
      if constexpr (verbose) {
        if (test) {
          std::cout << "Player 1 plays: " << hands[0][0] << std::endl;
          std::cout << "Player 2 plays: " << hands[1][0] << std::endl;
        }
      }
      if (std::size(hands[0]) - 1 < hands[0][0] ||
          std::size(hands[1]) - 1 < hands[1][0]) {
        winner = hands[1][0] > hands[0][0];
        hands[winner].push_back(hands[winner][0]);
        hands[winner].push_back(hands[!winner][0]);
        hands[0].erase(std::begin(hands[0]));
        hands[1].erase(std::begin(hands[1]));
        if constexpr (verbose) {
          if (test) {
            std::cout << "Player " << (winner + 1) << " wins round " << round
                      << " of game " << current_game << "!" << std::endl;
          }
        }
      } else {
        // recursive combat
        if constexpr (verbose) {
          if (test) {
            std::cout << "Playing a sub-game to determine the winner..."
                      << std::endl;
          }
        }
        stack.emplace_back(hands, history, round, current_game);
        hands[0].erase(
          std::begin(hands[0]) + hands[0][0] + 1, std::end(hands[0]));
        hands[1].erase(
          std::begin(hands[1]) + hands[1][0] + 1, std::end(hands[1]));
        hands[0].erase(std::begin(hands[0]));
        hands[1].erase(std::begin(hands[1]));
        history.clear();
        round = 0;
        current_game = ++next_game;
        if constexpr (verbose) {
          if (test) {
            std::cout << "\n=== Game " << current_game << " ===" << std::endl;
          }
        }
      }
    }
    if (std::empty(hands[0]) || std::empty(hands[1])) {
      winner = std::empty(hands[0]);
      if constexpr (verbose) {
        if (test) {
          std::cout << "The winner of game " << current_game << " is player "
                    << (winner + 1) << "!" << std::endl;
        }
      }
      if (std::empty(stack)) {
        break;
      }
      // recursive combat finished
      auto &[ohands, ohistory, oround, ogame] = stack.back();
      hands = std::move(ohands);
      history = std::move(ohistory);
      round = oround;
      current_game = ogame;
      stack.pop_back();
      if constexpr (verbose) {
        if (test) {
          std::cout << "\n...anyway, back to game " << current_game << "."
                    << std::endl;
          std::cout << "Player " << (winner + 1) << " wins round " << round
                    << " of game " << current_game << "!" << std::endl;
        }
      }
      hands[winner].push_back(hands[winner][0]);
      hands[winner].push_back(hands[!winner][0]);
      hands[0].erase(std::begin(hands[0]));
      hands[1].erase(std::begin(hands[1]));
    }
    ++round;
  }

  // calculate score
  for (z place = 1; auto face : std::views::reverse(hands[winner])) {
    result[1] += face * place;
    ++place;
  }

  if (test) {
    if (result[1] != expected[1]) {
      std::cout << "Fail, got " << result[1] << ", expected " << expected[1]
                << std::endl;
      std::exit(1);
    }
  } else {
    std::cout << result[1] << std::endl;
  }
}

int main() {
  for (auto filename : {"test/22", "test/22b", "input/22"}) {
    std::cout << "New file " << filename << std::endl;
    if (std::ifstream stream(filename); stream) {
      parts(stream);
    }
  }
  return 0;
}
