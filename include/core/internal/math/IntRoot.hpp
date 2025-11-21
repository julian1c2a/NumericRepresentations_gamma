#ifndef NUMREPR_CORE_INTERNAL_MATH_ROOTS_HPP
#define NUMREPR_CORE_INTERNAL_MATH_ROOTS_HPP

#include "../../append/integers.hpp"
#include "exp_log.hpp" // Necesario para bit_width y int_log2

#include <limits>
#include <type_traits>

namespace NumRepr {
namespace AuxFunc {

// =============================================================================
// RAÍCES CUADRADAS ENTERAS (Newton-Raphson)
// =============================================================================

// Helper para Newton-Raphson en compile-time
template <uint64_t n, uint64_t x0>
constexpr uint64_t floorsqrt_ct_newton() noexcept {
  return (x0 == 0) ? 0
                   : (((x0 + n / x0) / 2) >= x0
                          ? x0
                          : floorsqrt_ct_newton<n, (x0 + n / x0) / 2>());
}

namespace detail {
template <uint64_t n> struct floorsqrt_ct_helper {
  static constexpr uint64_t log2_n = (n == 0) ? 0 : int_log2_ct<n>();
  static constexpr uint64_t x0_base = (1ull << (log2_n / 2));
  static constexpr uint64_t x0 =
      (x0_base * x0_base < n) ? (x0_base * 2) : x0_base;
  static constexpr uint64_t value = floorsqrt_ct_newton<n, x0>();
};
} // namespace detail

template <uint64_t n> constexpr uint64_t floorsqrt_ct() noexcept {
  if constexpr (n < 2)
    return n;
  return detail::floorsqrt_ct_helper<n>::value;
}

template <typename T> constexpr T floorsqrt(T n) noexcept {
  if (n < 0)
    return 0; // Error domain
  if (n == 0 || n == 1)
    return n;

  using UnsignedT = std::make_unsigned_t<T>;
  UnsignedT un = static_cast<UnsignedT>(n);

  // Estimación inicial usando bit_width (de exp_log.hpp)
  UnsignedT x0 = UnsignedT(1) << (bit_width(un) / 2);
  if (x0 * x0 < un)
    x0 <<= 1; // Ajuste si subestimamos

  // Newton-Raphson
  UnsignedT x1 = (x0 + un / x0) / 2;
  while (x1 < x0) {
    x0 = x1;
    x1 = (x0 + un / x0) / 2;
  }
  return static_cast<T>(x0);
}

template <uint64_t n> consteval uint64_t ceilsqrt_ct() noexcept {
  if constexpr (n == 0)
    return 0;
  constexpr uint64_t root = floorsqrt_ct<n>();
  return (root * root == n) ? root : root + 1;
}

template <typename T> constexpr T ceilsqrt(T n) noexcept {
  if (n <= 0)
    return 0;
  T root = floorsqrt(n);
  return (root * root == n) ? root : root + 1;
}

template <typename T> constexpr bool is_perfect_square(T n) noexcept {
  if (n < 0)
    return false;
  if (n == 0 || n == 1)
    return true;
  T root = floorsqrt(n);
  return root * root == n;
}

template <uint64_t n> consteval bool is_perfect_square_ct() noexcept {
  if constexpr (n == 0 || n == 1)
    return true;
  constexpr auto root = floorsqrt_ct<n>();
  return root * root == n;
}

} // namespace AuxFunc
} // namespace NumRepr

#endif // NUMREPR_CORE_INTERNAL_MATH_ROOTS_HPP
