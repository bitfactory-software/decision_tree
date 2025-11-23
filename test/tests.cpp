#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <bit_factory/ml/decision_tree.hpp>
#include <iostream>
// #include <print>
#include <algorithm>
#include <concepts>
#include <string>
#include <tuple>

using namespace Catch::Matchers;

namespace {
// referrer, loaction, read FAQ, pages viewed, service choosen
using decision_tree =
    bit_factory::ml::decision_tree<std::string, std::string, bool, int, std::string>;
const decision_tree::rows_t test_data{{"Slashdot", "France", true, 19, "None"},
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

TEST_CASE("split_table_by_column_value") {
  auto split = decision_tree::split_table_by_column_value<2>(test_data, true);

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
  for (auto const& row : expected_0)
    CHECK(std::ranges::find(split[0], row) != split[0].end());

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
  for (auto const& row : expected_1)
    CHECK(std::ranges::find(split[1], row) != split[1].end());
}

TEST_CASE("result_counts") {
  auto counts = decision_tree::result_counts(test_data);
  CHECK(counts.size() == 3);
  CHECK(counts["None"] == 7);
  CHECK(counts["basic"] == 6);
  CHECK(counts["Premium"] == 3);

  auto impurity = decision_tree::gini_impurity(test_data);
  CHECK_THAT(impurity, WithinAbs(0.6328125, 0.00000001));
  auto e = decision_tree::entropy(test_data);
  CHECK_THAT(e, WithinAbs(1.50524081494414785, 0.00000001));
}

TEST_CASE("build_tree and classify") {
  auto tree = decision_tree::build_tree(test_data);
  std::cout << decision_tree::print_node(tree, "") << "\n";

  static_assert(std::same_as<decision_tree::observation_t,
                             std::tuple<std::string, std::string, bool, int>>);
  auto prediction = decision_tree::classify(tree, {"direct", "USA", true, 5});
  std::cout << decision_tree::print_result(prediction) << "\n";
  CHECK(*prediction.begin() == decision_tree::result_t{"basic", 4});
}
