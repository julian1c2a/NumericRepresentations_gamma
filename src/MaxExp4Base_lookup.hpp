#ifndef NUMREPR_INCLUDE_CORE_INTERNAL_MATH_TABLES_MAXEXP4BASE_LOOKUP_HPP_INCLUDED
#define NUMREPR_INCLUDE_CORE_INTERNAL_MATH_TABLES_MAXEXP4BASE_LOOKUP_HPP_INCLUDED

#include <array>
#include <cstdint>
#include <utility>

namespace NumRepr {
namespace AuxFunc {
namespace LUT {

/**
 * @brief Tabla generada de exponentes máximos para uint64_t.
 */
constexpr std::array<std::pair<uint64_t, uint64_t>, 30> base_maxexp_table = {{
    {0ull, ~0ull},
    {1ull, ~0ull},
    {2ull, 63},
    {3ull, 40},
    {4ull, 31},
    {5ull, 27},
    {6ull, 24},
    {7ull, 22},
    {8ull, 21},
    {9ull, 20},
    {10ull, 19},
    {11ull, 18},
    {12ull, 17},
    {14ull, 16},
    {16ull, 15},
    {20ull, 14},
    {24ull, 13},
    {31ull, 12},
    {41ull, 11},
    {57ull, 10},
    {85ull, 9},
    {139ull, 8},
    {256ull, 7},
    {566ull, 6},
    {1626ull, 5},
    {7132ull, 4},
    {65536ull, 3},
    {2642246ull, 2},
    {4294967296ull, 1},
    {4294967297ull, 1}
}};

} // namespace LUT
} // namespace AuxFunc
} // namespace NumRepr

#endif // NUMREPR_INCLUDE_CORE_INTERNAL_MATH_TABLES_MAXEXP4BASE_LOOKUP_HPP_INCLUDED
