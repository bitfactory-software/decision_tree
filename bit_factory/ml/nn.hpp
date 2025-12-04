#pragma once

#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <optional>
#include <ranges>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace bit_factory::ml::nn {

struct edge_t {
  std::size_t from = 0, to = 0;
  friend auto operator<=>(edge_t const&, edge_t const&) = default;
};

using hidden_nodes_t = std::map<std::string, std::size_t>;
using io_nodes_t = std::map<std::string, std::size_t>;
using edge_map_t = std::map<edge_t, double>;
using weights_t = std::vector<double>;
using ids_t = std::vector<std::size_t>;

class data_base_t {
  edge_map_t input_edges_, output_edges_;
  hidden_nodes_t hidden_nodes_;
  io_nodes_t input_nodes_, output_nodes_;

  static double get_strengh(edge_map_t const& edge_map, edge_t edge,
                            double default_) {
    if (auto found = edge_map.find(edge); found != edge_map.end())
      return found->second;
    return default_;
  }
  static void set_strengh(edge_map_t& edge_map, edge_t edge, double weight) {
    edge_map[edge] = weight;
  }

  static void add_to(io_nodes_t& to, std::vector<std::string> const& in) {
    to.insert_range(std ::views::zip(
        in, std::views::iota(to.size(), to.size() + in.size())));
  }

  static std::optional<std::size_t> get_io_id(io_nodes_t const& from,
                                              std::string const& token) {
    if (auto found = from.find(token); found != from.end())
      return found->second;
    return {};
  }

 public:
  data_base_t& add_in(std::vector<std::string> const& in) {
    add_to(input_nodes_, in);
    return *this;
  }
  data_base_t& add_out(std::vector<std::string> const& out) {
    add_to(output_nodes_, out);
    return *this;
  }

  std::optional<std::size_t> get_in_id(std::string const& token) {
    return get_io_id(input_nodes_, token);
  }
  std::optional<std::size_t> get_out_id(std::string const& token) {
    return get_io_id(output_nodes_, token);
  }
  edge_map_t const& input_edges() const { return input_edges_; }
  edge_map_t const& output_edges() const { return output_edges_; }
  hidden_nodes_t const& hidden_nodes() const { return hidden_nodes_; }

  double get_input_strengh(edge_t edge) const {
    return get_strengh(input_edges_, edge, -0.2);
  }
  double get_output_strengh(edge_t edge) const {
    return get_strengh(output_edges_, edge, 0.0);
  }

  void set_input_strengh(edge_t edge, double weight) {
    set_strengh(input_edges_, edge, weight);
  }
  void set_output_strengh(edge_t edge, double weight) {
    set_strengh(output_edges_, edge, weight);
  }

  void generate_hidden_node(std::vector<std::size_t> input_ids,
                            std::vector<std::size_t> const& output_ids) {
    std::ranges::sort(input_ids);
    auto id = std::format("{}", input_ids);
    if (auto found = hidden_nodes_.find(id); found != hidden_nodes_.end())
      return;
    auto hidden_id = hidden_nodes_.size();
    hidden_nodes_[id] = hidden_id;
    for (auto input_id : input_ids)
      set_strengh(input_edges_, edge_t{.from = input_id, .to = hidden_id},
                  1.0 / static_cast<double>(input_ids.size()));
    for (auto output_id : output_ids)
      set_strengh(output_edges_, edge_t{.from = hidden_id, .to = output_id},
                  0.1);
  }

  ids_t get_hidden_ids(ids_t const& in_ids, ids_t const& out_ids) const {
    std::set<std::size_t> hidden_ids;
    for (auto const& edge :
         input_edges_ | std::views::filter([&](auto const& node) {
           return std::ranges::find(in_ids, node.first.from) != in_ids.end();
         }) | std::views::keys)
      hidden_ids.insert(edge.to);
    for (auto const& edge :
         output_edges_ | std::views::filter([&](auto const& node) {
           return std::ranges::find(out_ids, node.first.to) != out_ids.end();
         }) | std::views::keys)
      hidden_ids.insert(edge.from);
    return ids_t{std::from_range, hidden_ids};
  }
};

inline auto init_weights(std::size_t size, double init = 1.0) {
  return weights_t{std::from_range, std::views::repeat(init, size)};
}
inline void init_weights(weights_t& weights, std::size_t size) {
  weights = init_weights(size);
}

