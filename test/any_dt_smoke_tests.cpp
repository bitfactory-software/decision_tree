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
using namespace bit_factory;
namespace {
namespace any_dt_smoke_test {
struct observation {
  std::optional<std::string> referrer;
  std::optional<std::string> location;
  std::optional<bool> read_faq;
  std::optional<int> pages_viewed;
};
struct sample {
  std::string referrer;
  std::string location;
  bool read_faq;
  int pages_viewed;
  std::string service_choosen;
  friend auto operator<=>(const sample&, const sample&) = default;
};

using samples = std::vector<sample>;

}  // namespace any_dt_smoke_test
}  // namespace

using namespace bit_factory::ml;
using namespace any_dt_smoke_test;

ANY_MODEL_MAP((any_dt_smoke_test::observation), bit_factory::ml::any::row) {
  static std::optional<bit_factory::ml::any::value<>> subscript(
      observation const& self, std::size_t i) {
    switch (i) {
      case 0:
        if (self.referrer) return *self.referrer;
        break;
      case 1:
        if (self.location) return *self.location;
        break;
      case 2:
        if (self.read_faq) return *self.read_faq;
        break;
      case 3:
        if (self.pages_viewed) return *self.pages_viewed;
        break;
      default:
        throw std::out_of_range("Invalid column index");
    }
    return std::nullopt;
  };
};

ANY_MODEL_MAP((any_dt_smoke_test::sample), bit_factory::ml::any::row) {
  static bit_factory::ml::any::value<> subscript(sample const& self,
                                                 std::size_t i) {
    switch (i) {
      case 0:
        return self.referrer;
      case 1:
        return self.location;
      case 2:
        return self.read_faq;
      case 3:
        return self.pages_viewed;
      case 4:
        return self.service_choosen;
      default:
        throw std::out_of_range("Invalid column index");
    }
  };
};

ANY_MODEL_MAP((any_dt_smoke_test::samples), bit_factory::ml::any::sheet) {
  static std::generator<row<>> rows(any_dt_smoke_test::samples const& self) {
    for (auto const& sample : self) co_yield sample;
  };
  static std::string column_header(
      [[maybe_unused]] any_dt_smoke_test::samples const& self,
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
      [[maybe_unused]] any_dt_smoke_test::samples const& self) {
    return 5;
  };
};

