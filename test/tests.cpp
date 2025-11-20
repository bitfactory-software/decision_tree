#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <decision_tree/decision_tree.hpp>
#include <print>
#include <string>

using namespace decision_tree;
using namespace Catch::Matchers;

namespace {
// referrer, loaction, read FAQ, pages viewed, service choosen
using input_t =
    decision_tree::input<std::string, std::string, bool, int, std::string>;
input_t rows{{"Slashdot", "USA", true, 18, "None"},
             {"Google", "France", true, 23, "Premium"},
             {"Digg", "USA", true, 24, "Basic"},
             {"Kiwitobes", "France", true, 23, "Basic"},
             {"Google", "UK", false, 21, "Premium"},
             {"(direct)", "New Zealand", false, 12, "None"},
             {"(direct)", "UK", false, 21, "Basic"},
             {"Google", "USA", false, 24, "Premium"},
             {"Slashdot", "France", true, 19, "None"},
             {"Digg", "USA", false, 18, "None"},
             {"Google", "UK", false, 18, "None"},
             {"Kiwitobes", "UK", false, 19, "None"},
             {"Digg", "New Zealand", true, 12, "Basic"},
             {"Google", "UK", true, 18, "Basic"},
             {"Kiwitobes", "France", true, 19, "Basic"}};

}  // namespace

TEST_CASE("divide_table_column") {
  auto split = rows.divide_table_column<2>(true);

  CHECK(split[0].size() == 8);
  std::println("split[0] {}", split[0]);
  input_t::rows_t expected_0 = {{"Slashdot", "USA", true, 18, "None"},
                                {"Google", "France", true, 23, "Premium"},
                                {"Digg", "USA", true, 24, "Basic"},
                                {"Kiwitobes", "France", true, 23, "Basic"},
                                {"Slashdot", "France", true, 19, "None"},
                                {"Digg", "New Zealand", true, 12, "Basic"},
                                {"Google", "UK", true, 18, "Basic"},
                                {"Kiwitobes", "France", true, 19, "Basic"}};
  for (auto const& row : expected_0)
    CHECK(split[0].find(row) != split[0].end());

  CHECK(split[1].size() == 7);
  std::println("split[1] {}", split[1]);
  input_t::rows_t expected_1 = {{"Google", "UK", false, 21, "Premium"},
                                {"(direct)", "UK", false, 21, "Basic"},
                                {"Google", "USA", false, 24, "Premium"},
                                {"Digg", "USA", false, 18, "None"},
                                {"Google", "UK", false, 18, "None"},
                                {"Kiwitobes", "UK", false, 19, "None"}};
  for (auto const& row : expected_1)
    CHECK(split[1].find(row) != split[1].end());
}

TEST_CASE("result_counts") {
  auto counts = rows.result_counts();
  CHECK(counts.size() == 3);
  CHECK(counts["None"] == 6);
  CHECK(counts["Basic"] == 6);
  CHECK(counts["Premium"] == 3);

  auto impurity = rows.gini_impurity();
  CHECK_THAT(impurity, WithinAbs(0.64, 0.00000001));
  auto e = rows.entropy();
  CHECK_THAT(e, WithinAbs(1.52192809488736214, 0.00000001));
}
