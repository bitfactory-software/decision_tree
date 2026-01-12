#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <ranges>
#include <set>
#include <sstream>
#include <string>
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

template <auto Labels, typename... Values>
struct tulpe_sheet {
  // static_assert(
  //     std::same_as<decltype(Labels), const char * [sizeof...(Values)]>);
  using row_t = std::tuple<Values...>;
  inline static constexpr std::size_t column_count = std::tuple_size_v<row_t>;
  inline static constexpr std::size_t predict_column = column_count - 1;
  using predict_t = std::tuple_element_t<predict_column, row_t>;
  using observation_t =
      typename detail::remove_last<std::tuple<std::optional<Values>...>>::type;
  using unique_tuple_t = typename detail::unique<std::tuple<>, Values...>::type;
  static constexpr const std::size_t observation_size = column_count - 1;
  template <std::size_t I>
  using row_column_type = std::tuple_element_t<I, row_t>;
  template <std::size_t I>
  using observation_column_type = std::tuple_element_t<I, row_t>;

  template <std::size_t I>
  static std::optional<row_column_type<I>> get_observation_value(
      observation_t const& observation) {
    return std::get<I>(observation);
  }
  template <std::size_t I>
  static auto get_observation_value(row_t const& row) {
    return std::get<I>(row);
  }
  static auto get_predict_value(row_t const& row) {
    return std::get<column_count - 1>(row);
  }

  static std::string get_label(std::size_t index) { return Labels[index]; }
};

template <typename ObservationValue, std::size_t ObservationSize,
          typename PredictValue = ObservationValue>
struct array_sheet {
  using observation_t =
      std::array<std::optional<ObservationValue>, ObservationSize>;
  using predict_t = PredictValue;
  using row_t =
      std::pair<std::array<ObservationValue, ObservationSize>, PredictValue>;
  using unique_tuple_t = typename detail::unique<std::tuple<>, ObservationValue,
                                                 PredictValue>::type;
  static constexpr const std::size_t observation_size = ObservationSize;
  template <std::size_t I>
  using row_column_type = ObservationValue;
  template <std::size_t I>
  using observation_column_type = ObservationValue;

  template <std::size_t I>
  static std::optional<ObservationValue> get_observation_value(
      observation_t const& observation) {
    return std::get<I>(observation);
  }
  template <std::size_t I>
  static ObservationValue get_observation_value(row_t const& row) {
    return std::get<I>(row.first);
  }
  static predict_t get_predict_value(row_t const& row) { return row.second; }

  static std::string get_label(std::size_t index) {
    if (index < observation_size) return "x[" + std::to_string(index) + "]";
    return "y";
  }
};

template <typename Sheet>
struct decision_tree {
  // types
  using sheet_t = Sheet;
  using observation_t = typename Sheet::observation_t;
  using predict_t = typename Sheet::predict_t;
  using row_t = typename Sheet::row_t;
  using unique_tuple_t = typename Sheet::unique_tuple_t;
  static constexpr const std::size_t observation_size = Sheet::observation_size;
  template <std::size_t I>
  using row_column_type = typename Sheet::template row_column_type<I>;
  template <std::size_t I>
  using observation_column_type =
      typename Sheet::template observation_column_type<I>;
  template <std::size_t I>
  static auto get_observation_value(auto const& observation) {
    return Sheet::template get_observation_value<I>(observation);
  }

  using rows_t = std::vector<row_t>;
  using rows_set_t = std::set<row_t>;
  using result_counts_t = std::map<predict_t, double>;
  using pointer_to_rows_t = std::vector<row_t const*>;
  using split_sets_t = std::array<pointer_to_rows_t, 2>;
  using result_t = typename result_counts_t::value_type;
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

  [[nodiscard]] static std::string to_string(
      result_counts_t const& result_counts) {
    std::stringstream s;
    s << print_result{result_counts};
    return s.str();
  }

