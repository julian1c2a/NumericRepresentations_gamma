#ifndef NUMREPR_INCLUDE_CORE_INTERNAL_MATH_INTROOT_HPP_INCLUDED
#define NUMREPR_INCLUDE_CORE_INTERNAL_MATH_INTROOT_HPP_INCLUDED

#include "../append/integers.hpp"
// #include "Int_ExpLog.hpp" // REMOVIDO: Usamos estándar <bit>

#include <limits>
#include <type_traits>
#include <bit> // Para std::bit_width (C++20)

namespace NumRepr {
namespace AuxFunc {

// =============================================================================
// FUNCTOR GENÉRICO NEWTON-RAPHSON
// =============================================================================

// Helper para Newton-Raphson en compile-time
template <uint64_t n, uint64_t x0>
constexpr uint64_t floorsqrt_ct_newton() noexcept {
  if constexpr (x0 == 0) { 
    return 0; 
  } else if constexpr (x0 * x0 == n) { 
    return x0; 
  } else { 
    return floorsqrt_ct_newton<n, (x0 + (n / x0)) / 2>(); 
  }
}

namespace detail {
template <uint64_t n> struct floorsqrt_ct_helper {
  // REEMPLAZO: Usamos std::bit_width para calcular log2 en tiempo de compilación
  // std::bit_width(n) retorna el número de bits necesarios para representar n.
  // log2(n) es aproximadamente bit_width(n) - 1 para n > 0.
  static constexpr uint64_t log2_n = (n == 0) ? 0 : (std::bit_width(n) - 1);
  
  static constexpr uint64_t x0_base = (1ull << (log2_n / 2));
  static constexpr uint64_t x0 {
      (x0_base * x0_base < n) ? (x0_base * 2) : x0_base 
  };
  static constexpr uint64_t value { floorsqrt_ct_newton<n, x0>() };
};
} // namespace detail

template <uint64_t n> constexpr uint64_t floorsqrt_ct() noexcept {
  if constexpr (n < 2)
    return n;
  return detail::floorsqrt_ct_helper<n>::value;
}

template <typename T> constexpr T floorsqrt(T n) noexcept {
  if (n < 0) {
    return 0; // Error domain
  }
  if (n == 0 || n == 1) {
    return n;
  }

  using UnsignedT = std::make_unsigned_t<T>;
  UnsignedT un = static_cast<UnsignedT>(n);

  // Estimación inicial usando std::bit_width
  // bit_width devuelve int, hacemos cast seguro o usamos directamente
  int bw {std::bit_width(un)};
  UnsignedT x0 = UnsignedT(1) << (bw / 2);
  
  if (x0 * x0 < un) {
    x0 <<= 1; // Ajuste si subestimamos
  }

  // Newton-Raphson
  UnsignedT x1 { (x0 + un / x0) / 2 };
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

// ¡Ojo!: Posible overflow si n es grande cercano a numeric_limits<T>::max()
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

#endif // NUMREPR_INCLUDE_CORE_INTERNAL_MATH_INTROOT_HPP_INCLUDED