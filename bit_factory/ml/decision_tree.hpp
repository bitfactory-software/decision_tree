#pragma once

#include <array>
#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <ranges>
#include <set>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace bit_factory::ml {

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
  using type = std::variant<Ts...>;
};

template <class Tuple>
struct remove_last;

template <>
struct remove_last<std::tuple<>>;  // Define as you wish or leave undefined

template <class... Args>
struct remove_last<std::tuple<Args...>> {
 private:
  using Tuple = std::tuple<Args...>;

  template <std::size_t... n>
  static std::tuple<std::tuple_element_t<n, Tuple>...> extract(
      std::index_sequence<n...>);

 public:
  using type =
      decltype(extract(std::make_index_sequence<sizeof...(Args) - 1>()));
};

}  // namespace detail

template <typename... Values>
struct decision_tree {
  // types
  using row_t = std::tuple<Values...>;
  using rows_t = std::vector<row_t>;
  using rows_set_t = std::set<row_t>;
  inline static constexpr std::size_t column_count = std::tuple_size_v<row_t>;
  inline static constexpr std::size_t predict_column = column_count - 1;
  using predict_t = std::tuple_element_t<predict_column, row_t>;
  using result_counts_t = std::map<predict_t, double>;
  using pointer_to_rows_t = std::vector<row_t const*>;
  using split_sets_t = std::array<pointer_to_rows_t, 2>;
  using observation_t =
      typename detail::remove_last<std::tuple<std::optional<Values>...>>::type;
  using result_t = typename result_counts_t::value_type;
  using unique_tuple_t = typename detail::unique<std::tuple<>, Values...>::type;
  using values_variant_t = typename detail::to_variant<unique_tuple_t>::type;

  struct print_result {
    result_counts_t const& result_counts;

