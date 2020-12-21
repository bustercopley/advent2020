#include "precompiled.h"
#include "split.h"
#include "symbols.h"

auto regex1 =
  re::regex(R"(^(?:(\d+),([a-z,]+)|([a-z ]+)(?:\(contains ([a-z, ]+)\))?)$)");
auto regex2 = re::regex(R"(^([a-z]+)(?:[, ]+|$)(.*)$)");

using z = std::size_t;

auto &vivify(auto &vector, z index) {
  if (std::size(vector) < index + 1) {
    vector.resize(index + 1);
  }
  return vector[index];
}

void parts(std::istream &stream) {
  bool test = false;
  z result1 = 0;
  z expected1 = 0;
  std::string result2;
  std::string expected2;

  symbols in_names;
  symbols aller_names;
  std::map<z, std::set<z>> contained_by;
  std::vector<int> occurs;
  std::vector<std::set<z>> might_contain;

  for (std::string line; std::getline(stream, line);) {
    if (auto m = match(regex1, line)) {
      if (matched(m, 1)) {
        test = true;
        expected1 = string_to<int>(match_view(m, 1, line));
        expected2 = match_string(m, 2);
      } else {
        auto in_view = match_view(m, 3, line);
        auto aller_view = match_view(m, 4, line);
        std::set<z> ins;
        for (auto [in_name] : split(in_view, regex2)) {
          z in = in_names[std::string(in_name)];
          ins.insert(in);
          ++vivify(occurs, in);
        }
        for (auto [aller_name] : split(aller_view, regex2)) {
          z aller = aller_names[std::string(aller_name)];
          auto [j, emplaced] = contained_by.try_emplace(aller, ins);
          auto &ingredients_with_allergen = j->second;
          if (!emplaced) {
            std::set<z> intersect;
            for (auto i : ingredients_with_allergen) {
              if (ins.contains(i)) {
                intersect.insert(i);
              }
            }
            ingredients_with_allergen = std::move(intersect);
          }
        }
      }
    }
  }
  bool done;
  do {
    done = true;
    for (auto &[aller, ins] : contained_by) {
      if (std::size(ins) == 1) {
        z in = *std::begin(ins);
        // 'aller' is contained by exactly one ingredient 'in',
        // so no other allergen is contained by 'in'
        for (auto &[other_aller, other_ins] : contained_by) {
          if (other_aller != aller) {
            auto iter = other_ins.find(in);
            if (iter != std::end(other_ins)) {
              done = false;
              other_ins.erase(iter);
            }
          }
        }
      }
    }
  } while (!done);
  for (const auto &[aller, ins] : contained_by) {
    for (z in : ins) {
      occurs[in] = 0;
    }
  }
  result1 = std::accumulate(std::begin(occurs), std::end(occurs), 0);

  if (test) {
    if (result1 != expected1) {
      std::cout << "Fail, got " << result1 << ", expected " << expected1
                << std::endl;
      std::exit(1);
    }
  } else {
    std::cout << result1 << std::endl;
  }

  std::vector<std::pair<z, z>> contains;
  for (const auto &[aller, ins] : contained_by) {
    if (!std::empty(ins)) {
      assert(std::size(ins) == 1);
      contains.emplace_back(aller, *std::begin(ins));
    }
  }
  std::sort(std::begin(contains), std::end(contains),
    [&aller_names](const auto &a, const auto &b) {
      return aller_names[a.first] < aller_names[b.first];
    });
  for (const auto &[aller, in] : contains) {
    result2 += in_names[in];
    result2 += ',';
  }
  result2.erase(std::size(result2) - 1);

  if (test) {
    if (result2 != expected2) {
      std::cout << "Fail, got " << result2 << ", expected " << expected2
                << std::endl;
      std::exit(1);
    }
  } else {
    std::cout << result2 << std::endl;
  }
}

int main() {
  for (auto filename : {"test/21", "input/21"}) {
    if (std::ifstream stream(filename); stream) {
      parts(stream);
    }
  }
  return 0;
}
