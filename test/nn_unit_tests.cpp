#include <bit_factory/ml/nn.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <print>
#include <ranges>
#include <string_view>

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
  auto hidden_ids =
      db.get_hidden_ids(db.get_io_ids({"Bank"}, {"WorldBank", "Earth"}));
  //  auto hidden_ids = db.get_hidden_ids({{1}, {0, 2}});
  CHECK(hidden_ids.size() == 1);
  CHECK(std::ranges::find(hidden_ids, 0) != hidden_ids.end());
  auto prediction_untrained =
      query_t{db,
              db.get_io_ids({"Bank", "World"}, {"WorldBank", "River", "Earth"})}
          .feed_forward();
  std::println("{}", prediction_untrained);
  CHECK(prediction_untrained.size() == 3);
  for (auto v : prediction_untrained) CHECK_THAT(v, WithinAbs(0.076, 0.01));
  auto io_ids =
      db.get_io_ids({"Bank", "World"}, {"WorldBank", "River", "Earth"});
  train(db, io_ids, *db.get_out_id("WorldBank"));
  auto prediction_trained = query_t{db, io_ids}.feed_forward();
  std::println("{}", prediction_trained);
  CHECK_THAT(prediction_trained[0], WithinAbs(0.335, 0.001));
  CHECK_THAT(prediction_trained[1], WithinAbs(0.055, 0.001));
  CHECK_THAT(prediction_trained[2], WithinAbs(0.055, 0.001));
}

