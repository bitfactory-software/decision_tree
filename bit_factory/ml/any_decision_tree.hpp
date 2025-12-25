#pragma once

#include <algorithm>
#include <array>
#include <bit_factory/anyxx.hpp>
#include <cmath>
#include <format>
#include <generator>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <ranges>
#include <set>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// cppcheck-suppress-begin unknownMacro

namespace bit_factory::ml::any_decision_tree {

ANY(value,
    (ANY_METHOD_DEFAULTED(bool, take_true_path, (value<> const&), const,
                          [&x](value<> const& rhs) {
                            if constexpr (std::is_arithmetic_v<T> &&
                                          !std::same_as<T, bool>) {
                              return x >=
                                     *anyxx::unchecked_unerase_cast<T>(rhs);
                            } else {
                              return x ==
                                     *anyxx::unchecked_unerase_cast<T>(rhs);
                              ;
                            }
                          }),
     ANY_METHOD_DEFAULTED(std::string, to_string, (), const,
                          [&x]() { return std::format("{}", x); }),
     ANY_METHOD_DEFAULTED(std::string, splits_op, (), const,
                          [&x]() {
                            if constexpr (std::is_arithmetic_v<T> &&
                                          !std::same_as<T, bool>) {
                              return " >= ";
                            } else {
                              return " == ";
                            }
                          }),
     ANY_OP_DEFAULTED(bool, <, less, (value<> const&), const,
                      [x](value<> const& rhs) {
                        return x < *anyxx::unchecked_unerase_cast<T>(rhs);
                      }),
     ANY_OP_DEFAULTED(bool, ==, eq, (value<> const&), const,
                      [x](value<> const& rhs) {
                        return x == *anyxx::unchecked_unerase_cast<T>(rhs);
                      })),
    anyxx::const_observer, )

ANY(row,
    (ANY_OP_MAP_NAMED(value<>, [], subscript, (std::size_t), const),
     ANY_OP_DEFAULTED(bool, ==, eq, (row<> const&), const,
                      [x](row<> const& rhs) {
                        return x == *anyxx::unchecked_unerase_cast<T>(rhs);
                      })),
    anyxx::const_observer, )

ANY(observation,
    (ANY_OP_MAP_NAMED(std::optional<value<>>, [], subscript, (std::size_t),
                      const)),
    anyxx::const_observer, )

ANY(sheet,
    (ANY_OP_MAP_NAMED(std::generator<row<>>, (), rows, (), const),
     ANY_METHOD(std::string, column_header, (std::size_t), const),
     ANY_METHOD(std::size_t, column_count, (), const)),
    anyxx::const_observer, )

using rows_t = std::vector<row<>>;
using rows_set_t = std::set<row<>>;
using result_counts_t = std::map<value<>, double>;
struct split_set {
  rows_t rows;
  std::generator<row<>> operator()() const {
    for (auto const& row : rows) co_yield row;
  }
};
using split_sets_t = std::array<split_set, 2>;
using result_t = typename result_counts_t::value_type;

struct print_result {
  result_counts_t const& result_counts;

  friend std::ostream& operator<<(std::ostream& os, print_result const& print) {
    std::string seperator;
    os << "{";
    for (auto result_count : print.result_counts)
      os << std::exchange(seperator, ", ") << result_count.first.to_string()
         << ": " << result_count.second;
    return os << "}";
  }
};

[[nodiscard]] inline std::string to_string(
    result_counts_t const& result_counts) {
  std::stringstream s;
  s << print_result{result_counts};
  return s.str();
}

struct column_value_t {
  std::size_t column = {};
  value<> v;
};

struct tree_t;
struct children_t {
  std::unique_ptr<tree_t> true_path, false_path;
};
using node_data_t = std::variant<children_t, result_counts_t>;
struct tree_t {
  sheet<> sheet_;
  column_value_t column_value;
  node_data_t node_data;