class query_t {
  weights_t ai_, ah_, ao_;
  std::vector<weights_t> wi_, wo_;
  ids_t input_ids_, hidden_ids_, output_ids_;

  void init_all_weights() {
    init_weights(ai_, input_ids_.size());
    init_weights(ah_, hidden_ids_.size());
    init_weights(ao_, output_ids_.size());
  }

  auto dtanh(double y) { return 1.0 - y * y; }

 public:
  query_t(data_base_t const& db, ids_t const& input_ids,
          ids_t const& output_ids) {
    input_ids_ = input_ids;
    hidden_ids_ = db.get_hidden_ids(input_ids, output_ids);
    output_ids_ = output_ids;

    init_all_weights();

    wi_.resize(input_ids_.size());
    for (auto i : input_ids_) {
      wi_[i].resize(hidden_ids_.size());
      for (auto h : hidden_ids_)
        wi_[i][h] = db.get_input_strengh({.from = i, .to = h});
    }

    wo_.resize(hidden_ids_.size());
    for (auto h : hidden_ids_) {
      wo_[h].resize(output_ids_.size());
      for (auto o : output_ids_)
        wo_[h][o] = db.get_output_strengh({.from = h, .to = o});
    }
  }

  weights_t feed_forward() {
    for (auto h : std::views::iota(0u, hidden_ids_.size())) {
      auto sum = 0.0;
      for (auto i : std::views::iota(0u, input_ids_.size()))
        sum += ai_[i] * wi_[i][h];
      ah_[h] = std::tanh(sum);
    }

    for (auto o : std::views::iota(0u, output_ids_.size())) {
      auto sum = 0.0;
      for (auto h : std::views::iota(0u, hidden_ids_.size()))
        sum += ah_[h] * wo_[h][o];
      ao_[o] = std::tanh(sum);
    }

    return ao_;
  }

  void back_propagate(weights_t const& targets, double n = 0.5) {
    // calculate errors for output
    weights_t output_deltas;
    init_weights(output_deltas, output_ids_.size());
    for (auto o : std::views::iota(0u, output_ids_.size()))
      output_deltas[o] = dtanh(ao_[o]) * (targets[o] - ao_[o]);

    // calculate errors for hiddend layer
    weights_t hidden_deltas;
    init_weights(hidden_deltas, hidden_ids_.size());
    for (auto h : std::views::iota(0u, hidden_ids_.size())) {
      auto error = 0.0;
      for (auto o : std::views::iota(0u, output_ids_.size()))
        error += output_deltas[o] * wo_[h][o];
      hidden_deltas[h] = dtanh(ah_[h]) * error;
    }

    // update output weights
    for (auto h : std::views::iota(0u, hidden_ids_.size()))
      for (auto o : std::views::iota(0u, output_ids_.size()))
        wo_[h][o] += output_deltas[o] * ah_[h] * n;

    // update input weights
    for (auto i : std::views::iota(0u, input_ids_.size()))
      for (auto h : std::views::iota(0u, hidden_ids_.size()))
        wi_[i][h] += hidden_deltas[h] * ai_[i] * n;
  }

  void update(data_base_t& db) {
    for (auto i : std::views::iota(0u, input_ids_.size()))
      for (auto h : std::views::iota(0u, hidden_ids_.size()))
        db.set_input_strengh({.from = input_ids_[i], .to = hidden_ids_[h]},
                             wi_[i][h]);
    for (auto h : std::views::iota(0u, hidden_ids_.size()))
      for (auto o : std::views::iota(0u, output_ids_.size()))
        db.set_output_strengh({.from = hidden_ids_[h], .to = output_ids_[o]},
                              wo_[h][o]);
  }
};

inline std::size_t index_of(auto const& vector, auto value) {
  auto found = std::ranges::find(vector, value);
  return vector.begin() - found;
}

void train(data_base_t& db, ids_t const& input_ids, ids_t const& output_ids,
           std::size_t answer_id) {
  query_t query{db, input_ids, output_ids};
  query.feed_forward();
  weights_t targets = init_weights(output_ids.size(), 0.0);
  targets[index_of(output_ids, answer_id)] = 1.0;
  query.back_propagate(targets);
  query.update(db);
}

}  // namespace bit_factory::ml::nn
