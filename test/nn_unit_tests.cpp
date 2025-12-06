#include <bit_factory/ml/nn.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <print>
#include <ranges>
#include <string_view>
#include <functional>

using namespace Catch::Matchers;
using namespace bit_factory::ml;
using std::operator""sv;

TEST_CASE("nn1") {
  nn::data_base_t db;
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
      nn::query_t{
          db, db.get_io_ids({"Bank", "World"}, {"WorldBank", "River", "Earth"})}
          .feed_forward();
  std::println("{}", prediction_untrained);
  CHECK(prediction_untrained.size() == 3);
  for (auto v : prediction_untrained) CHECK_THAT(v, WithinAbs(0.076, 0.01));
  auto io_ids =
      db.get_io_ids({"Bank", "World"}, {"WorldBank", "River", "Earth"});
  train(db, io_ids, *db.get_out_id("WorldBank"));
  auto prediction_trained = nn::query_t{db, io_ids}.feed_forward();
  std::println("{}", prediction_trained);
  CHECK_THAT(prediction_trained[0], WithinAbs(0.335, 0.001));
  CHECK_THAT(prediction_trained[1], WithinAbs(0.055, 0.001));
  CHECK_THAT(prediction_trained[2], WithinAbs(0.055, 0.001));
}

namespace {
std::pair<nn::in_signals_t, nn::out_signals_t> parse_signals(auto line,
                                                             int length = 9) {
  nn::in_signals_t in_signals;
  nn::out_signals_t out_signals;
  for (auto [i, word] :
       std::views::split(line, "|"sv) | std::views::enumerate) {
    if (i == 0) continue;
    auto token = std::format("{}|{}", i, std::string_view(word));
    //      std::println("token{}", token);
    if (i > length) continue;
    if (i < length)
      in_signals.push_back(token);
    else if (!std::string_view(word).empty())
      out_signals.push_back(token);
  }
  return {in_signals, out_signals};
}

void trace_io_nodes(std::string_view label, auto const& nodes) {
  for (auto node : nodes)
    std::println("{}: {}[{}]", label, node.first, node.second);
}

void trace_edges(std::string_view label, auto const& edges) {
  std::print("+++{}:\n", label);
  for (auto const& edge : edges)
    std::print("\t{} -> {} ({})\n", edge.first.from, edge.first.to,
               edge.second);
  std::print("---\n");
}

void trace_nn(nn::data_base_t const& db, auto label) {
  std::println("+++ {}", label);
  trace_io_nodes("in", db.input_nodes());
  trace_io_nodes("out", db.output_nodes());
  trace_edges("input_edges", db.input_edges());
  std::println("hidden_nodes: {}", db.hidden_nodes());
  trace_edges("output_edges", db.output_edges());
  std::println("--- {}", label);
}

void predict(nn::data_base_t& db, auto observation, int length = 9) {
  auto [observation_signals, _] = parse_signals(observation, length);
  db.add_in(observation_signals);
  // trace_nn(db, std::string_view(observation));
  nn::out_signals_t out_signals{std::from_range, db.get_out_signals()};
  auto io_ids = db.get_io_ids(observation_signals, out_signals);
  auto pure_predictions = nn::query_t{db, io_ids}.feed_forward();
  auto predictions = nn::sigmod(pure_predictions);
  std::println("prediction for {} sum = {}", std::string_view{observation},
               std::ranges::fold_left(pure_predictions, 0.0, std::plus<>{}));
  for (auto [i, prediction] : std::views::enumerate(predictions))
    std::println("\t{}, id =[{}]: {}, {}", *db.get_out_token(io_ids.out[i]),
                 io_ids.out[i], prediction, pure_predictions[i]);
}

void train_db(nn::data_base_t& db, auto const& data, bool log = false,
              int length = 9) {
  constexpr auto nl{"\n"sv};
  for (auto line : std::views::split(data, nl)) {
    auto [in_signals, out_signals] = parse_signals(line, length);
    db.add_in(in_signals);
    db.add_out(out_signals);
    auto train_ids = db.get_io_ids(
        in_signals, nn::out_signals_t{std::from_range, db.get_out_signals()});
    nn::train(db, train_ids, *db.get_out_id(out_signals.front()));
    if (log) trace_nn(db, std::string_view(line));

    if (!log) continue;
    auto prediction_trained = nn::query_t{db, train_ids}.feed_forward();
    std::println("prediction: {}",
                 std::views::zip(prediction_trained, train_ids.out));
  }
}

}  // namespace

