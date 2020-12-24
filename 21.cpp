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

  symbols ingredient_names;
  symbols allergen_names;
  // contained_by[allergen]: set of ingredients one of which contains 'allergen'
  // occurs[ingredient]: total number of occurrences of 'ingredient'
  std::map<z, std::set<z>> contained_by;
  std::vector<int> occurs;

  for (std::string line; std::getline(stream, line);) {
    if (auto m = match(regex1, line)) {
      if (matched(m, 1)) {
        test = true;
        expected1 = string_to<int>(match_view(m, 1, line));
        expected2 = match_string(m, 2);
      } else {
        // Process one line of input
        auto ingredient_view = match_view(m, 3, line); // ingredient list text
        auto allergen_view = match_view(m, 4, line);   // allergen list text
        std::set<z> ingredients; // set of ingredient symbols on this line
        for (auto [ingredient_name] : split(ingredient_view, regex2)) {
          z ingredient = ingredient_names[std::string(ingredient_name)];
          ingredients.insert(ingredient);
          ++vivify(occurs, ingredient);
        }
        for (auto [allergen_name] : split(allergen_view, regex2)) {
          z allergen = allergen_names[std::string(allergen_name)];
          // If contained_by[allergen] does not already exist,
          // assign to it the set of ingredients on this line
          auto [j, emplaced] = contained_by.try_emplace(allergen, ingredients);
          if (!emplaced) {
            // Otherwise, assign to contained_by[allergen] the intersection
            // of its existing value with the set of ingredients on this line
            auto &ingredients_containing_allergen = j->second;
            std::set<z> intersect;
            for (auto i : ingredients_containing_allergen) {
              if (ingredients.contains(i)) {
                intersect.insert(i);
              }
            }
            ingredients_with_allergen = std::move(intersect);
          }
        }
      }
    }
  }
  // Search for an allergen contained by exactly one ingredient and remove the
  // allergen from all other ingredients; repeat until no allergen found
  bool done;
  do {
    done = true;
    for (auto &[allergen, ingredients] : contained_by) {
      if (std::size(ingredients) == 1) {
        z ingredient = *std::begin(ingredients);
        // 'allergen' is contained by exactly one ingredient 'ingredient',
        // so no other allergen is contained by 'ingredient'
        for (auto &[other_allergen, other_ingredients] : contained_by) {
          if (other_allergen != allergen) {
            auto iter = other_ingredients.find(ingredient);
            if (iter != std::end(other_ingredients)) {
              done = false;
              other_ingredients.erase(iter);
            }
          }
        }
      }
    }
  } while (!done);

  // Day 1 result: add together the occurrence counts of all ingredients that
  // do not contain an allergen
  for (const auto &[allergen, ingredients] : contained_by) {
    for (z ingredient : ingredients) {
      occurs[ingredient] = 0;
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

  // Day 2

  // Map each allergen to the ingredient that contains it
  std::vector<std::pair<z, z>> contains;
  for (const auto &[allergen, ingredients] : contained_by) {
    if (!std::empty(ingredients)) {
      assert(std::size(ingredients) == 1);
      contains.emplace_back(allergen, *std::begin(ingredients));
    }
  }
  // Sort alphabetically by allergen name
  std::sort(std::begin(contains), std::end(contains),
    [&allergen_names](const auto &a, const auto &b) {
      return allergen_names[a.first] < allergen_names[b.first];
    });
  // Day 2 result: list of ingredient names
  for (const auto &[allergen, ingredient] : contains) {
    result2 += ingredient_names[ingredient];
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