  explicit operator bool() const {
    if (auto const* children = std::get_if<children_t>(&node_data))
      return children->true_path && children->false_path;
    return true;
  }
};

struct print_coumn_value {
  sheet<> const& sheet_;
  column_value_t const& column_value;
  friend std::ostream& operator<<(std::ostream& os,
                                  print_coumn_value const& print) {
    return os << print.sheet_.column_header(print.column_value.column)
              << print.column_value.v.splits_op() << std::boolalpha
              << print.column_value.v.to_string() << "?\n";
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
      os << print_coumn_value{p.node.sheet_, p.node.column_value};
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

[[nodiscard]] inline std::string to_string(tree_t const& tree) {
  std::stringstream s;
  s << print_node{tree, ""};
  return s.str();
}

[[nodiscard]] inline split_sets_t split_table_by_column_value(
    std::size_t i, auto const& get_rows, value<> const& v) {
  split_sets_t split_sets;
  for (auto const& row : get_rows())
    split_sets[row[i].take_true_path(v) ? 0 : 1].rows.push_back(row);
  return split_sets;
}

[[nodiscard]] inline value<> get_predict_value(sheet<> const& sheet_,
                                               row<> const& r) {
  return r[sheet_.column_count() - 1];
}

[[nodiscard]] inline result_counts_t result_counts(sheet<> const& sheet_,
                                                   auto const& get_rows) {
  result_counts_t counts;
  for (auto const& row : get_rows()) ++counts[get_predict_value(sheet_, row)];
  return counts;
}

[[nodiscard]] inline double result_counts_total(
    result_counts_t const& result_counts) {
  return std::ranges::fold_left(result_counts | std::views::values, 0.0,
                                std::plus<double>{});
}

[[nodiscard]] inline double gini_impurity(result_counts_t const& counts) {
  double total = result_counts_total(counts);
  auto impurity = 0.0;
  for (auto const& [k1, count1] : counts)
    for (auto const& [k2, count2] : counts)
      if (k1 != k2) impurity += (count1 / total) * (count2 / total);
  return impurity;
}

[[nodiscard]] inline double entropy(result_counts_t const& counts) {
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

[[nodiscard]] inline gain_t find_best_gain(sheet<> sheet_, auto const& get_rows,
                                           gain_t best_gain,
                                           double current_score,
                                           auto score_function) {
  for (auto i : std::views::iota(0u, sheet_.column_count() - 1)) {
    std::set<value<>> column_values;
    auto row_count = 0.0;
    for (auto const& row : get_rows()) {
      column_values.insert(row[i]);
      ++row_count;
    }
    for (const auto& value : column_values) {
      auto split_sets = split_table_by_column_value(i, get_rows, value);
      double p = static_cast<double>(split_sets[0].rows.size()) / row_count;
      double possible_gain =
          current_score -
          p * score_function(result_counts(sheet_, split_sets[0])) -
          (1 - p) * score_function(result_counts(sheet_, split_sets[1]));
      if (possible_gain > best_gain.gain && !split_sets[0].rows.empty() &&
          !split_sets[1].rows.empty())
        best_gain = {possible_gain, {i, value}, split_sets};
    }
  }
  return best_gain;
}

[[nodiscard]] inline tree_t build_tree(sheet<> const& sheet_,
                                       auto const& get_rows,
                                       auto score_function) {
  if (auto best_gain = find_best_gain(
          sheet_, get_rows,
          gain_t{.gain = 0.0, .criteria = {}, .split_sets = {}},
          score_function(result_counts(sheet_, get_rows)), score_function);
      best_gain.gain > 0) {
    return tree_t{.sheet_ = sheet_,
                  .column_value = best_gain.criteria,
                  .node_data = node_data_t{children_t{
                      .true_path = std::make_unique<tree_t>(build_tree(
                          sheet_, best_gain.split_sets[0], score_function)),
                      .false_path = std::make_unique<tree_t>(build_tree(
                          sheet_, best_gain.split_sets[1], score_function))}}};
  } else
    return tree_t{.sheet_ = sheet_,
                  .column_value = {},
                  .node_data = result_counts(sheet_, get_rows)};
}  // NOLINT(clang-analyzer-cplusplus.NewDeleteLeaks)

[[nodiscard]] tree_t build_tree(sheet<> const& sheet_) {
  return build_tree(sheet_, sheet_, &entropy);
}

[[nodiscard]] inline result_counts_t classify(tree_t const& tree,
                                              observation<> const& probe) {
  if (auto result = std::get_if<result_counts_t>(&tree.node_data))
    return *result;
  auto const& children = std::get<children_t>(tree.node_data);
  auto query_value = probe[tree.column_value.column];
  if (!query_value || !children.true_path || !children.false_path) return {};
  if (query_value->take_true_path(tree.column_value.v))
    return classify(*children.true_path, probe);
  else
    return classify(*children.false_path, probe);
}

[[nodiscard]] inline double sum(result_counts_t const& result_counts) {
  auto total = 0.0;
  for (auto const& [result, count] : result_counts) total += count;
  return total;
}

inline void add_weighted(result_counts_t& to, const result_counts_t& from,
                         double weight) {
  for (auto const& [result, count] : from) to[result] += count * weight;
}

[[nodiscard]] result_counts_t classify_with_missing_data(
    tree_t const& tree, observation<> const& probe);

[[nodiscard]] inline result_counts_t combine_children_of_missing_data_node(
    children_t const& children, observation<> const& probe) {
  auto result_true = classify_with_missing_data(*children.true_path, probe);
  auto result_false = classify_with_missing_data(*children.false_path, probe);
  auto sum_true = sum(result_true);
  auto sum_false = sum(result_false);
  auto sum_both = sum_true + sum_false;
  result_counts_t combined_result_counts;
  add_weighted(combined_result_counts, result_true, sum_true / sum_both);
  add_weighted(combined_result_counts, result_false, sum_false / sum_both);
  return combined_result_counts;
}

[[nodiscard]] inline result_counts_t classify_with_missing_data(
    tree_t const& tree, observation<> const& probe) {
  if (auto result = std::get_if<result_counts_t>(&tree.node_data))
    return *result;

  auto const& children = std::get<children_t>(tree.node_data);
  auto query_value = probe[tree.column_value.column];
  if (!query_value)
    return combine_children_of_missing_data_node(children, probe);
  else if (query_value->take_true_path(tree.column_value.v))
    return classify_with_missing_data(*children.true_path, probe);
  else
    return classify_with_missing_data(*children.false_path, probe);
}

[[nodiscard]] inline result_counts_t as_one(result_counts_t l,
                                            result_counts_t const& r) {
  for (auto [value, count] : r) l[value] += count;
  return l;
}

[[nodiscard]] inline double score_unpruned(const result_counts_t& true_result,
                                           const result_counts_t& false_result,
                                           auto score) {
  return (score(true_result) + score(false_result)) / 2.0;
}

[[nodiscard]] inline double gain(const result_counts_t& pruned,
                                 const result_counts_t& true_result,
                                 const result_counts_t& false_result,
                                 auto score) {
  return score(pruned) - score_unpruned(true_result, false_result, score);
}

[[nodiscard]] inline children_t prune_children(tree_t const& tree,
                                               double min_gain, auto score) {
  auto const& original_children = std::get<children_t>(tree.node_data);
  return {.true_path = std::make_unique<tree_t>(
              prune(*original_children.true_path, min_gain, score)),
          .false_path = std::make_unique<tree_t>(
              prune(*original_children.false_path, min_gain, score))};
}

[[nodiscard]] inline tree_t prune(tree_t const& tree, double min_gain,
                                  auto score) {
  if (std::holds_alternative<result_counts_t>(tree.node_data))
    return tree_t{.sheet_ = tree.sheet_,
                  .column_value = tree.column_value,
                  .node_data = std::get<result_counts_t>(tree.node_data)};

  tree_t pruned{.sheet_ = tree.sheet_,
                .column_value = tree.column_value,
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

[[nodiscard]] inline tree_t prune(tree_t const& tree, double min_gain) {
  return prune(tree, min_gain, &entropy);
}

}  // namespace bit_factory::ml::any_decision_tree

// cppcheck-suppress-end unknownMacro
