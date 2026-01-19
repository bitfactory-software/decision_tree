#include <version>
#if defined __cpp_lib_generator

#include <bit_factory/ml/any_decision_tree.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace bit_factory::ml;

namespace dt_prune_test {

using sample = std::tuple<bool, int, std::string>;
using samples = std::vector<sample>;
using probe = any_decision_tree::observation_tuple_t<sample>;

struct sample_sheet {
  samples data; // NOLINT (cppcoreguidelines-pro-type-member-init,hicpp-member-init)
  bool was_night_shift_is_significant = false;
};

const samples test_data{
    {true, 1, "N"}, {false, 2, "N"}, {false, 2, "N"}, {true, 1, ""},
    {false, 1, ""}, {false, 1, ""},  {false, 1, ""},  {false, 1, ""},
    {false, 1, ""}, {false, 1, ""},  {false, 1, ""},  {false, 1, ""},
    {false, 1, ""}, {false, 1, ""},  {false, 1, ""},  {false, 1, ""},
    {false, 1, ""}, {false, 1, ""},  {false, 1, ""},  {false, 1, ""},
    {false, 1, ""}, {false, 1, ""},  {false, 1, ""},  {false, 1, ""},
    {false, 1, ""}, {false, 1, ""},  {false, 1, ""},  {false, 1, ""},
    {false, 1, ""}, {false, 1, ""},  {false, 1, ""},  {false, 1, ""},
    {false, 1, ""}, {false, 1, ""},  {false, 1, ""},  {false, 1, ""},
    {false, 1, ""}, {false, 1, ""},  {false, 1, ""},  {false, 1, ""},
    {false, 1, ""}, {false, 1, ""},  {false, 1, ""},  {false, 1, ""},
    {false, 1, ""}, {false, 1, ""},  {false, 1, ""},  {false, 1, ""},
    {false, 1, ""}, {false, 1, ""},  {false, 1, ""},  {false, 1, ""},
    {false, 1, ""}, {false, 1, ""},  {false, 1, ""},  {false, 1, ""},
    {false, 1, ""}, {false, 1, ""},  {false, 1, ""},  {false, 1, ""},
    {false, 1, ""}, {false, 1, ""},  {false, 1, ""},  {false, 1, ""},
    {false, 1, ""}, {false, 1, ""},  {false, 1, ""},  {false, 1, ""},
    {false, 1, ""}};

}  // namespace dt_prune_test

ANY_MODEL_MAP((dt_prune_test::sample_sheet),
              bit_factory::ml::any_decision_tree::sheet) {
  static anyxx::any_forward_range<row<>, row<>> rows(
      dt_prune_test::sample_sheet const& self) {  // NOLINT
    return self.data;
  };
  static std::string column_header(
      [[maybe_unused]] dt_prune_test::sample_sheet const& self,
      std::size_t index) {
    switch (index) {
      case 0:
        return "WasNighShift";
      case 1:
        return "DaysOff";
      case 2:
        return "Prediction";
      default:
        throw std::out_of_range("Invalid column index");
    }
  };
  static bool column_is_significant(
      [[maybe_unused]] dt_prune_test::sample_sheet const& self,
      std::size_t index) {
    switch (index) {
      case 0:
        return self.was_night_shift_is_significant;
      case 1:
        return true;
      case 2:
        return false;
      default:
        throw std::out_of_range("Invalid column index");
    }
  };
  static std::size_t column_count(
      [[maybe_unused]] dt_prune_test::sample_sheet const& self) {
    return std::tuple_size_v<dt_prune_test::sample>;
  };
};

TEST_CASE("dt prune 1") {
  dt_prune_test::sample_sheet sheet{.data = dt_prune_test::test_data,
                                    .was_night_shift_is_significant = false};
  auto tree = any_decision_tree::build_tree(sheet);
  CHECK(to_string(tree) ==
        R"(DaysOff >= 2?
T-> {N: 2}
F-> WasNighShift?
   T-> {: 65}
   F-> {: 1, N: 1}
)");
  auto pruned = any_decision_tree::prune(tree, 0.0);
  CHECK(to_string(pruned) ==
        R"(DaysOff >= 2?
T-> {N: 2}
F-> {: 66, N: 1}
)");
}
TEST_CASE("dt prune 2") {
    dt_prune_test::sample_sheet sheet{ .data = dt_prune_test::test_data,
                                      .was_night_shift_is_significant = true };
    auto tree = any_decision_tree::build_tree(sheet);
  CHECK(to_string(tree) ==
        R"(DaysOff >= 2?
T-> {N: 2}
F-> WasNighShift?
   T-> {: 65}
   F-> {: 1, N: 1}
)");
  auto pruned = any_decision_tree::prune(tree, 0.0);
  CHECK(to_string(pruned) ==
        R"(DaysOff >= 2?
T-> {N: 2}
F-> WasNighShift?
   T-> {: 65}
   F-> {: 1, N: 1}
)");
}
#endif