  struct column_value_t {
    std::size_t column = {};
    values_variant_t value;
    friend std::ostream& operator<<(std::ostream& os,
                                    column_value_t const& column_value) {
      os << Sheet::get_label(column_value.column);
      std::visit(
          [&](const auto v) {
            os << std::boolalpha << decision_tree::splits_op(v) << v;
          },
          column_value.value);
      return os << "?\n";
    }
  };

  struct tree_t;
  struct children_t {
    std::unique_ptr<tree_t> true_path, false_path;
  };
  using node_data_t = std::variant<children_t, result_counts_t>;
  struct tree_t {
    column_value_t column_value;
    node_data_t node_data;

    explicit operator bool() const {
      if (auto const* children = std::get_if<children_t>(&node_data))
        return children->true_path && children->false_path;
      return true;
    }
  };

  struct print_node {
    tree_t const& node;
    std::string indent;
    friend std::ostream& operator<<(std::ostream& os, print_node const& p) {
      if (auto result = std::get_if<result_counts_t>(&p.node.node_data)) {
        return os << print_result{*result} << "\n";
      } else {
        auto const& children = std::get<children_t>(p.node.node_data);
        os << p.node.column_value;
        if (children.true_path)
          os << p.indent << "T-> "
             << print_node{*children.true_path, p.indent + "   "};
        if (children.false_path)
          os << p.indent << "F-> "
             << print_node{*children.false_path, p.indent + "   "};
        return os;
      }
    }
  };

  [[nodiscard]] friend std::string to_string(tree_t const& tree) {
    std::stringstream s;
    s << decision_tree::print_node{tree, ""};
    return s.str();
  }

  [[nodiscard]] static pointer_to_rows_t get_pointer_to_rows(
      rows_t const& rows) {
    pointer_to_rows_t pointer_to_rows;
    std::ranges::transform(rows, std::back_inserter(pointer_to_rows),
                           [](row_t const& row) { return &row; });
    return pointer_to_rows;
  }

  template <typename V>
  [[nodiscard]] static constexpr std::string splits_op(
      [[maybe_unused]] V const& column_value) {
    if constexpr (std::is_arithmetic_v<V> && !std::same_as<V, bool>) {
      return " >= ";
    } else {
      return " == ";
    }
  }
  template <typename V>
  [[nodiscard]] static constexpr bool splits(V const& query,
                                             V const& column_value) {
    if constexpr (std::is_arithmetic_v<V> && !std::same_as<V, bool>) {
      return query >= column_value;
    } else {
      return query == column_value;
    }
  }

  template <std::size_t I, typename V>
  [[nodiscard]] static split_sets_t split_table_by_column_value(
      pointer_to_rows_t const& rows, V const& value) {
    split_sets_t split_sets;
    for (row_t const* row : rows)
      split_sets[splits(get_observation_value<I>(*row), value) ? 0 : 1]
          .push_back(row);
    return split_sets;
  }

  [[nodiscard]] static result_counts_t result_counts(auto const& rows,
                                                     auto get_column) {
    result_counts_t counts;
    for (auto const& row : rows) ++counts[get_column(row)];
    return counts;
  }

  [[nodiscard]] static result_counts_t result_counts(
      pointer_to_rows_t const& rows) {
    return result_counts(
        rows, [](row_t const* row) { return Sheet::get_predict_value(*row); });
  }

  [[nodiscard]] static double result_counts_total(
      result_counts_t const& result_counts) {
    return std::ranges::fold_left(result_counts | std::views::values, 0.0,
                                  std::plus<double>{});
  }

  [[nodiscard]] static double gini_impurity(result_counts_t const& counts) {
    double total = result_counts_total(counts);
    auto impurity = 0.0;
    for (auto const& [k1, count1] : counts)
      for (auto const& [k2, count2] : counts)
        if (k1 != k2) impurity += (count1 / total) * (count2 / total);
    return impurity;
  }

  [[nodiscard]] static double entropy(result_counts_t const& counts) {
    double total = result_counts_total(counts);
    auto e = 0.0;
    for (auto const& [k, count] : counts) {
      auto p = count / total;
      e -= p * std::log2(p);
    }
    return e;
  }