#include "nn2_test_data.hpp"
TEST_CASE("nn2") {
  nn::data_base_t db;
  train_db(db, data_nn2);
  predict(db, R"(|   |D3 |   |D3 |   |   |   |D3 |)"sv);
  predict(db, R"(|   |D3 |   |D3 |   |   |D3 |D3 |)"sv);
  predict(db, R"(|   |D3 |   |D3 |   |D3 |D3 |D3 |)"sv);
  predict(db, R"(|   |D3 |   |D3 |D3 |D3 |D3 |D3 |)"sv);
  predict(db, R"(|   |D3 |D3 |D3 |D3 |D3 |D3 |D3 |)"sv);
  predict(db, R"(|D3 |D3 |D3 |D3 |D3 |D3 |D3 |D3 |)"sv);
}

#include "nn3_test_data.hpp"
TEST_CASE("nn3") {
  nn::data_base_t db;
  train_db(db, data_nn3);
  predict(db, R"(|   |   |D3 |   |   |LD6|   |   |)"sv);
  predict(db, R"(|   |   |D3 |   |   |LD6|LD6|   |)"sv);
  predict(db, R"(|   |   |D3 |   |   |LD6|LD6|LD6|)"sv);
  predict(db, R"(|   |   |D3 |   |   |LD6|LD6|N1 |)"sv);
  predict(db, R"(|   |   |D3 |   |   |   |N1 |N1 |)"sv);
  predict(db, R"(|N1 |   |   |   |D3 |L6D|   |   |)"sv);
  predict(db, R"(|   |N1 |N1 |   |   |D3 |   |   |)"sv);
}

TEST_CASE("parse_signals1") {
  auto [in, out] = parse_signals(R"(|N1 |   |LD6|)"sv, 3);
  CHECK(in.size() == 2);
  CHECK(in[0] == "1|N1 ");
  CHECK(in[1] == "2|   ");
  CHECK(out.size() == 1);
  CHECK(out[0] == "3|LD6");
}
TEST_CASE("parse_signals2") {
  auto [in, out] = parse_signals(R"(|N1 |   |)"sv, 3);
  CHECK(in.size() == 2);
  CHECK(in[0] == "1|N1 ");
  CHECK(in[1] == "2|   ");
  CHECK(out.size() == 0);
}

TEST_CASE("nnX1") {
  nn::data_base_t db;
  constexpr auto data{R"(|N1 |   |LD6|)"sv};
  train_db(db, data, true, 3);
  predict(db, R"(|N1 |   |)"sv, 3);  // -> "LD6"
}
TEST_CASE("nnX2") {
  nn::data_base_t db;
  constexpr auto data{R"(|   |   |RuB|)"sv};
  train_db(db, data, true, 3);
  predict(db, R"(|   |   |)"sv, 3);  // -> "LD6"
}
TEST_CASE("nnX3") {
  nn::data_base_t db;
  constexpr auto data{
      R"(|N1 |   |LD6|
|   |   |RuB|)"sv};
  train_db(db, data, true, 3);
  predict(db, R"(|N1 |   |)"sv, 3);  // -> "LD6"
  predict(db, R"(|   |   |)"sv, 3);  // -> "RuB"
}
TEST_CASE("nnX4") {
  nn::data_base_t db;
  constexpr auto data{
      R"(|N1 |   |LD6|
|   |   |RuB|)"sv};
  train_db(db, data, true, 3);
  predict(db, R"(|   |   |)"sv, 3);  // -> "RuB"
  predict(db, R"(|N1 |   |)"sv, 3);  // -> "LD6"
}
TEST_CASE("nn3b") {
  nn::data_base_t db;
  constexpr auto data_nn3a{
      R"(|N1 |   |LD6|
|   |   |RuB|)"sv};
  train_db(db, data_nn3a, true, 3);
  predict(db, R"(|N1 |   |)"sv, 3);  // -> "LD6"
  predict(db, R"(|N1 |RuB|)"sv, 3);  // -> "?"
  predict(db, R"(|N1 |N1 |)"sv, 3);  // -> "?"
  predict(db, R"(|   |   |)"sv, 3);  // -> "RuB"
  predict(db, R"(|   |RuB|)"sv, 3);  // -> "?"
  predict(db, R"(|   |N1 |)"sv, 3);  // -> "?"
  predict(db, R"(|RuB|   |)"sv, 3);  // -> "?"
  predict(db, R"(|RuB|RuB|)"sv, 3);  // -> "?"
  predict(db, R"(|RuB|N1 |)"sv, 3);  // -> "?"
}
