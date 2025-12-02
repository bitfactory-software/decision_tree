#include <bit_factory/ml/decision_tree.hpp>
#include <catch2/catch_test_macros.hpp>
// #include <print>
#include <string>

namespace {
// referrer, loaction, read FAQ, pages viewed, service choosen

}  // namespace

TEST_CASE("build_tree1") {
  using namespace bit_factory;
  using decision_tree =
      ml::decision_tree<std::string, std::string, std::string, std::string>;
  const decision_tree::rows_t samples{
      {"", "D", "", "N"},
      {"N", "", "", "N"},
      {"D", "", "D", "D"},
      {"", "", "", ""},
  };

  auto tree = decision_tree::build_tree(samples);
  CHECK(to_string(tree) ==
        R"(0:D?
T-> {D: 1}
F-> 0:?
   T-> 1:?
      T-> {: 1}
      F-> {N: 1}
   F-> {N: 1}
)");

  auto test = [&](decision_tree::observation_t const& observation) {
    return decision_tree::to_string(decision_tree::classify(tree, observation));
  };
  CHECK(test({"", "D", ""}) == "{N: 1}");
  CHECK(test({"N", "", ""}) == "{N: 1}");
  CHECK(test({"D", "", "D"}) == "{D: 1}");
  CHECK(test({"", "", ""}) == "{: 1}");
}
