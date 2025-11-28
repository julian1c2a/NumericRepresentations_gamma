#ifndef NUMREPR_INCLUDE_CORE_INTERNAL_MATH_INT_EXPLOG_HPP_INCLUDED
#define NUMREPR_INCLUDE_CORE_INTERNAL_MATH_INT_EXPLOG_HPP_INCLUDED

#include <concepts>
#include <limits>
#include <type_traits>

#include "../append/expected.hpp"
#include "../append/integers.hpp"
// Se asume que esta tabla existe y contiene std::pair<uint64_t, uint64_t>
// con {base, max_safe_exponent} ordenado o accesible.
#include "tables/MaxExp4Base_table.hpp"

namespace NumRepr::AuxFunc {

using std::numeric_limits;

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

/**
 * @brief Busca el exponente máximo seguro para una base dada (Runtime/Constexpr).
 * @details Itera sobre la tabla LUT para encontrar el límite seguro antes de que
 * base^exp desborde uint64_t.
 * @param base Base numérica.
 * @return Exponente máximo permitido.
 */
constexpr inline uint64_t max_exponent_for_base(uint64_t base) noexcept {
    // Acceso a la tabla definida en MaxExp4Base_table.hpp
    const auto& table = NumRepr::AuxFunc::LUT::base_maxexp_table;

    // Búsqueda lineal inversa (asumiendo orden ascendente de bases en la tabla
    // o rangos definidos). Si base >= entrada.base, entonces ese es su límite.
    for (size_t i = table.size(); i > 0; --i) {
        const auto& entry = table[i - 1];
        if (base >= entry.first) {
            return entry.second;
        }
    }
    // Fallback: Si la base es enorme, el exponente seguro es 1 o 0.
    // Si la base es 0 o 1, el exponente es "infinito" (aquí usamos max).
    if (base <= 1) return ~0ull; 
    return 1; 
}

} // namespace Safety

// =============================================================================
// NAMESPACE CT: FUNCIONES COMPILE-TIME (CONSTEVAL / META)
// =============================================================================
namespace ct {

/**
 * @brief Logaritmo base 2 en tiempo de compilación.
 * @return floor(log2(n))
 */
template <uint64_t n> 
constexpr uint64_t int_log2() noexcept {
  if constexpr (n < 2u) return 0;
  else return 1 + int_log2<(n >> 1)>();
}

/**
 * @brief Logaritmo base 2 seguro en tiempo de compilación.
 * @details Falla compilación si n es 0.
 */
template <uint64_t n> 
constexpr uint64_t int_log2_sf() noexcept {
  static_assert(n > 0, "NumRepr::ct::int_log2_sf: log_b(0) es indefinido");
  if constexpr (n < 2u) return 0;
  else return 1 + int_log2_sf<(n >> 1)>();
}

/**
 * @brief Calcula el ancho en bits necesario para representar n.
 */
template <uint64_t n> 
constexpr uint64_t bit_width() noexcept {
  return (n < 2u) ? 1 : int_log2_sf<n>() + 1;
}

/**
 * @brief Potencia de 2 en tiempo de compilación.
 */
template <size_t exponent> 
constexpr uint64_t int_pow2() noexcept {
  static_assert(exponent < 64, "NumRepr::ct::int_pow2: Overflow detectado (exp >= 64)");
  return 1ull << exponent;
}

/**
 * @brief Verifica si es potencia de 2.
 */
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

/**
 * @brief Obtiene exponente máximo seguro en tiempo de compilación.
 */
template <uint64_t base>
consteval uint64_t max_exponent_for_base() noexcept {
    using NumRepr::AuxFunc::LUT::base_maxexp_table;
    constexpr size_t last_idx {base_maxexp_table.size() - 1};
    return internal::find_max_exp_recursive<last_idx>(base);
}

/**
 * @brief Potencia entera genérica en tiempo de compilación.
 * @note Verifica overflow usando la tabla LUT en tiempo de compilación.
 */
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

/**
 * @brief Logaritmo entero genérico en tiempo de compilación.
 */
template <uint64_t base, int64_t n> 
constexpr int64_t int_log() noexcept {
  static_assert(base > 1, "Base invalida para logaritmo (debe ser > 1)");
  if constexpr (n <= 0) return -1;
  else if constexpr (n < static_cast<int64_t>(base)) return 0;
  else return 1 + int_log<base, n / base>();
}

} // namespace ct

