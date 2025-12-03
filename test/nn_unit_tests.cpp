#include <bit_factory/ml/nn.hpp>
#include <catch2/catch_test_macros.hpp>
#include <print>
#include <string>

TEST_CASE("nn1") {
  using namespace bit_factory::ml::nn;
  data_t data;
  generate_hidden_node(data, {101, 102}, {201, 202, 203});
  for( auto edge: data.input_edges_)
	  std::println("{}, {}, {}", edge.first.from, edge.first.to, edge.second);
  for( auto edge: data.output_edges_)
	  std::println("{}, {}, {}", edge.first.from, edge.first.to, edge.second);
}
