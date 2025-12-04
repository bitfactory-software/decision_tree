#include <bit_factory/ml/nn.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <print>

using namespace Catch::Matchers;

TEST_CASE("nn1") {
  using namespace bit_factory::ml::nn;
  data_base_t db;
  db.add_in({"World", "Bank"});
  db.add_out({"WorldBank", "River", "Earth"});
  CHECK(!db.get_in_id("WorldBank"));
  CHECK(*db.get_in_id("World") == 0);
  CHECK(*db.get_in_id("Bank") == 1);
  CHECK(!db.get_out_id("World"));
  CHECK(*db.get_out_id("WorldBank") == 0);
  CHECK(*db.get_out_id("River") == 1);
  CHECK(*db.get_out_id("Earth") == 2);
  db.generate_hidden_node({0, 1}, {0, 1, 2});
  for (auto edge : db.input_edges())
    std::println("{}, {}, {}", edge.first.from, edge.first.to, edge.second);
  for (auto edge : db.output_edges())
    std::println("{}, {}, {}", edge.first.from, edge.first.to, edge.second);
  auto hidden_ids = db.get_hidden_ids({1}, {0, 2});
  CHECK(hidden_ids.size() == 1);
  CHECK(std::ranges::find(hidden_ids, 0) != hidden_ids.end());
  auto prediction = query_t{db, {0, 1}, {0, 1, 2}}.feed_forward();
  std::println("{}", prediction);
  CHECK(prediction.size() == 3);
  for (auto v : prediction) CHECK_THAT(v, WithinAbs(0.076, 0.01));
}