    friend std::ostream& operator<<(std::ostream& os,
                                    print_result const& print) {
      std::string seperator;
      os << "{";
      for (auto result_count : print.result_counts)
        os << std::exchange(seperator, ", ") << result_count.first << ": "
           << result_count.second;
      return os << "}";
    }
  };

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
    std::unique_ptr<decision_node> true_path, false_path;
  };
  using node_data_t = std::variant<children_t, result_counts_t>;
  struct decision_node {
    column_value_t column_value;
    node_data_t node_data;
  };

  struct print_node {
    decision_node const& node;
    std::string indent;
    friend std::ostream& operator<<(std::ostream& os, print_node const& p) {
      if (auto result = std::get_if<result_counts_t>(&p.node.node_data)) {
        return os << print_result{*result} << "\n";
      } else {
        auto const& children = std::get<children_t>(p.node.node_data);
        os << p.node.column_value;
        os << p.indent << "T-> ";
        os << print_node{*children.true_path, p.indent + "   "};
        os << p.indent << "F-> ";
        os << print_node{*children.false_path, p.indent + "   "};
        return os;
      }
    }
  };

  static pointer_to_rows_t get_pointer_to_rows(rows_t const& rows) {
    pointer_to_rows_t pointer_to_rows;
    for (auto const& row : rows) pointer_to_rows.push_back(&row);
    return pointer_to_rows;
  }

  template <std::size_t I, typename V>
  static constexpr bool splits(auto const& row, V const& value) {
    if constexpr (std::is_arithmetic_v<V>) {
      return std::get<I>(row) >= value;
    } else {
      return std::get<I>(row) == value;
    }
  }

  template <std::size_t I, typename V>
  static split_sets_t split_table_by_column_value(pointer_to_rows_t const& rows,
                                                  V const& value) {
    split_sets_t split_sets;
    for (row_t const* row : rows)
      split_sets[splits<I>(*row, value) ? 0 : 1].push_back(row);
    return split_sets;
  }

  static result_counts_t result_counts(auto const& rows, auto get_column) {
    result_counts_t counts;
    for (auto const& row : rows) ++counts[get_column(row)];
    return counts;
  }

  static result_counts_t result_counts(pointer_to_rows_t const& rows) {
    return result_counts(
        rows, [](row_t const* row) { return std::get<predict_column>(*row); });
  }

  static double result_counts_total(result_counts_t const& result_counts) {
    double total = 0.0;
    for (auto const& result_count : result_counts) total += result_count.second;
    return total;
  }

  static double gini_impurity(result_counts_t const& counts) {
    double total = result_counts_total(counts);
    auto impurity = 0.0;
    for (auto const& [k1, count1] : counts)
      for (auto const& [k2, count2] : counts)
        if (k1 != k2) impurity += (count1 / total) * (count2 / total);
    return impurity;
  }

  static double entropy(result_counts_t const& counts) {
    double total = result_counts_total(counts);
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
  static gain_t find_best_gain(pointer_to_rows_t const& rows, gain_t best_gain,
                               double current_score, auto score_function) {
    if constexpr (Column < predict_column) {
      using column_t = std::tuple_element_t<Column, row_t>;
      std::set<column_t> column_values;
      for (auto const& row : rows) column_values.insert(std::get<Column>(*row));
      for (const auto& value : column_values) {
        auto split_sets = split_table_by_column_value<Column>(rows, value);
        double p = static_cast<double>(split_sets[0].size()) /
                   static_cast<double>(rows.size());
        auto gain = current_score -
                    p * score_function(result_counts(split_sets[0])) -
                    (1 - p) * score_function(result_counts(split_sets[1]));
        if (gain > best_gain.value && !split_sets[0].empty() &&
            !split_sets[1].empty())
          best_gain = {gain, {Column, value}, split_sets};
      }
      return find_best_gain<Column + 1>(rows, best_gain, current_score,
                                        score_function);
    } else {
      return best_gain;
    }
  }

  static decision_node build_tree(pointer_to_rows_t const& rows,
                                  auto score_function) {
    if (rows.empty()) return {};
    if (auto best_gain = find_best_gain<0>(
            rows, gain_t{.value = 0.0, .criteria = {}, .split_sets = {}},
            score_function(result_counts(rows)), score_function);
        best_gain.value > 0) {
      return decision_node{
          .column_value = best_gain.criteria,
          .node_data = node_data_t{children_t{
              .true_path = std::make_unique<decision_node>(
                  build_tree(best_gain.split_sets[0], score_function)),
              .false_path = std::make_unique<decision_node>(
                  build_tree(best_gain.split_sets[1], score_function))}}};
    } else
      return decision_node{.column_value = {},
                           .node_data = result_counts(rows)};
  }  // NOLINT(clang-analyzer-cplusplus.NewDeleteLeaks)

  static decision_node build_tree(rows_t const& rows, auto score_function) {
    return build_tree(get_pointer_to_rows(rows), score_function);
  }
  static decision_node build_tree(rows_t const& rows) {
    return build_tree(rows, &entropy);
  }

  template <std::size_t I>
  static bool take_true_branch(column_value_t column_value,
                               observation_t const& observation) {
    if constexpr (I < std::tuple_size_v<observation_t>) {
      if (column_value.column > I)
        return take_true_branch<I + 1>(column_value, observation);

      using optional_column_t = std::tuple_element_t<I, observation_t>;
      using column_t = typename optional_column_t::value_type;
      return splits<I>(observation, std::get<column_t>(column_value.value));
    } else {
      return false;  // should never be reached
    }
  }

  static result_counts_t classify(decision_node const& tree,
                                  observation_t const& observation) {
    if (auto result = std::get_if<result_counts_t>(&tree.node_data))
      return *result;
    auto const& children = std::get<children_t>(tree.node_data);
    if (take_true_branch<0>(tree.column_value, observation))
      return classify(*children.true_path, observation);
    else
      return classify(*children.false_path, observation);
  }

  static double sum(result_counts_t const& result_counts) {
    auto total = 0.0;
    for (auto const& [result, count] : result_counts) total += count;
    return total;
  }

  static void add_weighted(result_counts_t& to, const result_counts_t& from,
                           double weight) {
    for (auto const& [result, count] : from) to[result] += count * weight;
  }

  static result_counts_t combine_children_of_missing_data_node(
      children_t const& children, observation_t const& observation) {
    auto result_true =
        classify_with_missing_data(*children.true_path, observation);
    auto result_false =
        classify_with_missing_data(*children.false_path, observation);
    auto sum_true = sum(result_true);
    auto sum_false = sum(result_false);
    auto sum = sum_true + sum_false;
    result_counts_t combined_result_counts;
    add_weighted(combined_result_counts, result_true, sum_true / sum);
    add_weighted(combined_result_counts, result_false, sum_false / sum);
    return combined_result_counts;
  }

  template <std::size_t I>
  static result_counts_t classify_column_with_missing_data(
      decision_node const& tree, observation_t const& observation) {
    if constexpr (I < std::tuple_size_v<observation_t>) {
      if (I < tree.column_value.column)
        return classify_column_with_missing_data<I + 1>(tree, observation);

      auto const& children = std::get<children_t>(tree.node_data);
      if (!std::get<I>(observation))
        return combine_children_of_missing_data_node(children, observation);
      else if (take_true_branch<I>(tree.column_value, observation))
        return classify_with_missing_data(*children.true_path, observation);
      else
        return classify_with_missing_data(*children.false_path, observation);
    } else {
      return {};  // never reached
    }
  }

  static result_counts_t classify_with_missing_data(
      decision_node const& tree, observation_t const& observation) {
    if (auto result = std::get_if<result_counts_t>(&tree.node_data))
      return *result;
    return classify_column_with_missing_data<0>(tree, observation);
  }

  static result_counts_t as_one(result_counts_t l, result_counts_t const& r) {
    for (auto [value, count] : r) l[value] += count;
    return l;
  }

  static void prune(decision_node& tree, double min_gain, auto score_function) {
    if (auto* children = std::get_if<children_t>(&tree.node_data)) {
      prune(*children->true_path, min_gain, score_function);
      prune(*children->false_path, min_gain, score_function);
    }

    if (auto* children = std::get_if<children_t>(&tree.node_data))
      if (auto true_result =
              std::get_if<result_counts_t>(&children->true_path->node_data))
        if (auto false_result =
                std::get_if<result_counts_t>(&children->false_path->node_data))
          if (auto pruned = as_one(*true_result, *false_result);
              (score_function(pruned) - (score_function(*true_result) +
                                         score_function(*false_result) / 2.0)) <
              min_gain)
            tree.node_data = pruned;
  }

  static void prune(decision_node& tree, double min_gain) {
    prune(tree, min_gain, &entropy);
  }
};

}  // namespace bit_factory::ml
