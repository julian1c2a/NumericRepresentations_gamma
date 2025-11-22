#ifndef NUMREPR_CORE_INTERNAL_MATH_EXP_LOG_HPP
#define NUMREPR_CORE_INTERNAL_MATH_EXP_LOG_HPP

#include "../../append/expected.hpp"
#include "../../append/integers.hpp"

#include <concepts>
#include <limits>
#include <type_traits>

namespace NumRepr {
namespace AuxFunc {

using std::numeric_limits;

enum class math_error_ec : uint8_t {
  ok,
  notsupported,
  baddomain,
  overflow,
  underflow,
  unknown
};

// =============================================================================
// 0. UTILIDADES DE EXPONENTE MÁXIMO (Movido desde math_utils.hpp)
// =============================================================================

namespace LUT {
/**
 * @brief Calcula en tiempo de compilación el exponente máximo para una base
 * dada.
 */
template <uint64_t base> consteval 
size_t max_exponent_for_base_ct() noexcept {
  if constexpr (base < 2)
    return numeric_limits<uint64_t>::max();
  else if constexpr (base == 2)
    return 63;
  else if constexpr (base == 3)
    return 40;
  else if constexpr (base == 10)
    return 19;
  else
    return 1; // Fallback seguro para bases grandes no tabuladas
}

/**
 * @brief Calcula en tiempo de ejecución el exponente máximo.
 */
constexpr size_t max_exponent_for_base(uint64_t base) noexcept {
  if (base < 2)
    return numeric_limits<uint64_t>::max();
  else if (base == 2)
    return 63;
  else if (base == 3)
    return 40;
  else if (base == 10)
    return 19;
  else
    return 1;
}
} // namespace LUT

// =============================================================================
// 1. LOGARITMOS DE 2 (Compile-time & Runtime) (Optimized & Safe)
// =============================================================================

// --- Compiletime ---

template <uint64_t n> constexpr uint64_t int_log2_ct() noexcept {
  if constexpr (n < 2u)
    return 0;
  else
    return 1 + int_log2_ct<(n >> 1)>();
}

template <uint64_t n> constexpr uint64_t int_log2_sf_ct() noexcept {
  static_assert(n > 0, "log_b(0) indefinido");
  if constexpr (n < 2u)
    return 0;
  else
    return 1 + int_log2_sf_ct<(n >> 1)>();
}

template <uint64_t n> constexpr uint64_t bit_width_ct() noexcept {
  return (n < 2u) ? 1 : int_log2_sf_ct<n>() + 1;
}

template <size_t exponent> constexpr uint64_t int_pow2_ct() noexcept {
  static_assert(exponent < 64, "Overflow en int_pow2_ct");
  return 1ull << exponent;
}

template <uint64_t num> constexpr inline bool is_power_of_2_ct() noexcept {
  return num > 0 && (num & (num - 1)) == 0;
}

// --- Runtime ---

constexpr uint64_t int_log2(uint64_t n) noexcept {
  if (n < 2u)
    return 0;
  uint64_t res = 0;
  while (n >>= 1)
    ++res;
  return res;
}

// --- Runtime & Safe ---

constexpr expected<uint64_t, math_error_ec> int_log2_sf(uint64_t n) noexcept {
  if (n == 0)
    return unexpected(math_error_ec::baddomain);
  if (n == 1)
    return 0ull;
  uint64_t res = 0;
  while (n >>= 1)
    ++res;
  return res;
}

constexpr uint64_t bit_width(uint64_t n) noexcept {
  return n == 0u ? 0 : int_log2(n) + 1;
}

// =============================================================================
// 2. POTENCIAS DE 2 (Compile-time & Runtime) (Optimized & Safe)
// =============================================================================

// --- Runtime & Safe ---

constexpr expected<uint64_t, math_error_ec> int_pow2(size_t exponent) noexcept {
  if (exponent < 64)
    return (1ull << exponent);
  else
    return unexpected(math_error_ec::overflow);
}

template <typename T> constexpr inline bool is_power_of_2(T num) noexcept {
  return num > 0 && (num & (num - 1)) == 0;
}

// =============================================================================
// 3. POTENCIAS ENTERAS GENÉRICAS (Base N)
// =============================================================================

// --- Runtime & Safe ---
/**
 * @brief Calcula base^exponent con comprobación básica de overflow (retorna 0).
 */
constexpr inline uint64_t int_pow(uint64_t base, uint32_t exponent) noexcept {
  // Usamos la utilidad LUT local
  if (exponent > LUT::max_exponent_for_base(base))
    return 0;

  if (base == 0)
    return (exponent == 0) ? 1 : 0;
  if (base == 1)
    return 1;
  if (exponent == 0)
    return 1;
  if (exponent == 1)
    return base;

  uint64_t result = 1;
  uint64_t cur = base;

  while (exponent > 0) {
    if (exponent & 1) {
      if (result > numeric_limits<uint64_t>::max() / cur)
        return 0;
      result *= cur;
    }
    exponent >>= 1;
    if (exponent > 0) {
      if (cur > numeric_limits<uint64_t>::max() / cur)
        return 0;
      cur *= cur;
    }
  }
  return result;
}

// --- Compiletime & Safe ---

/**
 * @brief Versión compile-time de int_pow.
 */
template <uint64_t base, size_t exponent>
  requires(exponent <= LUT::max_exponent_for_base_ct<base>())
constexpr inline uint64_t int_pow_ct() noexcept {
  if constexpr (exponent == 0)
    return 1;
  else if constexpr (exponent == 1)
    return base;
  else if constexpr (exponent % 2 == 0) {
    constexpr auto half = int_pow_ct<base, exponent / 2>();
    return half * half;
  } else {
    return base * int_pow_ct<base, exponent - 1>();
  }
}

// =============================================================================
// 4. LOGARITMOS EN BASE N (int_log)
// =============================================================================

// --- Compiletime & Safe ---

template <uint64_t base, int64_t n> 
constexpr int64_t int_log_ct() noexcept {
  static_assert(base > 1, "Base inválida para logaritmo");
  if constexpr (n <= 0)
    return -1;
  else if constexpr (n < static_cast<int64_t>(base))
    return 0;
  else
    return 1 + int_log_ct<base, n / base>();
}

// --- Runtime Optimized ---

constexpr int64_t int_log(uint64_t base, int64_t n) noexcept {
  if (base <= 1)
    return 0; // Error
  if (n <= 0)
    return -1;
  if (static_cast<uint64_t>(n) < base)
    return 0;
  return 1 + int_log(base, n / base);
}

// =============================================================================
// 5. CONTEO DE DÍGITOS (Helper para I/O)
// =============================================================================

constexpr inline bool pow_leq_limit(
    uint64_t b, 
    uint32_t exp,
    uint64_t limit
  ) noexcept {
  
  if (b == 0)
    return (exp == 0 ? 1 <= limit : 0 <= limit);
  if (b == 1 || exp == 0)
    return 1 <= limit;

  uint64_t result = 1;
  uint64_t cur = b;

  while (exp > 0) {
    if (exp & 1) {
      if (result > limit / cur)
        return false;
      result *= cur;
    }
    exp >>= 1;
    if (exp > 0) {
      if (cur > numeric_limits<uint64_t>::max() / cur)
        return false;
      cur *= cur;
    }
  }
  return result <= limit;
}

// Conteo de dígitos en base genérica
// --- Runtime Optimized ---

constexpr inline 
size_t count_digits_base(uint64_t n, uint64_t base) noexcept {
  if (base < 2)
    return 0;
  if (n == 0)
    return 1;
  if (base == 2)
    return static_cast<size_t>(int_log2(n)) + 1u;

  uint32_t lo = 0u, hi = 1u;
  while (pow_leq_limit(base, hi, n)) {
    lo = hi;
    hi *= 2u;
    if (hi == 0)
      break;
  }

  while (lo + 1u < hi) {
    uint32_t mid = lo + (hi - lo) / 2u;
    if (pow_leq_limit(base, mid, n))
      lo = mid;
    else
      hi = mid;
  }
  return static_cast<size_t>(lo) + 1u;
}

constexpr inline size_t count_digits_base10(uint64_t n) noexcept {
  return count_digits_base(n, 10);
}

} // namespace AuxFunc
} // namespace NumRepr

#endif // NUMREPR_CORE_INTERNAL_MATH_EXP_LOG_HPP
