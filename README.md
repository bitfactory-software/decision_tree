[![CI](https://github.com/bitfactory-software/decision_tree/actions/workflows/CI.yml/badge.svg)](https://github.com/bitfactory-software/decision_tree/actions/workflows/CI.yml)

# decision_tree

From **Programming Collective Intelligence** by **Toby Segaran**, O'Reilly, **Chapter 7, Modeling with Decision Trees**.

Usage sample:

```cpp 
#include <bit_factory/ml/decision_tree.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <iostream>
#include <algorithm>
#include <concepts>
#include <optional>
#include <string>
#include <tuple>

using namespace Catch::Matchers;

namespace {
// referrer, loaction, read FAQ, pages viewed, service choosen
using decision_tree = bit_factory::ml::decision_tree<std::string, std::string,
                                                     bool, int, std::string>;
const decision_tree::rows_t test_data{
    {"Slashdot", "France", true, 19, "None"},
    {"Slashdot", "UK", false, 21, "None"},
    {"Slashdot", "USA", true, 18, "None"},
    {"Kiwitobes", "France", true, 19, "basic"},
    {"Kiwitobes", "France", true, 23, "basic"},
    {"Kiwitobes", "UK", false, 19, "None"},
    {"Google", "France", true, 23, "Premium"},
    {"Google", "UK", false, 21, "Premium"},
    {"Google", "USA", false, 24, "Premium"},
    {"Google", "UK", false, 18, "None"},
    {"Google", "UK", true, 18, "basic"},
    {"Digg", "New Zealand", true, 12, "basic"},
    {"Digg", "USA", false, 18, "None"},
    {"Digg", "USA", true, 24, "basic"},
    {"(direct)", "New Zealand", false, 12, "None"},
    {"(direct)", "UK", false, 21, "basic"}};

}  // namespace

static const auto pointer_to_test_data_rows =
    decision_tree::get_pointer_to_rows(test_data);

TEST_CASE("split_table_by_column_value") {
  auto split = decision_tree::split_table_by_column_value<2>(
      pointer_to_test_data_rows, true);

  CHECK(split[0].size() == 8);
  // std::println("split[0] {}", split[0]);
  const decision_tree::rows_t expected_0 = {
      {"Slashdot", "France", true, 19, "None"},
      {"Slashdot", "USA", true, 18, "None"},
      {"Kiwitobes", "France", true, 23, "basic"},
      {"Kiwitobes", "France", true, 19, "basic"},
      {"Google", "France", true, 23, "Premium"},
      {"Google", "UK", true, 18, "basic"},
      {"Digg", "New Zealand", true, 12, "basic"},
      {"Digg", "USA", true, 24, "basic"}};
  for (auto const& expected : expected_0)
    CHECK(std::ranges::find_if(split[0],
                               [&](decision_tree::row_t const* splitted) {
                                 return *splitted == expected;
                               }) != split[0].end());

  CHECK(split[1].size() == 8);
  // std::println("split[1] {}", split[1]);
  const decision_tree::rows_t expected_1 = {
      {"Slashdot", "UK", false, 21, "None"},
      {"Kiwitobes", "UK", false, 19, "None"},
      {"Google", "UK", false, 21, "Premium"},
      {"Google", "UK", false, 18, "None"},
      {"Google", "USA", false, 24, "Premium"},
      {"Digg", "USA", false, 18, "None"},
      {"(direct)", "New Zealand", false, 12, "None"},
      {"(direct)", "UK", false, 21, "basic"}};
  for (auto const& expected : expected_1)
    CHECK(std::ranges::find_if(split[1],
                               [&](decision_tree::row_t const* splitted) {
                                 return *splitted == expected;
                               }) != split[1].end());
}

TEST_CASE("result_counts") {
  auto counts = decision_tree::result_counts(pointer_to_test_data_rows);
  CHECK(counts.size() == 3);
  CHECK(counts["None"] == 7);
  CHECK(counts["basic"] == 6);
  CHECK(counts["Premium"] == 3);

  auto impurity = decision_tree::gini_impurity(counts);
  CHECK_THAT(impurity, WithinAbs(0.6328125, 0.00000001));
  auto e = decision_tree::entropy(counts);
  CHECK_THAT(e, WithinAbs(1.50524081494414785, 0.00000001));
}

TEST_CASE("build_tree, classify, prune, classify_with_missing_data") {
  using namespace std::string_literals;

  auto tree = decision_tree::build_tree(test_data);
  CHECK(to_string(tree) ==
        R"(0:Google?
T-> 3:21?
   T-> {Premium: 3}
   F-> 2:1?
      T-> {basic: 1}
      F-> {None: 1}
F-> 0:Slashdot?
   T-> {None: 3}
   F-> 2:1?
      T-> {basic: 4}
      F-> 3:21?
         T-> {basic: 1}
         F-> {None: 3}
)");

  static_assert(
      std::same_as<
          decision_tree::observation_t,
          std::tuple<std::optional<std::string>, std::optional<std::string>,
                     std::optional<bool>, std::optional<int>>>);
  auto prediction = decision_tree::classify(tree, {"(direct)", "USA", true, 5});
  CHECK(*prediction.begin() == decision_tree::result_t{"basic", 4});
  CHECK(decision_tree::to_string(prediction) == "{basic: 4}");

  auto tree_prune_0_1 = decision_tree::prune(decision_tree::build_tree(test_data), 0.1);
  CHECK(to_string(tree_prune_0_1) ==
        R"(0:Google?
T-> 3:21?
   T-> {Premium: 3}
   F-> 2:1?
      T-> {basic: 1}
      F-> {None: 1}
F-> 0:Slashdot?
   T-> {None: 3}
   F-> 2:1?
      T-> {basic: 4}
      F-> 3:21?
         T-> {basic: 1}
         F-> {None: 3}
)");
  auto tree_prune_1_0 = decision_tree::prune(tree_prune_0_1, 1.0);
  CHECK(to_string(tree_prune_1_0) ==
        R"(0:Google?
T-> 3:21?
   T-> {Premium: 3}
   F-> 2:1?
      T-> {basic: 1}
      F-> {None: 1}
F-> {None: 6, basic: 5}
)");

  {
    auto prediction_with_missing1 = decision_tree::classify_with_missing_data(
        tree, {"Google", {}, true, {}});
    CHECK(decision_tree::to_string(prediction_with_missing1) ==
          "{Premium: 2.25, basic: 0.25}");
    auto prediction_with_missing2 = decision_tree::classify_with_missing_data(
        tree, {"Google", "France", {}, {}});
    CHECK(decision_tree::to_string(prediction_with_missing2) ==
          "{None: 0.125, Premium: 2.25, basic: 0.125}");
    auto prediction_with_missing3 =
        decision_tree::classify_with_missing_data(tree, {"Google", {}, {}, {}});
    CHECK(decision_tree::to_string(prediction_with_missing3) ==
          "{None: 0.125, Premium: 2.25, basic: 0.125}");
  }

  {
    auto prediction_with_missing1 = decision_tree::classify_with_missing_data(
        tree_prune_1_0, {"Google", {}, true, {}});
    CHECK(decision_tree::to_string(prediction_with_missing1) ==
          "{Premium: 2.25, basic: 0.25}");
    auto prediction_with_missing2 = decision_tree::classify_with_missing_data(
        tree_prune_1_0, {"Google", "France", {}, {}});
    CHECK(decision_tree::to_string(prediction_with_missing2) ==
          "{None: 0.125, Premium: 2.25, basic: 0.125}");
    auto prediction_with_missing3 = decision_tree::classify_with_missing_data(
        tree_prune_1_0, {"Google", {}, {}, {}});
    CHECK(decision_tree::to_string(prediction_with_missing3) ==
          "{None: 0.125, Premium: 2.25, basic: 0.125}");
  }
}
```




## More Details

 * [Dependency Setup](README_dependencies.md)
 * [Building Details](README_building.md)
 * [Troubleshooting](README_troubleshooting.md)