using std::operator""sv;
constexpr auto data{
    R"(|   |   |RuB|   |   |   |   |   |   |
|   |RuB|   |   |   |   |   |   |   |
|RuB|   |   |   |   |   |   |   |   |
|   |   |   |   |   |   |   |   |D3 |
|   |   |   |   |   |   |   |D3 |D3 |
|   |   |   |   |   |   |D3 |D3 |   |
|   |   |   |   |   |D3 |D3 |   |D3 |
|   |   |   |   |D3 |D3 |   |D3 |D3 |
|   |   |   |D3 |D3 |   |D3 |D3 |   |
|   |   |D3 |D3 |   |D3 |D3 |   |   |
|   |D3 |D3 |   |D3 |D3 |   |   |RuB|
|D3 |D3 |   |D3 |D3 |   |   |RuB|   |
|D3 |   |D3 |D3 |   |   |RuB|   |   |
|   |D3 |D3 |   |   |RuB|   |   |D3 |
|D3 |D3 |   |   |RuB|   |   |D3 |D3 |
|D3 |   |   |RuB|   |   |D3 |D3 |RuB|
|   |   |RuB|   |   |D3 |D3 |RuB|   |
|   |RuB|   |   |D3 |D3 |RuB|   |   |
|RuB|   |   |D3 |D3 |RuB|   |   |   |
|   |   |D3 |D3 |RuB|   |   |   |D3 |
|   |D3 |D3 |RuB|   |   |   |D3 |   |
|D3 |D3 |RuB|   |   |   |D3 |   |   |
|D3 |RuB|   |   |   |D3 |   |   |D3 |
|RuB|   |   |   |D3 |   |   |D3 |D3 |
|   |   |   |D3 |   |   |D3 |D3 |   |
|   |   |D3 |   |   |D3 |D3 |   |   |
|   |D3 |   |   |D3 |D3 |   |   |   |
|D3 |   |   |D3 |D3 |   |   |   |   |
|   |   |D3 |D3 |   |   |   |   |   |
|   |D3 |D3 |   |   |   |   |   |   |
|D3 |D3 |   |   |   |   |   |   |   |
|D3 |   |   |   |   |   |   |   |RuB|
|   |   |   |   |   |   |   |RuB|   |
|   |   |   |   |   |   |RuB|   |   |
|   |   |   |   |   |RuB|   |   |L6 |
|   |   |   |   |RuB|   |   |L6 |L6 |
|   |   |   |RuB|   |   |L6 |L6 |   |
|   |   |RuB|   |   |L6 |L6 |   |   |
|   |RuB|   |   |L6 |L6 |   |   |   |
|RuB|   |   |L6 |L6 |   |   |   |   |
|   |   |L6 |L6 |   |   |   |   |   |
|   |L6 |L6 |   |   |   |   |   |   |
|L6 |L6 |   |   |   |   |   |   |   |
|L6 |   |   |   |   |   |   |   |RuB|
|   |   |   |   |   |   |   |RuB|RuB|
|   |   |   |   |   |   |RuB|RuB|   |
|   |   |   |   |   |RuB|RuB|   |   |
|   |   |   |   |RuB|RuB|   |   |D3 |
|   |   |   |RuB|RuB|   |   |D3 |   |
|   |   |RuB|RuB|   |   |D3 |   |   |
|   |RuB|RuB|   |   |D3 |   |   |D3 |
|RuB|RuB|   |   |D3 |   |   |D3 |D3 |
|RuB|   |   |D3 |   |   |D3 |D3 |   |
|   |   |D3 |   |   |D3 |D3 |   |   |
|   |D3 |   |   |D3 |D3 |   |   |D3 |
|D3 |   |   |D3 |D3 |   |   |D3 |D3 |
|   |   |D3 |D3 |   |   |D3 |D3 |   |
|   |D3 |D3 |   |   |D3 |D3 |   |   |
|D3 |D3 |   |   |D3 |D3 |   |   |   |
|D3 |   |   |D3 |D3 |   |   |   |D3 |
|   |   |D3 |D3 |   |   |   |D3 |D3 |
|   |D3 |D3 |   |   |   |D3 |D3 |   |
|D3 |D3 |   |   |   |D3 |D3 |   |   |
|D3 |   |   |   |D3 |D3 |   |   |D3 |
|   |   |   |D3 |D3 |   |   |D3 |   |
|   |   |D3 |D3 |   |   |D3 |   |   |
|   |D3 |D3 |   |   |D3 |   |   |D3 |
|D3 |D3 |   |   |D3 |   |   |D3 |D3 |
|D3 |   |   |D3 |   |   |D3 |D3 |D3 |
|   |   |D3 |   |   |D3 |D3 |D3 |   |
|   |D3 |   |   |D3 |D3 |D3 |   |   |
|D3 |   |   |D3 |D3 |D3 |   |   |L6 |
|   |   |D3 |D3 |D3 |   |   |L6 |L6 |
|   |D3 |D3 |D3 |   |   |L6 |L6 |   |
|D3 |D3 |D3 |   |   |L6 |L6 |   |   |
|D3 |D3 |   |   |L6 |L6 |   |   |   |
|D3 |   |   |L6 |L6 |   |   |   |   |
|   |   |L6 |L6 |   |   |   |   |   |
|   |L6 |L6 |   |   |   |   |   |D3 |
|L6 |L6 |   |   |   |   |   |D3 |D3 |
|L6 |   |   |   |   |   |D3 |D3 |   |
|   |   |   |   |   |D3 |D3 |   |   |
|   |   |   |   |D3 |D3 |   |   |   |
|   |   |   |D3 |D3 |   |   |   |D3 |
|   |   |D3 |D3 |   |   |   |D3 |D3 |
|   |D3 |D3 |   |   |   |D3 |D3 |   |
|D3 |D3 |   |   |   |D3 |D3 |   |   |
|D3 |   |   |   |D3 |D3 |   |   |   |
|   |   |   |D3 |D3 |   |   |   |   |
|   |   |D3 |D3 |   |   |   |   |D3 |
|   |D3 |D3 |   |   |   |   |D3 |D3 |
|D3 |D3 |   |   |   |   |D3 |D3 |   |
|D3 |   |   |   |   |D3 |D3 |   |   |
|   |   |   |   |D3 |D3 |   |   |   |
|   |   |   |D3 |D3 |   |   |   |D3 |
|   |   |D3 |D3 |   |   |   |D3 |D3 |
|   |D3 |D3 |   |   |   |D3 |D3 |RuB|
|D3 |D3 |   |   |   |D3 |D3 |RuB|   |
|D3 |   |   |   |D3 |D3 |RuB|   |   |
|   |   |   |D3 |D3 |RuB|   |   |D3 |
|   |   |D3 |D3 |RuB|   |   |D3 |D3 |
|   |D3 |D3 |RuB|   |   |D3 |D3 |   |
|D3 |D3 |RuB|   |   |D3 |D3 |   |D3 |
|D3 |RuB|   |   |D3 |D3 |   |D3 |   |
|RuB|   |   |D3 |D3 |   |D3 |   |L6 |
|   |   |D3 |D3 |   |D3 |   |L6 |   |
|   |D3 |D3 |   |D3 |   |L6 |   |   |
|D3 |D3 |   |D3 |   |L6 |   |   |   |
|D3 |   |D3 |   |L6 |   |   |   |   |
|   |D3 |   |L6 |   |   |   |   |RuB|
|D3 |   |L6 |   |   |   |   |RuB|D3 |
|   |L6 |   |   |   |   |RuB|D3 |   |
|L6 |   |   |   |   |RuB|D3 |   |   |
|   |   |   |   |RuB|D3 |   |   |   |
|   |   |   |RuB|D3 |   |   |   |   |
|   |   |RuB|D3 |   |   |   |   |D3 |
|   |RuB|D3 |   |   |   |   |D3 |D3 |
|RuB|D3 |   |   |   |   |D3 |D3 |   |
|D3 |   |   |   |   |D3 |D3 |   |RuB|
|   |   |   |   |D3 |D3 |   |RuB|   |
|   |   |   |D3 |D3 |   |RuB|   |D3 |
|   |   |D3 |D3 |   |RuB|   |D3 |D3 |
|   |D3 |D3 |   |RuB|   |D3 |D3 |   |
|D3 |D3 |   |RuB|   |D3 |D3 |   |D3 |
|D3 |   |RuB|   |D3 |D3 |   |D3 |   |
|   |RuB|   |D3 |D3 |   |D3 |   |D3 |
|RuB|   |D3 |D3 |   |D3 |   |D3 |   |
|   |D3 |D3 |   |D3 |   |D3 |   |   |
|D3 |D3 |   |D3 |   |D3 |   |   |   |
|D3 |   |D3 |   |D3 |   |   |   |D3 |
|   |D3 |   |D3 |   |   |   |D3 |D3 |
|D3 |   |D3 |   |   |   |D3 |D3 |   |
|   |D3 |   |   |   |D3 |D3 |   |   |
|D3 |   |   |   |D3 |D3 |   |   |D3 |
|   |   |   |D3 |D3 |   |   |D3 |D3 |
|   |   |D3 |D3 |   |   |D3 |D3 |D3 |
|   |D3 |D3 |   |   |D3 |D3 |D3 |   |
|D3 |D3 |   |   |D3 |D3 |D3 |   |   |
|D3 |   |   |D3 |D3 |D3 |   |   |   |
|   |   |D3 |D3 |D3 |   |   |   |RuB|
|   |D3 |D3 |D3 |   |   |   |RuB|D3 |
|D3 |D3 |D3 |   |   |   |RuB|D3 |   |
|D3 |D3 |   |   |   |RuB|D3 |   |   |
|D3 |   |   |   |RuB|D3 |   |   |RuB|
|   |   |   |RuB|D3 |   |   |RuB|   |
|   |   |RuB|D3 |   |   |RuB|   |   |
|   |RuB|D3 |   |   |RuB|   |   |   |
|RuB|D3 |   |   |RuB|   |   |   |   |
|D3 |   |   |RuB|   |   |   |   |D3 |
|   |   |RuB|   |   |   |   |D3 |D3 |
|   |RuB|   |   |   |   |D3 |D3 |   |
|RuB|   |   |   |   |D3 |D3 |   |   |
|   |   |   |   |D3 |D3 |   |   |D3 |
|   |   |   |D3 |D3 |   |   |D3 |D3 |
|   |   |D3 |D3 |   |   |D3 |D3 |   |
|   |D3 |D3 |   |   |D3 |D3 |   |   |
|D3 |D3 |   |   |D3 |D3 |   |   |   |
|D3 |   |   |D3 |D3 |   |   |   |   |
|   |   |D3 |D3 |   |   |   |   |D3 |
|   |D3 |D3 |   |   |   |   |D3 |D3 |
|D3 |D3 |   |   |   |   |D3 |D3 |D3 |
|D3 |   |   |   |   |D3 |D3 |D3 |   |
|   |   |   |   |D3 |D3 |D3 |   |   |
|   |   |   |D3 |D3 |D3 |   |   |D3 |
|   |   |D3 |D3 |D3 |   |   |D3 |D3 |
|   |D3 |D3 |D3 |   |   |D3 |D3 |   |
|D3 |D3 |D3 |   |   |D3 |D3 |   |   |
|D3 |D3 |   |   |D3 |D3 |   |   |   |
|D3 |   |   |D3 |D3 |   |   |   |   |
|   |   |D3 |D3 |   |   |   |   |   |
|   |D3 |D3 |   |   |   |   |   |D3 |
|D3 |D3 |   |   |   |   |   |D3 |D3 |
|D3 |   |   |   |   |   |D3 |D3 |   |
|   |   |   |   |   |D3 |D3 |   |   |
|   |   |   |   |D3 |D3 |   |   |   |
|   |   |   |D3 |D3 |   |   |   |D3 |
|   |   |D3 |D3 |   |   |   |D3 |   |
|   |D3 |D3 |   |   |   |D3 |   |RuB|
|D3 |D3 |   |   |   |D3 |   |RuB|D3 |
|D3 |   |   |   |D3 |   |RuB|D3 |   |
|   |   |   |D3 |   |RuB|D3 |   |   |
|   |   |D3 |   |RuB|D3 |   |   |D3 |
|   |D3 |   |RuB|D3 |   |   |D3 |D3 |
|D3 |   |RuB|D3 |   |   |D3 |D3 |   |
|   |RuB|D3 |   |   |D3 |D3 |   |   |
|RuB|D3 |   |   |D3 |D3 |   |   |   |
|D3 |   |   |D3 |D3 |   |   |   |RuB|
|   |   |D3 |D3 |   |   |   |RuB|D3 |
|   |D3 |D3 |   |   |   |RuB|D3 |D3 |
|D3 |D3 |   |   |   |RuB|D3 |D3 |   |
|D3 |   |   |   |RuB|D3 |D3 |   |   |
|   |   |   |RuB|D3 |D3 |   |   |   |
|   |   |RuB|D3 |D3 |   |   |   |   |
|   |RuB|D3 |D3 |   |   |   |   |   |
|RuB|D3 |D3 |   |   |   |   |   |   |
|D3 |D3 |   |   |   |   |   |   |   |
|D3 |   |   |   |   |   |   |   |   |
|   |   |   |   |   |   |   |   |RuB|
|   |   |   |   |   |   |   |RuB|   |
|   |   |   |   |   |   |RuB|   |D3 |
|   |   |   |   |   |RuB|   |D3 |D3 |
|   |   |   |   |RuB|   |D3 |D3 |   |
|   |   |   |RuB|   |D3 |D3 |   |   |
|   |   |RuB|   |D3 |D3 |   |   |D3 |
|   |RuB|   |D3 |D3 |   |   |D3 |D3 |
|RuB|   |D3 |D3 |   |   |D3 |D3 |   |
|   |D3 |D3 |   |   |D3 |D3 |   |   |
|D3 |D3 |   |   |D3 |D3 |   |   |   |
|D3 |   |   |D3 |D3 |   |   |   |D3 |
|   |   |D3 |D3 |   |   |   |D3 |D3 |
|   |D3 |D3 |   |   |   |D3 |D3 |   |
|D3 |D3 |   |   |   |D3 |D3 |   |   |
|D3 |   |   |   |D3 |D3 |   |   |RuB|
|   |   |   |D3 |D3 |   |   |RuB|D3 |
|   |   |D3 |D3 |   |   |RuB|D3 |D3 |
|   |D3 |D3 |   |   |RuB|D3 |D3 |   |
|D3 |D3 |   |   |RuB|D3 |D3 |   |   |
|D3 |   |   |RuB|D3 |D3 |   |   |   |
|   |   |RuB|D3 |D3 |   |   |   |   |
|   |RuB|D3 |D3 |   |   |   |   |   |
|RuB|D3 |D3 |   |   |   |   |   |   |
|D3 |D3 |   |   |   |   |   |   |   |
|D3 |   |   |   |   |   |   |   |   |
|   |   |   |   |   |   |   |   |D3 |
|   |   |   |   |   |   |   |D3 |D3 |
|   |   |   |   |   |   |D3 |D3 |   |
|   |   |   |   |   |D3 |D3 |   |   |
|   |   |   |   |D3 |D3 |   |   |   |
|   |   |   |D3 |D3 |   |   |   |D3 |
|   |   |D3 |D3 |   |   |   |D3 |D3 |
|   |D3 |D3 |   |   |   |D3 |D3 |   |
|D3 |D3 |   |   |   |D3 |D3 |   |RuB|
|D3 |   |   |   |D3 |D3 |   |RuB|   |
|   |   |   |D3 |D3 |   |RuB|   |D3 |
|   |   |D3 |D3 |   |RuB|   |D3 |D3 |
|   |D3 |D3 |   |RuB|   |D3 |D3 |   |
|D3 |D3 |   |RuB|   |D3 |D3 |   |   |
|D3 |   |RuB|   |D3 |D3 |   |   |D3 |
|   |RuB|   |D3 |D3 |   |   |D3 |D3 |
|RuB|   |D3 |D3 |   |   |D3 |D3 |   |
|   |D3 |D3 |   |   |D3 |D3 |   |   |
|D3 |D3 |   |   |D3 |D3 |   |   |RuB|
|D3 |   |   |D3 |D3 |   |   |RuB|D3 |
|   |   |D3 |D3 |   |   |RuB|D3 |D3 |
|   |D3 |D3 |   |   |RuB|D3 |D3 |   |
|D3 |D3 |   |   |RuB|D3 |D3 |   |   |
|D3 |   |   |RuB|D3 |D3 |   |   |D3 |
|   |   |RuB|D3 |D3 |   |   |D3 |D3 |
|   |RuB|D3 |D3 |   |   |D3 |D3 |D3 |
|RuB|D3 |D3 |   |   |D3 |D3 |D3 |   |
|D3 |D3 |   |   |D3 |D3 |D3 |   |   |
|D3 |   |   |D3 |D3 |D3 |   |   |   |
|   |   |D3 |D3 |D3 |   |   |   |   |
|   |D3 |D3 |D3 |   |   |   |   |   |
|D3 |D3 |D3 |   |   |   |   |   |D3 |
|D3 |D3 |   |   |   |   |   |D3 |D3 |
|D3 |   |   |   |   |   |D3 |D3 |   |
|   |   |   |   |   |D3 |D3 |   |   |
|   |   |   |   |D3 |D3 |   |   |D3 |
|   |   |   |D3 |D3 |   |   |D3 |   |
|   |   |D3 |D3 |   |   |D3 |   |   |
|   |D3 |D3 |   |   |D3 |   |   |   |
|D3 |D3 |   |   |D3 |   |   |   |   |
|D3 |   |   |D3 |   |   |   |   |RuB|
|   |   |D3 |   |   |   |   |RuB|RuB|
|   |D3 |   |   |   |   |RuB|RuB|   |
|D3 |   |   |   |   |RuB|RuB|   |D3 |
|   |   |   |   |RuB|RuB|   |D3 |D3 |
|   |   |   |RuB|RuB|   |D3 |D3 |   |
|   |   |RuB|RuB|   |D3 |D3 |   |   |
|   |RuB|RuB|   |D3 |D3 |   |   |   |
|RuB|RuB|   |D3 |D3 |   |   |   |   |
|RuB|   |D3 |D3 |   |   |   |   |   |
|   |D3 |D3 |   |   |   |   |   |   |
|D3 |D3 |   |   |   |   |   |   |   |
|D3 |   |   |   |   |   |   |   |   |
|   |   |   |   |   |   |   |   |D3 |
|   |   |   |   |   |   |   |D3 |   |
|   |   |   |   |   |   |D3 |   |L6 |
|   |   |   |   |   |D3 |   |L6 |   |
|   |   |   |   |D3 |   |L6 |   |   |
|   |   |   |D3 |   |L6 |   |   |D3 |
|   |   |D3 |   |L6 |   |   |D3 |D3 |
|   |D3 |   |L6 |   |   |D3 |D3 |   |
|D3 |   |L6 |   |   |D3 |D3 |   |   |
|   |L6 |   |   |D3 |D3 |   |   |   |
|L6 |   |   |D3 |D3 |   |   |   |D3 |
|   |   |D3 |D3 |   |   |   |D3 |D3 |
|   |D3 |D3 |   |   |   |D3 |D3 |   |
|D3 |D3 |   |   |   |D3 |D3 |   |   |
|D3 |   |   |   |D3 |D3 |   |   |D3 |
|   |   |   |D3 |D3 |   |   |D3 |D3 |
|   |   |D3 |D3 |   |   |D3 |D3 |   |
|   |D3 |D3 |   |   |D3 |D3 |   |   |
|D3 |D3 |   |   |D3 |D3 |   |   |   |
|D3 |   |   |D3 |D3 |   |   |   |D3 |
|   |   |D3 |D3 |   |   |   |D3 |D3 |
|   |D3 |D3 |   |   |   |D3 |D3 |   |
|D3 |D3 |   |   |   |D3 |D3 |   |   |
|D3 |   |   |   |D3 |D3 |   |   |   |
|   |   |   |D3 |D3 |   |   |   |   |
|   |   |D3 |D3 |   |   |   |   |   |
|   |D3 |D3 |   |   |   |   |   |   |
|D3 |D3 |   |   |   |   |   |   |   |
|D3 |   |   |   |   |   |   |   |   |
|   |   |   |   |   |   |   |   |D3 |
|   |   |   |   |   |   |   |D3 |D3 |
|   |   |   |   |   |   |D3 |D3 |D3 |
|   |   |   |   |   |D3 |D3 |D3 |   |
|   |   |   |   |D3 |D3 |D3 |   |   |
|   |   |   |D3 |D3 |D3 |   |   |D3 |
|   |   |D3 |D3 |D3 |   |   |D3 |D3 |
|   |D3 |D3 |D3 |   |   |D3 |D3 |   |
|D3 |D3 |D3 |   |   |D3 |D3 |   |   |
|D3 |D3 |   |   |D3 |D3 |   |   |D3 |
|D3 |   |   |D3 |D3 |   |   |D3 |   |
|   |   |D3 |D3 |   |   |D3 |   |D3 |
|   |D3 |D3 |   |   |D3 |   |D3 |   |
|D3 |D3 |   |   |D3 |   |D3 |   |   |
|D3 |   |   |D3 |   |D3 |   |   |D3 |)"sv};