  struct gain_t {
    double gain;
    column_value_t criteria;
    split_sets_t split_sets;
  };

  template <std::size_t Column>
  static gain_t find_best_gain(pointer_to_rows_t const& rows, gain_t best_gain,
                               double current_score, auto score_function) {
    if constexpr (Column < observation_size) {
      using column_t = row_column_type<Column>;
      std::set<column_t> column_values;
      for (auto const& row : rows)
        column_values.insert(get_observation_value<Column>(*row));
      for (const column_t& value : column_values) {
        auto split_sets = split_table_by_column_value<Column>(rows, value);
        double p = static_cast<double>(split_sets[0].size()) /
                   static_cast<double>(rows.size());
        double possible_gain =
            current_score - p * score_function(result_counts(split_sets[0])) -
            (1 - p) * score_function(result_counts(split_sets[1]));
        if (possible_gain > best_gain.gain && !split_sets[0].empty() &&
            !split_sets[1].empty())
          best_gain = {possible_gain, {Column, value}, split_sets};
      }
      return find_best_gain<Column + 1>(rows, best_gain, current_score,
                                        score_function);
    } else {
      return best_gain;
    }
  }

  [[nodiscard]] static tree_t build_tree(pointer_to_rows_t const& rows,
                                         auto score_function) {
    if (rows.empty()) return {};
    if (auto best_gain = find_best_gain<0>(
            rows, gain_t{.gain = 0.0, .criteria = {}, .split_sets = {}},
            score_function(result_counts(rows)), score_function);
        best_gain.gain > 0.0) {
      return tree_t{.column_value = best_gain.criteria,
                    .node_data = node_data_t{children_t{
                        .true_path = std::make_unique<tree_t>(build_tree(
                            best_gain.split_sets[0], score_function)),
                        .false_path = std::make_unique<tree_t>(build_tree(
                            best_gain.split_sets[1], score_function))}}};
    } else
      return tree_t{.column_value = {}, .node_data = result_counts(rows)};
  }  // NOLINT(clang-analyzer-cplusplus.NewDeleteLeaks)

  [[nodiscard]] static tree_t build_tree(rows_t const& rows,
                                         auto score_function) {
    return build_tree(get_pointer_to_rows(rows), score_function);
  }
  [[nodiscard]] static tree_t build_tree(rows_t const& rows) {
    return build_tree(rows, &entropy);
  }

  template <std::size_t I, typename V>
  [[nodiscard]] static bool take_true_branch(V const& query_value,
                                             column_value_t const& column_value,
                                             observation_t const& observation) {
    if constexpr (I < observation_size) {
      if (column_value.column > I) {
        if constexpr (I < observation_size - 1) {
          return take_true_branch<I + 1>(
              *get_observation_value<I + 1>(observation), column_value,
              observation);
        } else {
          return false;  // should never be reached}
        }
      }
      using column_t = observation_column_type<I>;
      if constexpr (std::same_as<V, column_t>) {
        return splits(query_value, std::get<column_t>(column_value.value));
      } else {
        return false;  // should never be reached}
      }
    } else {
      return false;  // should never be reached
    }
  }

  [[nodiscard]] static result_counts_t classify(
      tree_t const& tree, observation_t const& observation) {
    if (auto result = std::get_if<result_counts_t>(&tree.node_data))
      return *result;
    auto const& children = std::get<children_t>(tree.node_data);
    auto query_value = get_observation_value<0>(observation);
    if (!query_value || !children.true_path || !children.false_path) return {};
    if (take_true_branch<0>(*query_value, tree.column_value, observation))
      return classify(*children.true_path, observation);
    else
      return classify(*children.false_path, observation);
  }

  [[nodiscard]] static double sum(result_counts_t const& result_counts) {
    auto total = 0.0;
    for (auto const& [result, count] : result_counts) total += count;
    return total;
  }

