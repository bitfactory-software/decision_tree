#pragma once

#include <array>
#include <set>
#include <map>
#include <tuple>
#include <type_traits>
#include <vector>

namespace decision_tree {

template <typename... Values>
struct input {
  // types
  using row_t = std::tuple<Values...>;
  using rows_t = std::vector<row_t>;
  using rows_set_t = std::set<row_t>;
  inline static constexpr std::size_t column_count = std::tuple_size_v<row_t>;
  inline static constexpr std::size_t predict_column = column_count - 1;
  using predict_t = std::tuple_element_t<predict_column, row_t>;

  // data
  rows_t rows;

  input() = default;
  input(std::initializer_list<row_t> const& init) : rows(init) {}

  template <std::size_t I, typename V>
  static constexpr bool splits(row_t const& row, V const& value) {
    if constexpr (std::is_arithmetic_v<V>) {
      return std::get<I>(row) >= value;
    } else {
      return std::get<I>(row) == value;
    }
  }

  template <std::size_t I, typename V>
  auto divide_table_column(V const& value) const {
    std::array<rows_set_t, 2> split_sets;
    for (auto const& row : rows)
      split_sets[splits<I>(row, value) ? 0 : 1].insert(row);
    return split_sets;
  }

  auto result_counts() const {
    std::map<predict_t, int> counts;
    for (auto const& row : rows) ++counts[std::get<predict_column>(row)];
    return counts;
  }
};

}  // namespace decision_tree
