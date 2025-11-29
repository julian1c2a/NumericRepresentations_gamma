#ifndef NUMREPR_INCLUDE_CORE_INTERNAL_MATH_INT_EXPLOG_HPP_INCLUDED
#define NUMREPR_INCLUDE_CORE_INTERNAL_MATH_INT_EXPLOG_HPP_INCLUDED

#include <concepts>
#include <limits>
#include <type_traits>

#include "../append/expected.hpp"
#include "../append/integers.hpp"
// Se asume que esta tabla existe y contiene std::pair<uint64_t, uint64_t>
#include "tables/MaxExp4Base_table.hpp"

namespace NumRepr::AuxFunc {

using std::numeric_limits;

// =============================================================================
// HELPER MULTIPLICACIÓN SEGURA (CONSTEXPR COMPATIBLE MSVC/GCC/CLANG)
// =============================================================================
/**
 * @brief Multiplicación con chequeo de overflow.
 * @return true si hubo overflow, false si ok. Resultado en *res.
 */
constexpr inline bool check_mul_overflow(uint64_t a, uint64_t b, uint64_t* res) noexcept {
#if defined(__GNUC__) || defined(__clang__)
    // GCC y Clang tienen este builtin que es constexpr
    return __builtin_mul_overflow(a, b, res);
#else
    // Fallback genérico para MSVC (que no tiene builtin constexpr de overflow)
    // a * b > MAX? => a > MAX / b
    if (a == 0 || b == 0) {
        *res = 0;
        return false;
    }
    if (a > std::numeric_limits<uint64_t>::max() / b) {
        *res = static_cast<uint64_t>(a * b); // Wraparound
        return true;
    }
    *res = a * b;
    return false;
#endif
}

/**
 * @brief Códigos de error para operaciones matemáticas.
 */
enum class math_error_ec : uint8_t {
  ok,
  notsupported,
  baddomain,
  overflow,
  underflow,
  unknown
};

// =============================================================================
// SEGURIDAD: LÍMITES DE EXPONENTES (Lookup Table Helpers)
// =============================================================================
namespace Safety {

constexpr inline uint64_t max_exponent_for_base(uint64_t base) noexcept {
    const auto& table = NumRepr::AuxFunc::LUT::base_maxexp_table;
    for (size_t i = table.size(); i > 0; --i) {
        const auto& entry = table[i - 1];
        if (base >= entry.first) {
            return entry.second;
        }
    }
    if (base <= 1) return ~0ull; 
    return 1; 
}

} // namespace Safety

// =============================================================================
// NAMESPACE CT: FUNCIONES COMPILE-TIME
// =============================================================================
namespace ct {

template <uint64_t n> 
constexpr uint64_t int_log2() noexcept {
  if constexpr (n < 2u) return 0;
  else return 1 + int_log2<(n >> 1)>();
}

template <uint64_t n> 
constexpr uint64_t int_log2_sf() noexcept {
  static_assert(n > 0, "NumRepr::ct::int_log2_sf: log_b(0) es indefinido");
  if constexpr (n < 2u) return 0;
  else return 1 + int_log2_sf<(n >> 1)>();
}

template <uint64_t n> 
constexpr uint64_t bit_width() noexcept {
  if constexpr (n < 2u) { 
    return 1; 
  } else {
    return int_log2_sf<n>() + 1; 
  }
}

template <size_t exponent> 
constexpr uint64_t int_pow2() noexcept {
  static_assert(exponent < 64, "NumRepr::ct::int_pow2: Overflow detectado (exp >= 64)");
  return 1ull << exponent;
}

template <uint64_t num> 
constexpr inline bool is_power_of_2() noexcept {
  return num > 0 && (num & (num - 1)) == 0;
}

// --- Helpers internos para recursión de LUT ---
namespace internal {
    template <size_t I>
    consteval uint64_t find_max_exp_recursive(uint64_t base) {
        constexpr auto& table = NumRepr::AuxFunc::LUT::base_maxexp_table;
        if (base >= table[I].first) {
            return table[I].second;
        }
        if constexpr (I > 0) {
          return find_max_exp_recursive<I - 1>(base);
        } else {
          return (base <= 1) ? ~0ull : 1; 
        }
    }
} // namespace internal

template <uint64_t base>
consteval uint64_t max_exponent_for_base() noexcept {
    using NumRepr::AuxFunc::LUT::base_maxexp_table;
    constexpr size_t last_idx {base_maxexp_table.size() - 1};
    return internal::find_max_exp_recursive<last_idx>(base);
}

template <uint64_t base, size_t exponent>
  requires(exponent <= max_exponent_for_base<base>())
constexpr inline uint64_t int_pow() noexcept {
  if constexpr (exponent == 0) return 1;
  else if constexpr (exponent == 1) return base;
  else if constexpr (exponent % 2 == 0) {
    constexpr auto half = int_pow<base, exponent / 2>();
    return half * half;
  } else {
    return base * int_pow<base, exponent - 1>();
  }
}

template <uint64_t base, int64_t n> 
constexpr int64_t int_log() noexcept {
  static_assert(base > 1, "Base invalida para logaritmo (debe ser > 1)");
  if constexpr (n <= 0) return -1;
  else if constexpr (n < static_cast<int64_t>(base)) return 0;
  else return 1 + int_log<base, n / base>();
}

} // namespace ct

// =============================================================================
// RUNTIME: FUNCIONES EJECUTABLES
// =============================================================================

constexpr uint64_t int_log2(uint64_t n) noexcept {
  if (n < 2u) return 0;
#if defined(__GNUC__) || defined(__clang__)
  return 63 - __builtin_clzll(n);
#elif defined(_MSC_VER)
  if (std::is_constant_evaluated()) {
      uint64_t res = 0;
      while (n >>= 1) ++res;
      return res;
  } else {
      unsigned long index;
      _BitScanReverse64(&index, n);
      return index;
  }
#else
  uint64_t res = 0;
  while (n >>= 1) ++res;
  return res;
#endif
}

constexpr expected<uint64_t, math_error_ec> int_log2_sf(uint64_t n) noexcept {
  if (n == 0) return unexpected(math_error_ec::baddomain);
  return int_log2(n);
}

constexpr uint64_t bit_width(uint64_t n) noexcept {
  return n == 0u ? 0 : int_log2(n) + 1;
}

constexpr expected<uint64_t, math_error_ec> int_pow2(size_t exponent) noexcept {
  if (exponent < 64) return (1ull << exponent);
  else return unexpected(math_error_ec::overflow);
}

template <typename T> 
constexpr inline bool is_power_of_2(T num) noexcept {
  return num > 0 && (num & (num - 1)) == 0;
}

/**
 * @brief Potencia entera con validación de seguridad (LUT).
 */
constexpr inline uint64_t int_pow(uint64_t base, uint32_t exponent) noexcept {
  if (exponent > Safety::max_exponent_for_base(base)) { 
      return 0; // Overflow detectado por tabla
  }
  if (base == 0) return (exponent == 0) ? 1 : 0;
  if (base == 1) return 1;
  if (exponent == 0) return 1;
  if (exponent == 1) return base;

  uint64_t result = 1;
  uint64_t cur = base;

  while (exponent > 0) {
    if (exponent & 1) {
      // CORREGIDO: Usamos check_mul_overflow wrapper
      if (check_mul_overflow(result, cur, &result)) return 0;
    }
    exponent >>= 1;
    if (exponent > 0) {
      if (check_mul_overflow(cur, cur, &cur)) return 0; 
    }
  }
  return result;
}

constexpr int64_t int_log(uint64_t base, int64_t n) noexcept {
  if (base <= 1) return 0;
  if (n <= 0) return -1;
  if (static_cast<uint64_t>(n) < base) return 0;
  return 1 + int_log(base, n / base);
}

// =============================================================================
// UTILIDADES: CONTEO DE DÍGITOS
// =============================================================================

constexpr inline bool pow_leq_limit(uint64_t b, uint32_t exp, uint64_t limit) noexcept {
  if (b == 0) return (exp == 0 ? 1 <= limit : 0 <= limit);
  if (b == 1 || exp == 0) return 1 <= limit;

  uint64_t result = 1;
  uint64_t cur = b;

  while (exp > 0) {
    if (exp & 1) {
      if (limit / cur < result) return false;
      result *= cur;
    }
    exp >>= 1;
    if (exp > 0) {
      if (numeric_limits<uint64_t>::max() / cur < cur) return false;
      cur *= cur;
    }
  }
  return result <= limit;
}

constexpr inline size_t count_digits_base(uint64_t n, uint64_t base) noexcept {
  if (base < 2) return 0;
  if (n == 0) return 1;
  if (base == 2) return static_cast<size_t>(int_log2(n)) + 1u;

  uint32_t lo = 0u, hi = 1u;
  while (pow_leq_limit(base, hi, n)) {
    lo = hi;
    hi *= 2u;
    if (hi == 0) break; 
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

} // namespace NumRepr::AuxFunc

#endif // NUMREPR_INCLUDE_CORE_INTERNAL_MATH_INT_EXPLOG_HPP_INCLUDED