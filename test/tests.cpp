#include <bit_factory/ml/decision_tree.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <iostream>
// #include <print>
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
  std::cout << "build_tree:\n" << decision_tree::print_node(tree, "") << "\n\n";

  static_assert(
      std::same_as<
          decision_tree::observation_t,
          std::tuple<std::optional<std::string>, std::optional<std::string>,
                     std::optional<bool>, std::optional<int>>>);
  auto prediction = decision_tree::classify(tree, {"(direct)", "USA", true, 5});
  std::cout << R"-(classify(tree, {"(direct)", "USA", true, 5}): )-"
            << decision_tree::print_result(prediction) << "\n\n";
  CHECK(*prediction.begin() == decision_tree::result_t{"basic", 4});

  auto tree_prune_0_1 = decision_tree::build_tree(test_data);
  decision_tree::prune(tree_prune_0_1, 0.1);
  std::cout << "tree_prune_0_1\n"
            << decision_tree::print_node(tree_prune_0_1, "") << "\n\n";
  auto tree_prune_1_0 = decision_tree::build_tree(test_data);
  decision_tree::prune(tree_prune_1_0, 1.0);
  std::cout << "tree_prune_1_0\n"
            << decision_tree::print_node(tree_prune_1_0, "") << "\n\n";

  {
      auto prediction_with_missing1 =
          decision_tree::classify_with_missing_data(tree, {"Google", {}, true, {}});
      std::cout
          << R"-(classify_with_missing_data(tree, {"Google", {}, true, {}}): )-"
          << decision_tree::print_result(prediction_with_missing1) << "\n\n";
      auto prediction_with_missing2 = decision_tree::classify_with_missing_data(
          tree, {"Google", "France", {}, {}});
      std::cout
          << R"-(classify_with_missing_data(tree, {"Google", "France", {}, {}}): )-"
          << decision_tree::print_result(prediction_with_missing2) << "\n\n";
      auto prediction_with_missing3 =
          decision_tree::classify_with_missing_data(tree, {"Google", {}, {}, {}});
      std::cout << R"-(classify_with_missing_data(tree, {"Google", {}, {}, {}}): )-"
                << decision_tree::print_result(prediction_with_missing3) << "\n\n";
  }

  {
      auto prediction_with_missing1 =
          decision_tree::classify_with_missing_data(tree_prune_1_0, {"Google", {}, true, {}});
      std::cout
          << R"-(classify_with_missing_data(tree_prune_1_0, {"Google", {}, true, {}}): )-"
          << decision_tree::print_result(prediction_with_missing1) << "\n\n";
      auto prediction_with_missing2 = decision_tree::classify_with_missing_data(
          tree_prune_1_0, {"Google", "France", {}, {}});
      std::cout
          << R"-(classify_with_missing_data(tree_prune_1_0, {"Google", "France", {}, {}}): )-"
          << decision_tree::print_result(prediction_with_missing2) << "\n\n";
      auto prediction_with_missing3 =
          decision_tree::classify_with_missing_data(tree_prune_1_0, {"Google", {}, {}, {}});
      std::cout << R"-(classify_with_missing_data(tree_prune_1_0, {"Google", {}, {}, {}}): )-"
                << decision_tree::print_result(prediction_with_missing3) << "\n\n";
  }
}
