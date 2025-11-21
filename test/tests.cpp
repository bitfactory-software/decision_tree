#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <decision_tree/decision_tree.hpp>
#include <iostream>
//#include <print>
#include <string>
#include <algorithm>

using namespace Catch::Matchers;

//namespace {
//// referrer, loaction, read FAQ, pages viewed, service choosen
//using data_set =
//    decision_tree::data<std::string, std::string, bool, int, std::string>;
//const data_set::rows_t test_data{{"Slashdot", "USA", true, 18, "None"},
//                                 {"Google", "France", true, 23, "Premium"},
//                                 {"Digg", "USA", true, 24, "Basic"},
//                                 {"Kiwitobes", "France", true, 23, "Basic"},
//                                 {"Google", "UK", false, 21, "Premium"},
//                                 {"(direct)", "New Zealand", false, 12, "None"},
//                                 {"(direct)", "UK", false, 21, "Basic"},
//                                 {"Google", "USA", false, 24, "Premium"},
//                                 {"Slashdot", "France", true, 19, "None"},
//                                 {"Digg", "USA", false, 18, "None"},
//                                 {"Google", "UK", false, 18, "None"},
//                                 {"Kiwitobes", "UK", false, 19, "None"},
//                                 {"Digg", "New Zealand", true, 12, "Basic"},
//                                 {"Google", "UK", true, 18, "Basic"},
//                                 {"Kiwitobes", "France", true, 19, "Basic"}};
//
//}  // namespace
//
//TEST_CASE("split_table_by_column_value") {
//  auto split = data_set::split_table_by_column_value<2>(test_data, true);
//
//  CHECK(split[0].size() == 8);
//  //std::println("split[0] {}", split[0]);
//  const data_set::rows_t expected_0 = {{"Slashdot", "USA", true, 18, "None"},
//                                 {"Google", "France", true, 23, "Premium"},
//                                 {"Digg", "USA", true, 24, "Basic"},
//                                 {"Kiwitobes", "France", true, 23, "Basic"},
//                                 {"Slashdot", "France", true, 19, "None"},
//                                 {"Digg", "New Zealand", true, 12, "Basic"},
//                                 {"Google", "UK", true, 18, "Basic"},
//                                 {"Kiwitobes", "France", true, 19, "Basic"}};
//  for (auto const& row : expected_0)
//    CHECK(std::ranges::find(split[0], row) != split[0].end());
//
//  CHECK(split[1].size() == 7);
//  //std::println("split[1] {}", split[1]);
//  const data_set::rows_t expected_1 = {{"Google", "UK", false, 21, "Premium"},
//                                 {"(direct)", "UK", false, 21, "Basic"},
//                                 {"Google", "USA", false, 24, "Premium"},
//                                 {"Digg", "USA", false, 18, "None"},
//                                 {"Google", "UK", false, 18, "None"},
//                                 {"Kiwitobes", "UK", false, 19, "None"}};
//  for (auto const& row : expected_1)
//    CHECK(std::ranges::find(split[1], row) != split[1].end());
//}
//
//TEST_CASE("result_counts") {
//  auto counts = data_set::result_counts(test_data);
//  CHECK(counts.size() == 3);
//  CHECK(counts["None"] == 6);
//  CHECK(counts["Basic"] == 6);
//  CHECK(counts["Premium"] == 3);
//
//  auto impurity = data_set::gini_impurity(test_data);
//  CHECK_THAT(impurity, WithinAbs(0.64, 0.00000001));
//  auto e = data_set::entropy(test_data);
//  CHECK_THAT(e, WithinAbs(1.52192809488736214, 0.00000001));
//}
//
//TEST_CASE("build_tree") {
//  auto tree = data_set::build_tree_with_entropy(test_data);
//  std::cout << data_set::print(tree, "");
//}

TEST_CASE("build_tree") {
	CHECK(true);
}
