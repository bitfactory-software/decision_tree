#pragma once

#include <array>
#include <cmath>
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
  using type = std::variant<std::monostate, typename Ts...>;
};

// using MyTuple =
//     std::tuple<std::vector<char>, std::vector<double>, std::vector<int>>;
// using MyVariant = typename get_variant<MyTuple>::type;
//
// using Expected = std::variant<std::monostate, char, double, int>;
// static_assert(std::is_same_v<Expected, MyVariant>);
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
  using split_sets_t = std::array<rows_set_t, 2>;
  using result_t = typename result_counts_t::value_type;
  using unique_tuple_t = typename detail::unique<std::tuple<>, Values...>::type;
  using values_variant_t = typename detail::to_variant<unique_tuple_t>::type;

  struct column_value_t {
    std::size_t column = {};
    values_variant_t value;
  };

  struct decision_node {
    column_value_t column_value;
    struct children {
      std::unique_ptr<decision_node> true_path, false_path;
    };
    std::variant<children, result_t> data;
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
  static auto split_table_by_column_value(rows_t const & rows, V const& value) {
    split_sets_t split_sets;
    for (auto const& row : rows)
      split_sets[splits<I>(row, value) ? 0 : 1].insert(row);
    return split_sets;
  }

  static auto result_counts(rows_t const & rows) {
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
    double value_XXX;
    column_value_t criteria;
    split_sets_t split_sets;
  };

  //template <std::size_t Column>
  //static gain_t gain(rows_t const& rows, gain_t const& best_gain) {
  //  if constexpr (Column < predict_column) {
  //    using column_t = std::tuple_element_t<predict_column, row_t>;
  //    std::set<column_t> column_values;
  //    for (auto const& row : rows) column_values.insert(std::get<Column>(rows));
  //    for (const auto& value : column_values) {
  //      auto split_sets =
  //    }
  //  } else {
  //    return best_gain;
  //  }
  //}

  //static decision_node build_tree(rows_t const& rows, auto score) {
  //  if (rows.empty()) return {};
  //  auto current_score = score(rows, score);

  //  auto best_gain = 0.0;
  //  auto best_criteria = column_value_t{};
  //  auto best_split_sets = split_sets_t{};
  //  for (auto column : std::views::iota(0, column_count)) {
  //    auto column_values = std::map <
  //  }
  //}
};

}  // namespace decision_tree