// =============================================================================
// RUNTIME: FUNCIONES EJECUTABLES (Optimizadas y Seguras)
// =============================================================================

/**
 * @brief Logaritmo base 2 entero.
 */
constexpr uint64_t int_log2(uint64_t n) noexcept {
  if (n < 2u) return 0;
#if defined(__GNUC__) || defined(__clang__)
  // Builtin para contar ceros a la izquierda (clz) - muy rápido
  return 63 - __builtin_clzll(n);
#elif defined(_MSC_VER)
  // Intrinsic MSVC si estuviéramos en runtime puro, pero en constexpr usamos el bucle
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
 * @param base Base.
 * @param exponent Exponente.
 * @return Resultado de base^exponent, o 0 si ocurre overflow.
 */
constexpr inline uint64_t int_pow(uint64_t base, uint32_t exponent) noexcept {
  // 1. Check de seguridad rápido usando la tabla LUT
  // Esto reemplaza la "cadena de ifs" manual por una búsqueda en tabla.
  if (exponent > Safety::max_exponent_for_base(base)) { 
      return 0; // Overflow detectado por tabla
  }

  // 2. Casos triviales
  if (base == 0) return (exponent == 0) ? 1 : 0;
  if (base == 1) return 1;
  if (exponent == 0) return 1;
  if (exponent == 1) return base;

  // 3. Exponenciación binaria (Square and Multiply)
  uint64_t result = 1;
  uint64_t cur = base;

  while (exponent > 0) {
    if (exponent & 1) {
      // Check extra por si la tabla no fuese perfecta o para bases dinámicas
      if (__builtin_mul_overflow(result, cur, &result)) return 0;
    }
    exponent >>= 1;
    if (exponent > 0) {
      if (__builtin_mul_overflow(cur, cur, &cur)) {
          // Si cur desborda y aún queda exponente, el resultado final desbordará
          // a menos que result ya fuera final, pero el loop continúa.
          return 0; 
      }
    }
  }
  return result;
}

/**
 * @brief Logaritmo entero base N (floor).
 */
constexpr int64_t int_log(uint64_t base, int64_t n) noexcept {
  if (base <= 1) return 0; // Error de dominio base
  if (n <= 0) return -1;   // Error de dominio n
  if (static_cast<uint64_t>(n) < base) return 0;
  
  // Recursión de cola optimizable
  return 1 + int_log(base, n / base);
}

// =============================================================================
// UTILIDADES: CONTEO DE DÍGITOS
// =============================================================================

/**
 * @brief Helper: verifica si b^exp <= limit sin desbordar.
 */
constexpr inline bool pow_leq_limit(uint64_t b, uint32_t exp, uint64_t limit) noexcept {
  if (b == 0) return (exp == 0 ? 1 <= limit : 0 <= limit);
  if (b == 1 || exp == 0) return 1 <= limit;

  uint64_t result = 1;
  uint64_t cur = b;

  while (exp > 0) {
    if (exp & 1) {
      if (limit / cur < result) return false; // Check div evita overflow
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

/**
 * @brief Cuenta los dígitos necesarios para representar n en base 'base'.
 */
constexpr inline size_t count_digits_base(uint64_t n, uint64_t base) noexcept {
  if (base < 2) return 0;
  if (n == 0) return 1;
  if (base == 2) return static_cast<size_t>(int_log2(n)) + 1u;

  // Aproximación inicial usando potencias de 2 del exponente
  uint32_t lo = 0u, hi = 1u;
  while (pow_leq_limit(base, hi, n)) {
    lo = hi;
    hi *= 2u;
    if (hi == 0) break; // Safety break
  }

  // Búsqueda binaria para el exponente exacto
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