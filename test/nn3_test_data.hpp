#pragma once

#include <string_view>
#include <string>

using std::operator""sv;
constexpr auto data_nn3{
R"(|   |   |   |RuB|L6D|   |   |N1 |N1 |
|   |   |RuB|L6D|   |   |N1 |N1 |   |
|   |RuB|L6D|   |   |N1 |N1 |   |   |
|RuB|L6D|   |   |N1 |N1 |   |   |   |
|L6D|   |   |N1 |N1 |   |   |   |N1 |
|   |   |N1 |N1 |   |   |   |N1 |N1 |
|   |N1 |N1 |   |   |   |N1 |N1 |   |
|N1 |N1 |   |   |   |N1 |N1 |   |   |
|N1 |   |   |   |N1 |N1 |   |   |   |
|   |   |   |N1 |N1 |   |   |   |   |
|   |   |N1 |N1 |   |   |   |   |   |
|   |N1 |N1 |   |   |   |   |   |RuB|
|N1 |N1 |   |   |   |   |   |RuB|N1 |
|N1 |   |   |   |   |   |RuB|N1 |N1 |
|   |   |   |   |   |RuB|N1 |N1 |   |
|   |   |   |   |RuB|N1 |N1 |   |   |
|   |   |   |RuB|N1 |N1 |   |   |L6D|
|   |   |RuB|N1 |N1 |   |   |L6D|L6D|
|   |RuB|N1 |N1 |   |   |L6D|L6D|   |
|RuB|N1 |N1 |   |   |L6D|L6D|   |L6D|
|N1 |N1 |   |   |L6D|L6D|   |L6D|   |
|N1 |   |   |L6D|L6D|   |L6D|   |   |
|   |   |L6D|L6D|   |L6D|   |   |   |
|   |L6D|L6D|   |L6D|   |   |   |N1 |
|L6D|L6D|   |L6D|   |   |   |N1 |N1 |
|L6D|   |L6D|   |   |   |N1 |N1 |   |
|   |L6D|   |   |   |N1 |N1 |   |   |
|L6D|   |   |   |N1 |N1 |   |   |   |
|   |   |   |N1 |N1 |   |   |   |D3 |
|   |   |N1 |N1 |   |   |   |D3 |D3 |
|   |N1 |N1 |   |   |   |D3 |D3 |RuB|
|N1 |N1 |   |   |   |D3 |D3 |RuB|   |
|N1 |   |   |   |D3 |D3 |RuB|   |   |
|   |   |   |D3 |D3 |RuB|   |   |   |
|   |   |D3 |D3 |RuB|   |   |   |N1 |
|   |D3 |D3 |RuB|   |   |   |N1 |N1 |
|D3 |D3 |RuB|   |   |   |N1 |N1 |   |
|D3 |RuB|   |   |   |N1 |N1 |   |   |
|RuB|   |   |   |N1 |N1 |   |   |N1 |
|   |   |   |N1 |N1 |   |   |N1 |N1 |
|   |   |N1 |N1 |   |   |N1 |N1 |   |
|   |N1 |N1 |   |   |N1 |N1 |   |   |
|N1 |N1 |   |   |N1 |N1 |   |   |   |
|N1 |   |   |N1 |N1 |   |   |   |   |
|   |   |N1 |N1 |   |   |   |   |   |
|   |N1 |N1 |   |   |   |   |   |   |
|N1 |N1 |   |   |   |   |   |   |   |
|N1 |   |   |   |   |   |   |   |RuB|
|   |   |   |   |   |   |   |RuB|N1 |
|   |   |   |   |   |   |RuB|N1 |N1 |
|   |   |   |   |   |RuB|N1 |N1 |   |
|   |   |   |   |RuB|N1 |N1 |   |   |
|   |   |   |RuB|N1 |N1 |   |   |   |
|   |   |RuB|N1 |N1 |   |   |   |   |
|   |RuB|N1 |N1 |   |   |   |   |   |
|RuB|N1 |N1 |   |   |   |   |   |RuB|
|N1 |N1 |   |   |   |   |   |RuB|RuB|
|N1 |   |   |   |   |   |RuB|RuB|   |
|   |   |   |   |   |RuB|RuB|   |L6D|
|   |   |   |   |RuB|RuB|   |L6D|   |
|   |   |   |RuB|RuB|   |L6D|   |N1 |
|   |   |RuB|RuB|   |L6D|   |N1 |N1 |
|   |RuB|RuB|   |L6D|   |N1 |N1 |   |
|RuB|RuB|   |L6D|   |N1 |N1 |   |   |
|RuB|   |L6D|   |N1 |N1 |   |   |   |
|   |L6D|   |N1 |N1 |   |   |   |N1 |
|L6D|   |N1 |N1 |   |   |   |N1 |   |
|   |N1 |N1 |   |   |   |N1 |   |   |
|N1 |N1 |   |   |   |N1 |   |   |   |
|N1 |   |   |   |N1 |   |   |   |L6D|
|   |   |   |N1 |   |   |   |L6D|N1 |
|   |   |N1 |   |   |   |L6D|N1 |N1 |
|   |N1 |   |   |   |L6D|N1 |N1 |   |
|N1 |   |   |   |L6D|N1 |N1 |   |   |
|   |   |   |L6D|N1 |N1 |   |   |L6D|
|   |   |L6D|N1 |N1 |   |   |L6D|L6D|
|   |L6D|N1 |N1 |   |   |L6D|L6D|   |
|L6D|N1 |N1 |   |   |L6D|L6D|   |   |
|N1 |N1 |   |   |L6D|L6D|   |   |L6D|
|N1 |   |   |L6D|L6D|   |   |L6D|L6D|
|   |   |L6D|L6D|   |   |L6D|L6D|L6D|
|   |L6D|L6D|   |   |L6D|L6D|L6D|   |
|L6D|L6D|   |   |L6D|L6D|L6D|   |   |
|L6D|   |   |L6D|L6D|L6D|   |   |N1 |
|   |   |L6D|L6D|L6D|   |   |N1 |   |
|   |L6D|L6D|L6D|   |   |N1 |   |   |
|L6D|L6D|L6D|   |   |N1 |   |   |RuB|
|L6D|L6D|   |   |N1 |   |   |RuB|   |
|L6D|   |   |N1 |   |   |RuB|   |N1 |
|   |   |N1 |   |   |RuB|   |N1 |N1 |
|   |N1 |   |   |RuB|   |N1 |N1 |   |
|N1 |   |   |RuB|   |N1 |N1 |   |   |
|   |   |RuB|   |N1 |N1 |   |   |   |
|   |RuB|   |N1 |N1 |   |   |   |N1 |
|RuB|   |N1 |N1 |   |   |   |N1 |N1 |
|   |N1 |N1 |   |   |   |N1 |N1 |   |
|N1 |N1 |   |   |   |N1 |N1 |   |   |
|N1 |   |   |   |N1 |N1 |   |   |D3 |
|   |   |   |N1 |N1 |   |   |D3 |   |
|   |   |N1 |N1 |   |   |D3 |   |RuB|
|   |N1 |N1 |   |   |D3 |   |RuB|   |
|N1 |N1 |   |   |D3 |   |RuB|   |L6D|
|N1 |   |   |D3 |   |RuB|   |L6D|L6D|
|   |   |D3 |   |RuB|   |L6D|L6D|L6D|
|   |D3 |   |RuB|   |L6D|L6D|L6D|   |
|D3 |   |RuB|   |L6D|L6D|L6D|   |   |
|   |RuB|   |L6D|L6D|L6D|   |   |N1 |
|RuB|   |L6D|L6D|L6D|   |   |N1 |N1 |
|   |L6D|L6D|L6D|   |   |N1 |N1 |   |
|L6D|L6D|L6D|   |   |N1 |N1 |   |   |
|L6D|L6D|   |   |N1 |N1 |   |   |   |
|L6D|   |   |N1 |N1 |   |   |   |L6D|
|   |   |N1 |N1 |   |   |   |L6D|   |
|   |N1 |N1 |   |   |   |L6D|   |   |
|N1 |N1 |   |   |   |L6D|   |   |RuB|
|N1 |   |   |   |L6D|   |   |RuB|N1 |
|   |   |   |L6D|   |   |RuB|N1 |N1 |
|   |   |L6D|   |   |RuB|N1 |N1 |   |
|   |L6D|   |   |RuB|N1 |N1 |   |   |
|L6D|   |   |RuB|N1 |N1 |   |   |   |
|   |   |RuB|N1 |N1 |   |   |   |   |
|   |RuB|N1 |N1 |   |   |   |   |N1 |
|RuB|N1 |N1 |   |   |   |   |N1 |N1 |
|N1 |N1 |   |   |   |   |N1 |N1 |   |
|N1 |   |   |   |   |N1 |N1 |   |   |
|   |   |   |   |N1 |N1 |   |   |   |
|   |   |   |N1 |N1 |   |   |   |   |
|   |   |N1 |N1 |   |   |   |   |   |
|   |N1 |N1 |   |   |   |   |   |   |
|N1 |N1 |   |   |   |   |   |   |   |
|N1 |   |   |   |   |   |   |   |   |
|   |   |   |   |   |   |   |   |RuB|
|   |   |   |   |   |   |   |RuB|   |
|   |   |   |   |   |   |RuB|   |N1 |
|   |   |   |   |   |RuB|   |N1 |N1 |
|   |   |   |   |RuB|   |N1 |N1 |   |
|   |   |   |RuB|   |N1 |N1 |   |   |
|   |   |RuB|   |N1 |N1 |   |   |   |
|   |RuB|   |N1 |N1 |   |   |   |   |
|RuB|   |N1 |N1 |   |   |   |   |   |
|   |N1 |N1 |   |   |   |   |   |   |
|N1 |N1 |   |   |   |   |   |   |   |
|N1 |   |   |   |   |   |   |   |N1 |
|   |   |   |   |   |   |   |N1 |N1 |
|   |   |   |   |   |   |N1 |N1 |   |
|   |   |   |   |   |N1 |N1 |   |   |
|   |   |   |   |N1 |N1 |   |   |N1 |
|   |   |   |N1 |N1 |   |   |N1 |N1 |
|   |   |N1 |N1 |   |   |N1 |N1 |   |
|   |N1 |N1 |   |   |N1 |N1 |   |   |
|N1 |N1 |   |   |N1 |N1 |   |   |N1 |
|N1 |   |   |N1 |N1 |   |   |N1 |N1 |
|   |   |N1 |N1 |   |   |N1 |N1 |   |
|   |N1 |N1 |   |   |N1 |N1 |   |   |
|N1 |N1 |   |   |N1 |N1 |   |   |   |
|N1 |   |   |N1 |N1 |   |   |   |L6D|
|   |   |N1 |N1 |   |   |   |L6D|L6D|
|   |N1 |N1 |   |   |   |L6D|L6D|N1 |
|N1 |N1 |   |   |   |L6D|L6D|N1 |   |
|N1 |   |   |   |L6D|L6D|N1 |   |   |
|   |   |   |L6D|L6D|N1 |   |   |   |
|   |   |L6D|L6D|N1 |   |   |   |N1 |
|   |L6D|L6D|N1 |   |   |   |N1 |N1 |
|L6D|L6D|N1 |   |   |   |N1 |N1 |   |
|L6D|N1 |   |   |   |N1 |N1 |   |   |
|N1 |   |   |   |N1 |N1 |   |   |N1 |
|   |   |   |N1 |N1 |   |   |N1 |N1 |
|   |   |N1 |N1 |   |   |N1 |N1 |   |
|   |N1 |N1 |   |   |N1 |N1 |   |   |
|N1 |N1 |   |   |N1 |N1 |   |   |RuB|
|N1 |   |   |N1 |N1 |   |   |RuB|   |
|   |   |N1 |N1 |   |   |RuB|   |   |
|   |N1 |N1 |   |   |RuB|   |   |L6D|
|N1 |N1 |   |   |RuB|   |   |L6D|N1 |
|N1 |   |   |RuB|   |   |L6D|N1 |N1 |
|   |   |RuB|   |   |L6D|N1 |N1 |   |
|   |RuB|   |   |L6D|N1 |N1 |   |   |
|RuB|   |   |L6D|N1 |N1 |   |   |N1 |
|   |   |L6D|N1 |N1 |   |   |N1 |   |
|   |L6D|N1 |N1 |   |   |N1 |   |   |
|L6D|N1 |N1 |   |   |N1 |   |   |   |
|N1 |N1 |   |   |N1 |   |   |   |   |
|N1 |   |   |N1 |   |   |   |   |N1 |
|   |   |N1 |   |   |   |   |N1 |N1 |
|   |N1 |   |   |   |   |N1 |N1 |   |
|N1 |   |   |   |   |N1 |N1 |   |   |
|   |   |   |   |N1 |N1 |   |   |   |
|   |   |   |N1 |N1 |   |   |   |N1 |
|   |   |N1 |N1 |   |   |   |N1 |N1 |
|   |N1 |N1 |   |   |   |N1 |N1 |   |
|N1 |N1 |   |   |   |N1 |N1 |   |   |
|N1 |   |   |   |N1 |N1 |   |   |   |
|   |   |   |N1 |N1 |   |   |   |L6D|
|   |   |N1 |N1 |   |   |   |L6D|RuB|
|   |N1 |N1 |   |   |   |L6D|RuB|   |
|N1 |N1 |   |   |   |L6D|RuB|   |   |
|N1 |   |   |   |L6D|RuB|   |   |N1 |
|   |   |   |L6D|RuB|   |   |N1 |N1 |
|   |   |L6D|RuB|   |   |N1 |N1 |   |
|   |L6D|RuB|   |   |N1 |N1 |   |   |
|L6D|RuB|   |   |N1 |N1 |   |   |   |
|RuB|   |   |N1 |N1 |   |   |   |   |
|   |   |N1 |N1 |   |   |   |   |   |
|   |N1 |N1 |   |   |   |   |   |   |
|N1 |N1 |   |   |   |   |   |   |   |
|N1 |   |   |   |   |   |   |   |   |
|   |   |   |   |   |   |   |   |N1 |
|   |   |   |   |   |   |   |N1 |   |
|   |   |   |   |   |   |N1 |   |N1 |
|   |   |   |   |   |N1 |   |N1 |   |
|   |   |   |   |N1 |   |N1 |   |   |
|   |   |   |N1 |   |N1 |   |   |RuB|
|   |   |N1 |   |N1 |   |   |RuB|   |
|   |N1 |   |N1 |   |   |RuB|   |   |
|N1 |   |N1 |   |   |RuB|   |   |L6D|
|   |N1 |   |   |RuB|   |   |L6D|   |
|N1 |   |   |RuB|   |   |L6D|   |N1 |
|   |   |RuB|   |   |L6D|   |N1 |N1 |
|   |RuB|   |   |L6D|   |N1 |N1 |   |
|RuB|   |   |L6D|   |N1 |N1 |   |   |
|   |   |L6D|   |N1 |N1 |   |   |N1 |
|   |L6D|   |N1 |N1 |   |   |N1 |N1 |
|L6D|   |N1 |N1 |   |   |N1 |N1 |   |
|   |N1 |N1 |   |   |N1 |N1 |   |   |
|N1 |N1 |   |   |N1 |N1 |   |   |   |
|N1 |   |   |N1 |N1 |   |   |   |   |
|   |   |N1 |N1 |   |   |   |   |   |
|   |N1 |N1 |   |   |   |   |   |L6D|
|N1 |N1 |   |   |   |   |   |L6D|L6D|
|N1 |   |   |   |   |   |L6D|L6D|   |
|   |   |   |   |   |L6D|L6D|   |RuB|
|   |   |   |   |L6D|L6D|   |RuB|N1 |
|   |   |   |L6D|L6D|   |RuB|N1 |N1 |
|   |   |L6D|L6D|   |RuB|N1 |N1 |   |
|   |L6D|L6D|   |RuB|N1 |N1 |   |   |
|L6D|L6D|   |RuB|N1 |N1 |   |   |N1 |
|L6D|   |RuB|N1 |N1 |   |   |N1 |N1 |
|   |RuB|N1 |N1 |   |   |N1 |N1 |   |
|RuB|N1 |N1 |   |   |N1 |N1 |   |   |
|N1 |N1 |   |   |N1 |N1 |   |   |   |
|N1 |   |   |N1 |N1 |   |   |   |   |
|   |   |N1 |N1 |   |   |   |   |L6D|
|   |N1 |N1 |   |   |   |   |L6D|L6D|
|N1 |N1 |   |   |   |   |L6D|L6D|   |
|N1 |   |   |   |   |L6D|L6D|   |   |
|   |   |   |   |L6D|L6D|   |   |   |
|   |   |   |L6D|L6D|   |   |   |L6D|
|   |   |L6D|L6D|   |   |   |L6D|L6D|
|   |L6D|L6D|   |   |   |L6D|L6D|N1 |
|L6D|L6D|   |   |   |L6D|L6D|N1 |   |
|L6D|   |   |   |L6D|L6D|N1 |   |   |
|   |   |   |L6D|L6D|N1 |   |   |L6D|
|   |   |L6D|L6D|N1 |   |   |L6D|L6D|
|   |L6D|L6D|N1 |   |   |L6D|L6D|   |
|L6D|L6D|N1 |   |   |L6D|L6D|   |   |
|L6D|N1 |   |   |L6D|L6D|   |   |   |
|N1 |   |   |L6D|L6D|   |   |   |   |
|   |   |L6D|L6D|   |   |   |   |   |
|   |L6D|L6D|   |   |   |   |   |   |
|L6D|L6D|   |   |   |   |   |   |RuB|
|L6D|   |   |   |   |   |   |RuB|N1 |
|   |   |   |   |   |   |RuB|N1 |N1 |
|   |   |   |   |   |RuB|N1 |N1 |   |
|   |   |   |   |RuB|N1 |N1 |   |   |
|   |   |   |RuB|N1 |N1 |   |   |   |
|   |   |RuB|N1 |N1 |   |   |   |N1 |
|   |RuB|N1 |N1 |   |   |   |N1 |N1 |
|RuB|N1 |N1 |   |   |   |N1 |N1 |   |
|N1 |N1 |   |   |   |N1 |N1 |   |   |
|N1 |   |   |   |N1 |N1 |   |   |   |
|   |   |   |N1 |N1 |   |   |   |N1 |
|   |   |N1 |N1 |   |   |   |N1 |N1 |
|   |N1 |N1 |   |   |   |N1 |N1 |   |
|N1 |N1 |   |   |   |N1 |N1 |   |   |
|N1 |   |   |   |N1 |N1 |   |   |   |
|   |   |   |N1 |N1 |   |   |   |   |
|   |   |N1 |N1 |   |   |   |   |N1 |
|   |N1 |N1 |   |   |   |   |N1 |N1 |
|N1 |N1 |   |   |   |   |N1 |N1 |   |
|N1 |   |   |   |   |N1 |N1 |   |   |
|   |   |   |   |N1 |N1 |   |   |   |
|   |   |   |N1 |N1 |   |   |   |RuB|
|   |   |N1 |N1 |   |   |   |RuB|   |
|   |N1 |N1 |   |   |   |RuB|   |   |
|N1 |N1 |   |   |   |RuB|   |   |   |
|N1 |   |   |   |RuB|   |   |   |L6D|
|   |   |   |RuB|   |   |   |L6D|L6D|
|   |   |RuB|   |   |   |L6D|L6D|N1 |
|   |RuB|   |   |   |L6D|L6D|N1 |   |
|RuB|   |   |   |L6D|L6D|N1 |   |   |
|   |   |   |L6D|L6D|N1 |   |   |   |
|   |   |L6D|L6D|N1 |   |   |   |L6D|
|   |L6D|L6D|N1 |   |   |   |L6D|L6D|
|L6D|L6D|N1 |   |   |   |L6D|L6D|   |
|L6D|N1 |   |   |   |L6D|L6D|   |L6D|
|N1 |   |   |   |L6D|L6D|   |L6D|N1 |
|   |   |   |L6D|L6D|   |L6D|N1 |N1 |
|   |   |L6D|L6D|   |L6D|N1 |N1 |   |
|   |L6D|L6D|   |L6D|N1 |N1 |   |   |
|L6D|L6D|   |L6D|N1 |N1 |   |   |N1 |
|L6D|   |L6D|N1 |N1 |   |   |N1 |   |
|   |L6D|N1 |N1 |   |   |N1 |   |   |
|L6D|N1 |N1 |   |   |N1 |   |   |   |
|N1 |N1 |   |   |N1 |   |   |   |N1 |
|N1 |   |   |N1 |   |   |   |N1 |N1 |
|   |   |N1 |   |   |   |N1 |N1 |   |
|   |N1 |   |   |   |N1 |N1 |   |   |
|N1 |   |   |   |N1 |N1 |   |   |   |
|   |   |   |N1 |N1 |   |   |   |L6D|
|   |   |N1 |N1 |   |   |   |L6D|N1 |
|   |N1 |N1 |   |   |   |L6D|N1 |N1 |
|N1 |N1 |   |   |   |L6D|N1 |N1 |   |
|N1 |   |   |   |L6D|N1 |N1 |   |   |
|   |   |   |L6D|N1 |N1 |   |   |   |
|   |   |L6D|N1 |N1 |   |   |   |D3 |
|   |L6D|N1 |N1 |   |   |   |D3 |L6D|
|L6D|N1 |N1 |   |   |   |D3 |L6D|   |
|N1 |N1 |   |   |   |D3 |L6D|   |   |
|N1 |   |   |   |D3 |L6D|   |   |L6D|
|   |   |   |D3 |L6D|   |   |L6D|   |
|   |   |D3 |L6D|   |   |L6D|   |   |
|   |D3 |L6D|   |   |L6D|   |   |   |
|D3 |L6D|   |   |L6D|   |   |   |N1 |
|L6D|   |   |L6D|   |   |   |N1 |N1 |
|   |   |L6D|   |   |   |N1 |N1 |   |
|   |L6D|   |   |   |N1 |N1 |   |   |
|L6D|   |   |   |N1 |N1 |   |   |D3 |
|   |   |   |N1 |N1 |   |   |D3 |   |
|   |   |N1 |N1 |   |   |D3 |   |   |
|   |N1 |N1 |   |   |D3 |   |   |L6D|
|N1 |N1 |   |   |D3 |   |   |L6D|   |)"sv};
