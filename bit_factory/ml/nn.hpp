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
using in_signal_t = std::string;
using in_signals_t = std::vector<in_signal_t>;
using out_signal_t = std::string;
using out_signals_t = std::vector<out_signal_t>;

struct io_ids_t {
  ids_t in, out;
};

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

  static void add_to(io_nodes_t& to, auto const& signals) {
    for (auto const& signal : signals)
      if (auto found = to.find(signal); found == to.end())
        to[signal] = to.size();
  }

  static std::optional<std::size_t> get_io_id(io_nodes_t const& from,
                                              std::string const& token) {
    if (auto found = from.find(token); found != from.end())
      return found->second;
    return {};
  }

 public:
  data_base_t& add_in(in_signals_t const& in) {
    add_to(input_nodes_, in);
    return *this;
  }
  data_base_t& add_out(out_signals_t const& out) {
    add_to(output_nodes_, out);
    return *this;
  }

  auto get_in_signals() const{
      return input_nodes_ | std::views::keys;
  }
  auto get_out_signals() const{
      return output_nodes_ | std::views::keys;
  }

  std::optional<std::size_t> get_in_id(in_signal_t const& token) {
    return get_io_id(input_nodes_, token);
  }
  std::optional<std::size_t> get_out_id(out_signal_t const& token) {
    return get_io_id(output_nodes_, token);
  }
  io_ids_t get_io_ids(in_signals_t const& in_signals,
                      out_signals_t const& out_signals) {
    return io_ids_t{
        .in = {std::from_range,
               in_signals | std::views::transform(
                                [&](in_signal_t const& signal) -> std::size_t {
                                  return *get_in_id(signal);
                                })},
        .out = {
            std::from_range,
            out_signals | std::views::transform(
                              [&](out_signal_t const& signal) -> std::size_t {
                                return *get_out_id(signal);
                              })}};
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

  void generate_hidden_node(ids_t input_ids, ids_t const& output_ids) {
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
  void generate_hidden_node(io_ids_t const& io_ids) {
    generate_hidden_node(io_ids.in, io_ids.out);
  }

  ids_t get_hidden_ids(io_ids_t const& io_ids) const {
    std::set<std::size_t> hidden_ids;
    for (auto const& edge :
         input_edges_ | std::views::filter([&](auto const& hidden_node) {
           return std::ranges::find(io_ids.in, hidden_node.first.from) !=
                  io_ids.in.end();
         }) | std::views::keys)
      hidden_ids.insert(edge.to);
    for (auto const& edge :
         output_edges_ | std::views::filter([&](auto const& hidden_node) {
           return std::ranges::find(io_ids.out, hidden_node.first.to) !=
                  io_ids.out.end();
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

inline double& at_grow(std::vector<weights_t>& w, std::size_t i,
                       std::size_t j) {
  if (w.size() <= i) w.resize(i + 1);
  if (w[i].size() < j + 1) w[i].resize(j + 1);
  return w[i][j];
}

inline double at_safe(std::vector<weights_t> const& w, std::size_t i,
                      std::size_t j) {
  if (w.size() <= i) return 0.0;
  if (w[i].size() < j + 1) return 0.0;
  return w[i][j];
}

class query_t {
  weights_t ai_, ah_, ao_;
  std::vector<weights_t> wi_, wo_;
  io_ids_t io_ids_;
  ids_t hidden_ids_;

  void init_all_weights() {
    init_weights(ai_, io_ids_.in.size());
    init_weights(ah_, hidden_ids_.size());
    init_weights(ao_, io_ids_.out.size());
  }

  auto dtanh(double y) { return 1.0 - y * y; }

 public:
  query_t(data_base_t const& db, io_ids_t const& io_ids) {
    io_ids_ = io_ids;
    hidden_ids_ = db.get_hidden_ids(io_ids);

    init_all_weights();

    for (auto i : io_ids_.in)
      for (auto h : hidden_ids_)
        at_grow(wi_, i, h) = db.get_input_strengh({.from = i, .to = h});

    for (auto h : hidden_ids_)
      for (auto o : io_ids_.out)
        at_grow(wo_, h, o) = db.get_output_strengh({.from = h, .to = o});
  }

  weights_t feed_forward() {
    for (auto h : std::views::iota(0u, hidden_ids_.size())) {
      auto sum = 0.0;
      for (auto i : std::views::iota(0u, io_ids_.in.size()))
        sum += ai_[i] * at_safe(wi_, i, h);
      ah_[h] = std::tanh(sum);
    }

    for (auto o : std::views::iota(0u, io_ids_.out.size())) {
      auto sum = 0.0;
      for (auto h : std::views::iota(0u, hidden_ids_.size()))
        sum += ah_[h] * at_safe(wo_, h, o);
      ao_[o] = std::tanh(sum);
    }

    return ao_;
  }

  void back_propagate(weights_t const& targets, double n = 0.5) {
    // calculate errors for output
    weights_t output_deltas;
    init_weights(output_deltas, io_ids_.out.size());
    for (auto o : std::views::iota(0u, io_ids_.out.size()))
      output_deltas[o] = dtanh(ao_[o]) * (targets[o] - ao_[o]);

    // calculate errors for hiddend layer
    weights_t hidden_deltas;
    init_weights(hidden_deltas, hidden_ids_.size());
    for (auto h : std::views::iota(0u, hidden_ids_.size())) {
      auto error = 0.0;
      for (auto o : std::views::iota(0u, io_ids_.out.size()))
        error += output_deltas[o] * wo_[h][o];
      hidden_deltas[h] = dtanh(ah_[h]) * error;
    }

    // update output weights
    for (auto h : std::views::iota(0u, hidden_ids_.size()))
      for (auto o : std::views::iota(0u, io_ids_.out.size()))
        at_grow(wo_, h, o) += output_deltas[o] * ah_[h] * n;

    // update input weights
    for (auto i : std::views::iota(0u, io_ids_.in.size()))
      for (auto h : std::views::iota(0u, hidden_ids_.size()))
        at_grow(wi_, i, h) += hidden_deltas[h] * ai_[i] * n;
  }

  void update(data_base_t& db) {
    for (auto i : std::views::iota(0u, io_ids_.in.size()))
      for (auto h : std::views::iota(0u, hidden_ids_.size()))
        db.set_input_strengh({.from = io_ids_.in[i], .to = hidden_ids_[h]},
                             wi_[i][h]);
    for (auto h : std::views::iota(0u, hidden_ids_.size()))
      for (auto o : std::views::iota(0u, io_ids_.out.size()))
        db.set_output_strengh({.from = hidden_ids_[h], .to = io_ids_.out[o]},
                              wo_[h][o]);
  }
};

inline std::size_t index_of(auto const& vector, auto value) {
  auto found = std::ranges::find(vector, value);
  return vector.begin() - found;
}

void train(data_base_t& db, io_ids_t const& io_ids, std::size_t answer_id) {
  db.generate_hidden_node(io_ids);
  query_t query{db, io_ids};
  query.feed_forward();
  weights_t targets = init_weights(io_ids.out.size(), 0.0);
  targets[index_of(io_ids.out, answer_id)] = 1.0;
  query.back_propagate(targets);
  query.update(db);
}

}  // namespace bit_factory::ml::nn