  static void add_weighted(result_counts_t& to, const result_counts_t& from,
                           double weight) {
    for (auto const& [result, count] : from) to[result] += count * weight;
  }

  [[nodiscard]] static result_counts_t combine_children_of_missing_data_node(
      children_t const& children, observation_t const& observation) {
    auto result_true =
        classify_with_missing_data(*children.true_path, observation);
    auto result_false =
        classify_with_missing_data(*children.false_path, observation);
    auto sum_true = sum(result_true);
    auto sum_false = sum(result_false);
    auto sum_both = sum_true + sum_false;
    result_counts_t combined_result_counts;
    add_weighted(combined_result_counts, result_true, sum_true / sum_both);
    add_weighted(combined_result_counts, result_false, sum_false / sum_both);
    return combined_result_counts;
  }

  template <std::size_t I>
  [[nodiscard]] static result_counts_t classify_column_with_missing_data(
      tree_t const& tree, observation_t const& observation) {
    if constexpr (I < std::tuple_size_v<observation_t>) {
      if (I < tree.column_value.column)
        return classify_column_with_missing_data<I + 1>(tree, observation);

      auto const& children = std::get<children_t>(tree.node_data);
      auto query_value = get_observation_value<I>(observation);
      if (!query_value)
        return combine_children_of_missing_data_node(children, observation);
      else if (take_true_branch<I>(*query_value, tree.column_value,
                                   observation))
        return classify_with_missing_data(*children.true_path, observation);
      else
        return classify_with_missing_data(*children.false_path, observation);
    } else {
      return {};  // never reached
    }
  }

  [[nodiscard]] static result_counts_t classify_with_missing_data(
      tree_t const& tree, observation_t const& observation) {
    if (auto result = std::get_if<result_counts_t>(&tree.node_data))
      return *result;
    return classify_column_with_missing_data<0>(tree, observation);
  }

  [[nodiscard]] static result_counts_t as_one(result_counts_t l,
                                              result_counts_t const& r) {
    for (auto [value, count] : r) l[value] += count;
    return l;
  }

  [[nodiscard]] static double score_unpruned(
      const result_counts_t& true_result, const result_counts_t& false_result,
      auto score) {
    return (score(true_result) + score(false_result)) / 2.0;
  }

  [[nodiscard]] static double gain(const result_counts_t& pruned,
                                   const result_counts_t& true_result,
                                   const result_counts_t& false_result,
                                   auto score) {
    return score(pruned) - score_unpruned(true_result, false_result, score);
  }

  [[nodiscard]] static children_t prune_children(tree_t const& tree,
                                                 double min_gain, auto score) {
    auto const& original_children = std::get<children_t>(tree.node_data);
    return {.true_path = std::make_unique<tree_t>(
                prune(*original_children.true_path, min_gain, score)),
            .false_path = std::make_unique<tree_t>(
                prune(*original_children.false_path, min_gain, score))};
  }

  [[nodiscard]] static tree_t prune(tree_t const& tree, double min_gain,
                                    auto score) {
    if (std::holds_alternative<result_counts_t>(tree.node_data))
      return tree_t{.column_value = tree.column_value,
                    .node_data = std::get<result_counts_t>(tree.node_data)};

    tree_t pruned{.column_value = tree.column_value,
                  .node_data = prune_children(tree, min_gain, score)};
    auto& pruned_children = std::get<children_t>(pruned.node_data);
    if (auto true_result =
            std::get_if<result_counts_t>(&pruned_children.true_path->node_data))
      if (auto false_result = std::get_if<result_counts_t>(
              &pruned_children.false_path->node_data))
        if (auto pruned_result = as_one(*true_result, *false_result);
            gain(pruned_result, *true_result, *false_result, score) < min_gain)
          pruned.node_data = pruned_result;
    return pruned;
  }

  [[nodiscard]] static tree_t prune(tree_t const& tree, double min_gain) {
    return prune(tree, min_gain, &entropy);
  }
};

}  // namespace bit_factory::ml