namespace {
namespace any_dt_smoke_test {

// referrer, loaction, read FAQ, pages viewed, service choosen
const samples test_data = {{"Slashdot", "France", true, 19, "None"},
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
any::sheet<> sheet = test_data;

TEST_CASE("any, split_table_by_column_value") {
  auto split = any::decision_tree::split_table_by_column_value(2, sheet, true);

  CHECK(split[0].rows.size() == 8);
  // std::println("split[0] {}", split[0]);
  const samples expected_0 = {{"Slashdot", "France", true, 19, "None"},
                              {"Slashdot", "USA", true, 18, "None"},
                              {"Kiwitobes", "France", true, 23, "basic"},
                              {"Kiwitobes", "France", true, 19, "basic"},
                              {"Google", "France", true, 23, "Premium"},
                              {"Google", "UK", true, 18, "basic"},
                              {"Digg", "New Zealand", true, 12, "basic"},
                              {"Digg", "USA", true, 24, "basic"}};
  for (auto const& expected : expected_0)
    CHECK(std::ranges::find_if(split[0].rows, [&](any::row<> const& splitted) {
            return splitted == any::row<>{expected};
          }) != split[0].rows.end());

  CHECK(split[1].rows.size() == 8);
  // std::println("split[1] {}", split[1]);
  const samples expected_1 = {{"Slashdot", "UK", false, 21, "None"},
                              {"Kiwitobes", "UK", false, 19, "None"},
                              {"Google", "UK", false, 21, "Premium"},
                              {"Google", "UK", false, 18, "None"},
                              {"Google", "USA", false, 24, "Premium"},
                              {"Digg", "USA", false, 18, "None"},
                              {"(direct)", "New Zealand", false, 12, "None"},
                              {"(direct)", "UK", false, 21, "basic"}};
  for (auto const& expected : expected_1)
    CHECK(std::ranges::find_if(split[1].rows, [&](any::row<> const& splitted) {
            return splitted == expected;
          }) != split[1].rows.end());
}

TEST_CASE("any, result_counts") {
  using namespace std::string_literals;
  auto counts = any::decision_tree{sheet}.result_counts(sheet);
  CHECK(counts.size() == 3);
  CHECK(counts["None"s] == 7);
  CHECK(counts["basic"s] == 6);
  CHECK(counts["Premium"s] == 3);

  auto impurity = any::decision_tree::gini_impurity(counts);
  CHECK_THAT(impurity, WithinAbs(0.6328125, 0.00000001));
  auto e = any::decision_tree::entropy(counts);
  CHECK_THAT(e, WithinAbs(1.50524081494414785, 0.00000001));
}

TEST_CASE("any, build_tree, classify, prune, classify_with_missing_data") {
  using namespace std::string_literals;

  auto tree_structure = any::decision_tree{test_data};
  auto tree = tree_structure.build_tree();

  CHECK(tree_structure.to_string(tree) ==
        R"(referrer == Google?
T-> pages viewed >= 21?
   T-> {Premium: 3}
   F-> read FAQ == false?
      T-> {None: 1}
      F-> {basic: 1}
F-> referrer == Slashdot?
   T-> {None: 3}
   F-> read FAQ == false?
      T-> pages viewed >= 21?
         T-> {basic: 1}
         F-> {None: 3}
      F-> {basic: 4}
)");

  //   static_assert(
  //       std::same_as<
  //           decision_tree::observation_t,
  //           std::tuple<std::optional<std::string>,
  //           std::optional<std::string>,
  //                      std::optional<bool>, std::optional<int>>>);
  auto prediction = tree_structure.classify(tree, {"(direct)", "USA", true, 5});

  CHECK(*prediction.begin() == any::result_t{"basic"s, 4});
  CHECK(to_string(prediction) == "{basic: 4}");

  auto tree_prune_0_1 = tree_structure.prune(tree_structure.build_tree(), 0.1);
  CHECK(tree_structure.to_string(tree_prune_0_1) ==
        R"(referrer == Google?
T-> pages viewed >= 21?
   T-> {Premium: 3}
   F-> read FAQ == false?
      T-> {None: 1}
      F-> {basic: 1}
F-> referrer == Slashdot?
   T-> {None: 3}
   F-> read FAQ == false?
      T-> pages viewed >= 21?
         T-> {basic: 1}
         F-> {None: 3}
      F-> {basic: 4}
)");

  auto tree_prune_1_0 = tree_structure.prune(tree_prune_0_1, 1.0);
  CHECK(tree_structure.to_string(tree_prune_1_0) ==
        R"(referrer == Google?
T-> pages viewed >= 21?
   T-> {Premium: 3}
   F-> read FAQ == false?
      T-> {None: 1}
      F-> {basic: 1}
F-> {None: 6, basic: 5}
)");

  {
    auto prediction_with_missing1 = tree_structure.classify_with_missing_data(
        tree, {"Google"s, {}, true, {}});
    CHECK(to_string(prediction_with_missing1) ==
          "{Premium: 2.25, basic: 0.25}");
    auto prediction_with_missing2 = tree_structure.classify_with_missing_data(
        tree, {"Google"s, "France"s, {}, {}});
    CHECK(to_string(prediction_with_missing2) ==
          "{None: 0.125, Premium: 2.25, basic: 0.125}");
    auto prediction_with_missing3 =
        tree_structure.classify_with_missing_data(tree, {"Google"s, {}, {}, {}});
    CHECK(to_string(prediction_with_missing3) ==
          "{None: 0.125, Premium: 2.25, basic: 0.125}");
  }

  {
    auto prediction_with_missing1 = tree_structure.classify_with_missing_data(
        tree_prune_1_0, {"Google"s, {}, true, {}});
    CHECK(to_string(prediction_with_missing1) ==
          "{Premium: 2.25, basic: 0.25}");
    auto prediction_with_missing2 = tree_structure.classify_with_missing_data(
        tree_prune_1_0, {"Google"s, "France"s, {}, {}});
    CHECK(to_string(prediction_with_missing2) ==
          "{None: 0.125, Premium: 2.25, basic: 0.125}");
    auto prediction_with_missing3 = tree_structure.classify_with_missing_data(
        tree_prune_1_0, {"Google"s, {}, {}, {}});
    CHECK(to_string(prediction_with_missing3) ==
          "{None: 0.125, Premium: 2.25, basic: 0.125}");
  }
}
}  // namespace any_dt_smoke_test
}  // namespace