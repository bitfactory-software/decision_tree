#pragma once

#include <array>
#include <set>
#include <tuple>
#include <type_traits>
#include <vector>

namespace decision_tree {

template <typename... Values>
using row_t = std::tuple<Values...>;

template <typename... Values>
using rows_t = std::vector<row_t<Values...>>;

template <typename... Values>
using rows_set_t = std::set<row_t<Values...>>;

template <typename V>
struct node {
  V value;
};

template <std::size_t I, typename V, typename... Values>
constexpr bool splits(row_t<Values...> const& row, V const& value) {
  if constexpr (std::is_arithmetic_v<V>) {
    return std::get<I>(row) >= value;
  } else {
    return std::get<I>(row) == value;
  }
}

template <std::size_t I, typename V, typename... Values>
auto divide_table_column(rows_t<Values...> const& rows, V const& value) {
  std::array<rows_set_t<Values...>, 2> split_sets;
  for (auto const& row : rows)
    split_sets[splits<I>(row, value) ? 0 : 1].insert(row);
  return split_sets;
}

}  // namespace decision_tree
