#include <version>
#if defined __cpp_lib_generator

#include <bit_factory/ml/any_decision_tree.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
// #include <print>
#include <algorithm>
#include <concepts>
#include <optional>
#include <string>
#include <tuple>

using namespace Catch::Matchers;
using namespace bit_factory::ml;
namespace {
namespace tuple_dt_smoke_test {

using sample = std::tuple<std::string, std::string, bool, int, std::string>;
using samples = std::vector<sample>;
using probe = any_decision_tree::observation_tuple_t<sample>;

const samples test_data{{"Slashdot", "France", true, 19, "None"},
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

}  // namespace tuple_dt_smoke_test
}  // namespace

ANY_MODEL_MAP((tuple_dt_smoke_test::samples),
              bit_factory::ml::any_decision_tree::sheet) {
  static std::generator<row<>> rows(
      tuple_dt_smoke_test::samples const& self) {  // NOLINT
    for (auto const& sample : self) co_yield sample;
  };
  static std::string column_header(
      [[maybe_unused]] tuple_dt_smoke_test::samples const& self,
      std::size_t index) {
    switch (index) {
      case 0:
        return "referrer";
      case 1:
        return "location";
      case 2:
        return "read FAQ";
      case 3:
        return "pages viewed";
      case 4:
        return "service choosen";
      default:
        throw std::out_of_range("Invalid column index");
    }
  };
  static std::size_t column_count(
      [[maybe_unused]] tuple_dt_smoke_test::samples const& self) {
    return std::tuple_size_v<tuple_dt_smoke_test::sample>;
  };
};

namespace {
namespace tuple_dt_smoke_test {

using namespace bit_factory::ml;

const any_decision_tree::sheet<> sheet = test_data;

TEST_CASE("split_table_by_column_value") {
  auto split = any_decision_tree::split_table_by_column_value(2, sheet, true);

  CHECK(split[0].rows.size() == 8);
  const samples expected_0 = {{"Slashdot", "UK", false, 21, "None"},
                              {"Kiwitobes", "UK", false, 19, "None"},
                              {"Google", "UK", false, 21, "Premium"},
                              {"Google", "UK", false, 18, "None"},
                              {"Google", "USA", false, 24, "Premium"},
                              {"Digg", "USA", false, 18, "None"},
                              {"(direct)", "New Zealand", false, 12, "None"},
                              {"(direct)", "UK", false, 21, "basic"}};
  for (auto const& expected : expected_0)
    CHECK(std::ranges::find_if(split[0].rows,
                               [&](any_decision_tree::row<> const& splitted) {
                                 return splitted == expected;
                               }) != split[0].rows.end());

  CHECK(split[1].rows.size() == 8);
  // std::println("split[1] {}", split[1]);
  const samples expected_1 = {{"Slashdot", "France", true, 19, "None"},
                              {"Slashdot", "USA", true, 18, "None"},
                              {"Kiwitobes", "France", true, 23, "basic"},
                              {"Kiwitobes", "France", true, 19, "basic"},
                              {"Google", "France", true, 23, "Premium"},
                              {"Google", "UK", true, 18, "basic"},
                              {"Digg", "New Zealand", true, 12, "basic"},
                              {"Digg", "USA", true, 24, "basic"}};
  for (auto const& expected : expected_1)
    CHECK(std::ranges::find_if(split[1].rows,
                               [&](any_decision_tree::row<> const& splitted) {
                                 return splitted == expected;
                               }) != split[1].rows.end());
}

TEST_CASE("result_counts") {
  using namespace std::string_literals;

  auto counts = any_decision_tree::result_counts(sheet, sheet);
  CHECK(counts.size() == 3);
  CHECK(counts["None"s] == 7);
  CHECK(counts["basic"s] == 6);
  CHECK(counts["Premium"s] == 3);

  auto impurity = any_decision_tree::gini_impurity(counts);
  CHECK_THAT(impurity, WithinAbs(0.6328125, 0.00000001));
  auto e = any_decision_tree::entropy(counts);
  CHECK_THAT(e, WithinAbs(1.50524081494414785, 0.00000001));
}

TEST_CASE("build_tree, classify, prune, classify_with_missing_data") {
  using namespace std::string_literals;

  auto test_data_sheet = any_decision_tree::sheet{test_data};
  auto tree = any_decision_tree::build_tree(test_data_sheet);
  CHECK(to_string(tree) ==
        R"(referrer == Google?
T-> pages viewed >= 21?
   T-> {Premium: 3}
   F-> read FAQ?
      T-> {basic: 1}
      F-> {None: 1}
F-> referrer == Slashdot?
   T-> {None: 3}
   F-> read FAQ?
      T-> {basic: 4}
      F-> pages viewed >= 21?
         T-> {basic: 1}
         F-> {None: 3}
)");

  // static_assert(
  //     std::same_as<
  //         decision_tree::observation_t,
  //         std::tuple<std::optional<std::string>, std::optional<std::string>,
  //                    std::optional<bool>, std::optional<int>>>);
  auto prediction = classify(tree, probe{"(direct)"s, "USA"s, true, 5});
  CHECK(*prediction.begin() == any_decision_tree::result_t{"basic"s, 4});
  CHECK(any_decision_tree::to_string(prediction) == "{basic: 4}");

  auto tree_prune_0_1 = any_decision_tree::prune(
      any_decision_tree::build_tree(test_data_sheet), 0.1);
  CHECK(to_string(tree_prune_0_1) ==
        R"(referrer == Google?
T-> pages viewed >= 21?
   T-> {Premium: 3}
   F-> read FAQ?
      T-> {basic: 1}
      F-> {None: 1}
F-> referrer == Slashdot?
   T-> {None: 3}
   F-> read FAQ?
      T-> {basic: 4}
      F-> pages viewed >= 21?
         T-> {basic: 1}
         F-> {None: 3}
)");
  auto tree_prune_1_0 = prune(tree_prune_0_1, 1.0);
  CHECK(to_string(tree_prune_1_0) ==
        R"(referrer == Google?
T-> pages viewed >= 21?
   T-> {Premium: 3}
   F-> read FAQ?
      T-> {basic: 1}
      F-> {None: 1}
F-> {None: 6, basic: 5}
)");

  {
    auto p1 = probe{"Google"s, {}, true, {}};
    auto o1 = any_decision_tree::observation{p1};
    auto v1 = o1[0];
    CHECK(v1);
    CHECK(v1->to_string() == "Google");  // NOLINT
    auto prediction_with_missing1 =
        classify_with_missing_data(tree, probe{"Google"s, {}, true, {}});
    CHECK(to_string(prediction_with_missing1) ==
          "{Premium: 2.25, basic: 0.25}");
    auto prediction_with_missing2 =
        classify_with_missing_data(tree, probe{"Google", "France", {}, {}});
    CHECK(to_string(prediction_with_missing2) ==
          "{None: 0.125, Premium: 2.25, basic: 0.125}");
    auto prediction_with_missing3 =
        classify_with_missing_data(tree, probe{"Google", {}, {}, {}});
    CHECK(to_string(prediction_with_missing3) ==
          "{None: 0.125, Premium: 2.25, basic: 0.125}");
  }

  {
    auto prediction_with_missing1 = classify_with_missing_data(
        tree_prune_1_0, probe{"Google", {}, true, {}});
    CHECK(to_string(prediction_with_missing1) ==
          "{Premium: 2.25, basic: 0.25}");
    auto prediction_with_missing2 = classify_with_missing_data(
        tree_prune_1_0, probe{"Google", "France", {}, {}});
    CHECK(to_string(prediction_with_missing2) ==
          "{None: 0.125, Premium: 2.25, basic: 0.125}");
    auto prediction_with_missing3 =
        classify_with_missing_data(tree_prune_1_0, probe{"Google", {}, {}, {}});
    CHECK(to_string(prediction_with_missing3) ==
          "{None: 0.125, Premium: 2.25, basic: 0.125}");
  }
}

}  // namespace tuple_dt_smoke_test
}  // namespace

#endif