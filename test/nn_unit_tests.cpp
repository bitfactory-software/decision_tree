#include <bit_factory/ml/nn.hpp>
#include <catch2/catch_test_macros.hpp>
#include <print>

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
}
