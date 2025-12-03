#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <memory>
#include <optional>
#include <ranges>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

namespace bit_factory::ml::nn {

struct edge_t {
  std::size_t from = 0, to = 0;
  friend auto operator<=>(edge_t const&, edge_t const&) = default;
};
}  // namespace bit_factory::ml::nn

namespace std {
template <>
struct hash<bit_factory::ml::nn::edge_t> {
  inline size_t operator()(const bit_factory::ml::nn::edge_t& e) const {
    return e.to ^ std::hash<std::size_t>{}(e.from) + 0x9e3779b9 + (e.to << 6) +
                      (e.to >> 2);
  }
};
}  // namespace std

namespace bit_factory::ml::nn {

using hidden_nodes_t = std::unordered_map<std::string, std::size_t>;
using io_nodes_t = std::unordered_map<std::string, std::size_t>;
using edge_map_t = std::unordered_map<edge_t, double>;

struct data_t {
  edge_map_t input_edges_, output_edges_;
  hidden_nodes_t hidden_nodes_;
  io_nodes_t input_nodes_, output_nodes_;
};

static inline double get_strengh(edge_map_t const& edge_map, edge_t edge,
                                 double default_) {
  if (auto found = edge_map.find(edge); found != edge_map.end())
    return found->second;
  return default_;
}
inline double get_input_strengh(data_t const& data, edge_t edge) {
  return get_strengh(data.input_edges_, edge, -0.2);
}
inline double get_output_strengh(data_t const& data, edge_t edge) {
  return get_strengh(data.output_edges_, edge, 0.0);
}

inline void set_strengh(edge_map_t& edge_map, edge_t edge, double weight) {
  edge_map[edge] = weight;
}

inline void generate_hidden_node(data_t& data,
                                 std::vector<std::size_t> input_ids,
                                 std::vector<std::size_t> const& output_ids) {
  std::ranges::sort(input_ids);
  auto id = std::format("{}", input_ids);
  if (auto found = data.hidden_nodes_.find(id);
      found != data.hidden_nodes_.end())
    return;
  auto hidden_id = data.hidden_nodes_.size();
  data.hidden_nodes_[id] = hidden_id;
  for (auto input_id : input_ids)
    set_strengh(data.input_edges_, edge_t{.from = input_id, .to = hidden_id},
                1.0 / static_cast<double>(input_ids.size()));
  for (auto output_id : output_ids)
    set_strengh(data.output_edges_, edge_t{.from = hidden_id, .to = output_id},
                0.1);
}

}  // namespace bit_factory::ml::nn