using namespace bit_factory::ml;

std::pair<nn::in_signals_t, nn::out_signals_t> parse_signals(auto line) {
  nn::in_signals_t in_signals;
  nn::out_signals_t out_signals;
  for (auto [i, word] :
       std::views::split(line, "|"sv) | std::views::enumerate) {
    if (i == 0) continue;
    auto token = std::format("{}|{}", i, std::string_view(word));
    //      std::println("token{}", token);
    if (i == 10) continue;
    if (i < 9)
      in_signals.push_back(token);
    else
      out_signals.push_back(token);
  }
  return {in_signals, out_signals};
}

void predict(nn::data_base_t db, auto observation) {
  auto [observation_signals, _] = parse_signals(observation);
  nn::out_signals_t out_signals{std::from_range, db.get_out_signals()};
  auto io_ids = db.get_io_ids(observation_signals, out_signals);
  auto prediction_for_observation = nn::query_t{db, io_ids}.feed_forward();
  std::println("prediction {} -> {}", observation_signals,
               std::views::zip(prediction_for_observation, out_signals));
}

TEST_CASE("nn2") {
  constexpr auto nl{"\n"sv};
  nn::data_base_t db;

  for (auto line : std::views::split(data, nl)) {
    auto [in_signals, out_signals] = parse_signals(line);
    db.add_in(in_signals);
    db.add_out(out_signals);
    auto train_ids = db.get_io_ids(
        in_signals, nn::out_signals_t{std::from_range, db.get_out_signals()});
    nn::train(db, train_ids, *db.get_out_id(out_signals.front()));

    auto prediction_trained = nn::query_t{db, train_ids}.feed_forward();
    std::println("prediction: {}", prediction_trained);
  }
  for (auto i : db.get_in_signals()) std::println("in: {}", i);
  for (auto o : db.get_out_signals()) std::println("out: {}", o);

  predict(db, R"(|   |D3 |   |D3 |   |   |D3 |D3 |)"sv);
  predict(db, R"(|   |D3 |   |D3 |   |D3 |D3 |D3 |)"sv);
  predict(db, R"(|   |D3 |   |D3 |D3 |D3 |D3 |D3 |)"sv);
  predict(db, R"(|   |D3 |D3 |D3 |D3 |D3 |D3 |D3 |)"sv);
  predict(db, R"(|D3 |D3 |D3 |D3 |D3 |D3 |D3 |D3 |)"sv);
}
