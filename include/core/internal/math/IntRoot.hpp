#ifndef NUMREPR_INCLUDE_CORE_INTERNAL_MATH_INTROOT_HPP_INCLUDED
#define NUMREPR_INCLUDE_CORE_INTERNAL_MATH_INTROOT_HPP_INCLUDED

#include "../append/integers.hpp"
#include <bit>
#include <limits>
#include <type_traits>

namespace NumRepr {
namespace AuxFunc {

// =============================================================================
// IMPLEMENTACIÓN BASE (Iterativa y Constexpr)
// =============================================================================

/**
 * @brief Calcula la raíz cuadrada entera (floor) usando Newton-Raphson.
 * @details Esta función es constexpr, por lo que puede ejecutarse en 
 * tiempo de compilación o ejecución. Usa un bucle while simple, evitando
 * problemas de profundidad de recursión de templates.
 */
template <typename T>
constexpr T floorsqrt_impl(T n) noexcept {
  // Manejo de errores de dominio y casos triviales
  if (n < 0) { return 0; }
  if (n == 0 || n == 1) { return n; }

  using UnsignedT = std::make_unsigned_t<T>;
  const UnsignedT un {static_cast<UnsignedT>(n)};

  // Estimación inicial usando std::bit_width (C++20)
  // sqrt(x) aprox 2^(log2(x)/2)
  const int bw {std::bit_width(un)};
  UnsignedT x0 { UnsignedT(1) << (bw / 2) };
  
  // Ajuste fino inicial para no subestimar demasiado
  if (x0 * x0 < un) { x0 <<= 1; }

  // Algoritmo Newton-Raphson iterativo
  UnsignedT x1 { (x0 + un / x0) / 2 };
  
  while (x1 < x0) {
    x0 = x1;
    x1 = (x0 + un / x0) / 2;
  }
  
  return static_cast<T>(x0);
}

// =============================================================================
// INTERFAZ PÚBLICA
// =============================================================================

/**
 * @brief Raíz cuadrada entera (suelo) en tiempo de ejecución o compilación.
 */
template <typename T> 
constexpr T floorsqrt(T n) noexcept {
  return floorsqrt_impl(n);
}

/**
 * @brief Raíz cuadrada entera (suelo) FORZADA en tiempo de compilación.
 * @note Usa consteval para garantizar que solo se ejecute en compile-time.
 */
template <uint64_t n> 
consteval uint64_t floorsqrt_ct() noexcept {
  return floorsqrt_impl(n);
}

/**
 * @brief Raíz cuadrada entera (techo).
 */
template <typename T> 
constexpr T ceilsqrt(T n) noexcept {
  if (n <= 0) { return 0; }
  const T root { floorsqrt_impl(n) };
  if (root * root == n) {
    return root;
  } else {
    return root + 1;
  }
}

/**
 * @brief Raíz cuadrada entera (techo) FORZADA en tiempo de compilación.
 */
template <uint64_t n> 
consteval uint64_t ceilsqrt_ct() noexcept {
  constexpr uint64_t root { floorsqrt_impl(n) };
  if constexpr (root * root == n) {
    return root;
  } else {
    return root + 1;
  }
}

/**
 * @brief Verifica si un número es un cuadrado perfecto.
 */
template <typename T> 
constexpr bool is_perfect_square(T n) noexcept {
  if (n < 0) { return false; }
  if (n == 0 || n == 1) { return true; }
  const T root { floorsqrt_impl(n) };
  return (root * root == n);
}

/**
 * @brief Verifica cuadrado perfecto FORZADO en tiempo de compilación.
 */
template <uint64_t n> 
consteval bool is_perfect_square_ct() noexcept {
  constexpr uint64_t root { floorsqrt_impl(n) };
  return (root * root == n);
}

} // namespace AuxFunc
} // namespace NumRepr

#endif // NUMREPR_INCLUDE_CORE_INTERNAL_MATH_INTROOT_HPP_INCLUDED