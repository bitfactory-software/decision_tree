#pragma once

#include <array>
#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <ranges>
#include <set>
#include <tuple>
#include <type_traits>
#include <variant>
#include <vector>

namespace decision_tree {

namespace detail {
template <typename T, typename... Ts>
struct unique : std::type_identity<T> {};

template <typename... Ts, typename U, typename... Us>
struct unique<std::tuple<Ts...>, U, Us...>
    : std::conditional_t<(std::is_same_v<U, Ts> || ...),
                         unique<std::tuple<Ts...>, Us...>,
                         unique<std::tuple<Ts..., U>, Us...>> {};

template <typename... Ts>
using unique_tuple = typename unique<std::tuple<>, Ts...>::type;

template <typename Tuple>
struct to_variant;

template <typename... Ts>
struct to_variant<std::tuple<Ts...>> {
  using type = std::variant<typename Ts...>;
};
}  // namespace detail

template <typename... Values>
struct data {
  // types
  using row_t = std::tuple<Values...>;
  using rows_t = std::vector<row_t>;
  using rows_set_t = std::set<row_t>;
  inline static constexpr std::size_t column_count = std::tuple_size_v<row_t>;
  inline static constexpr std::size_t predict_column = column_count - 1;
  using predict_t = std::tuple_element_t<predict_column, row_t>;
  using result_counts_t = std::map<predict_t, int>;
  using split_sets_t = std::array<rows_t, 2>;
  using result_t = typename result_counts_t::value_type;
  using unique_tuple_t = typename detail::unique<std::tuple<>, Values...>::type;
  using values_variant_t = typename detail::to_variant<unique_tuple_t>::type;

  struct column_value_t {
    std::size_t column = {};
    values_variant_t value;
    friend std::ostream& operator<<(std::ostream& os,
                                    column_value_t const& column_value) {
      os << column_value.column << ":";
      std::visit([&](const auto v) { os << v; }, column_value.value);
      return os << "?\n";
    }
  };

  struct decision_node;
  struct children_t {
    std::shared_ptr<decision_node> true_path, false_path;
  };
  using node_data_t = std::variant<children_t, result_t>;
  struct decision_node {
    column_value_t column_value;
    node_data_t node_data;
  };

  struct print {
    decision_node const& node;
    std::string indent;
    friend std::ostream& operator<<(std::ostream& os, print const& p) {
      if (auto result = std::get_if<result_t>(&p.node.node_data)) {
        return os << "{" << result->first << ": " << result->second << "}\n";
      } else {
        auto const& children = std::get<children_t>(p.node.node_data);
        os << p.node.column_value;
        os << p.indent << "T-> ";
        os << print{*children.true_path, p.indent + "   "};
        os << p.indent << "F-> ";
        os << print{*children.false_path, p.indent + "   "};
        return os;
      }
    }
  };

  template <std::size_t I, typename V>
  static constexpr bool splits(row_t const& row, V const& value) {
    if constexpr (std::is_arithmetic_v<V>) {
      return std::get<I>(row) >= value;
    } else {
      return std::get<I>(row) == value;
    }
  }

  template <std::size_t I, typename V>
  static split_sets_t split_table_by_column_value(rows_t const& rows,
                                                  V const& value) {
    std::array<rows_set_t, 2> split_sets;
    for (auto const& row : rows)
      split_sets[splits<I>(row, value) ? 0 : 1].insert(row);
    return {std::ranges::to<std::vector>(split_sets[0]),
            std::ranges::to<std::vector>(split_sets[1])};
  }

  static auto result_counts(rows_t const& rows) {
    std::map<predict_t, int> counts;
    for (auto const& row : rows) ++counts[std::get<predict_column>(row)];
    return counts;
  }

  static auto gini_impurity(rows_t const& rows) {
    double total = static_cast<double>(rows.size());
    auto counts = result_counts(rows);
    auto impurity = 0.0;
    for (auto const& [k1, count1] : counts)
      for (auto const& [k2, count2] : counts)
        if (k1 != k2) impurity += (count1 / total) * (count2 / total);
    return impurity;
  }

  static auto entropy(rows_t const& rows) {
    double total = static_cast<double>(rows.size());
    auto counts = result_counts(rows);
    auto e = 0.0;
    for (auto const& [k, count] : counts) {
      auto p = count / total;
      e -= p * std::log2(p);
    }
    return e;
  }

  struct gain_t {
    double value;
    column_value_t criteria;
    split_sets_t split_sets;
  };

  template <std::size_t Column>
  static gain_t gain(rows_t const& rows, gain_t best_gain, double current_score,
                     auto score_function) {
    if constexpr (Column < predict_column) {
      using column_t = std::tuple_element_t<Column, row_t>;
      std::set<column_t> column_values;
      for (auto const& row : rows) column_values.insert(std::get<Column>(row));
      for (const auto& value : column_values) {
        auto split_sets = split_table_by_column_value<Column>(rows, value);
        double p = split_sets[0].size() / static_cast<double>(rows.size());
        auto gain = current_score - p * score_function(split_sets[0]) -
                    (1 - p) * score_function(split_sets[1]);
        if (gain > best_gain.value && !split_sets[0].empty() &&
            !split_sets[1].empty())
          best_gain = {gain, {Column, value}, split_sets};
      }
      return gain<Column + 1>(rows, best_gain, current_score, score_function);
    } else {
      return best_gain;
    }
  }

  static decision_node build_tree(rows_t const& rows, auto score_function) {
    if (rows.empty()) return {};
    if (auto best_gain = gain<0>(rows, gain_t{.value = 0.0},
                                 score_function(rows), score_function);
        best_gain.value > 0) {
      return decision_node{
          .column_value = best_gain.criteria,
          .node_data = node_data_t{children_t{
              .true_path = std::make_shared<decision_node>(
                  build_tree(best_gain.split_sets[0], score_function)),
              .false_path = std::make_shared<decision_node>(
                  build_tree(best_gain.split_sets[1], score_function))}}};
    } else
      return decision_node{.node_data = *result_counts(rows).begin()};
  }
  static decision_node build_tree_with_entropy(rows_t const& rows) {
    return build_tree(rows, &entropy);
  }
};

}  // namespace decision_tree
