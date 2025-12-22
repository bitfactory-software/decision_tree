#include <bit_factory/ml/decision_tree.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

TEST_CASE("build_tree1") {
  using namespace bit_factory;
  using decision_tree =
      ml::decision_tree<ml::array_sheet<std::string, 3>>;
  const decision_tree::rows_t samples{
      {{"", "D", ""}, "N"},
      {{"N", "", ""}, "N"},
      {{"D", "", "D"}, "D"},
      {{"", "", ""}, ""}
  };

  auto tree = decision_tree::build_tree(samples);
  CHECK(to_string(tree) ==
        R"(x[0] == D?
T-> {D: 1}
F-> x[0] == ?
   T-> x[1] == ?
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

TEST_CASE("build_tree2") {
  using namespace bit_factory;
  std::string ___ = "", X = "X", Y = "Y";
  using decision_tree = ml::decision_tree<ml::array_sheet<std::string, 8>>;
  const decision_tree::rows_t samples{
      {{___, ___, ___, ___, ___, ___, X, ___}, Y},
      {{___, ___, ___, ___, ___, Y, ___, ___}, Y},
      {{___, ___, ___, ___, ___, ___, X, ___}, X},
      {{___, ___, ___, ___, ___, Y, ___, ___}, ___},
      {{___, ___, ___, ___, ___, ___, X, ___}, ___}};

  auto tree = decision_tree::build_tree(samples);
  CHECK(to_string(tree) ==
        R"(x[5] == ?
T-> {: 1, X: 1, Y: 1}
F-> {: 1, Y: 1}
)");

  CHECK(tree);
  auto test = [&](decision_tree::observation_t const& observation) {
    return decision_tree::to_string(decision_tree::classify(tree, observation));
  };
  CHECK(test({___, ___, ___, ___, ___, ___, Y, ___}) == "{: 1, X: 1, Y: 1}");
}

TEST_CASE("print empty node") {
    using namespace bit_factory;
    using decision_tree = ml::decision_tree<ml::array_sheet<std::string, 8>>;
    const decision_tree::rows_t samples{};
    auto tree = decision_tree::build_tree(samples);
    CHECK(to_string(tree) == R"(x[0] == ?
)");
    CHECK(!tree);
}

TEST_CASE("classify empty node") {
    using namespace bit_factory;
    using decision_tree = ml::decision_tree<ml::array_sheet<std::string, 8>>;
    const decision_tree::tree_t tree{};
    const decision_tree::observation_t observation = { "", "", "", "", "", "", "Y", ""};
    const auto result = decision_tree::to_string(decision_tree::classify(tree, observation));
    CHECK(result == "{}");
    CHECK(!tree);
}