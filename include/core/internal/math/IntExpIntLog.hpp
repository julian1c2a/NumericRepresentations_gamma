#ifndef NUMREPR_INCLUDE_CORE_INTERNAL_MATH_INTEXPINTLOG_HPP_INCLUDED
#define NUMREPR_INCLUDE_CORE_INTERNAL_MATH_INTEXPINTLOG_HPP_INCLUDED

#include "../append/expected.hpp"
#include "../append/integers.hpp"

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
// =============================================================================
// 0. SEGURIDAD: CHECKERS DE EXPONENTE MÁXIMO (Basado en Tabla)
// =============================================================================

namespace Safety {

/**
 * @brief Obtiene el exponente máximo seguro para una base dada (uint64_t).
 * @details Busca en la tabla LUT el rango correspondiente a la base.
 * La tabla contiene pares {base_inicio, max_exp}.
 * Si base >= base_inicio[i], entonces max_exp es max_exp[i].
 * Se recorre la tabla de mayor a menor para encontrar el rango.
 */
constexpr inline uint64_t max_exponent_for_base(uint64_t base) noexcept {
    // Accedemos a la tabla en el namespace correcto
    const auto& table = NumRepr::AuxFunc::LUT::base_maxexp_table;

    // Búsqueda lineal inversa: Eficiente para tablas pequeñas (~30 elementos)
    // y garantiza constexpr simple.
    for (size_t i = table.size(); i > 0; --i) {
        const auto& entry = table[i - 1];
        if (base >= entry.first) {
            return entry.second;
        }
    }
    // Fallback seguro (técnicamente inalcanzable si la tabla empieza en 0)
    return ~0ull; 
}

/**
 * @brief Versión Compile-Time de la seguridad.
 * @details Evalúa la función constexpr en tiempo de compilación.
 */
// --- Implementación Recursiva para Compile-Time (_ct) ---

namespace internal {
    // Búsqueda recursiva en tiempo de compilación sobre la tabla estática.
    // I: Índice actual a comprobar (empezando por el final).
    template <size_t I>
    consteval uint64_t find_max_exp_recursive_ct(uint64_t base) {
        // Referencia a la tabla constexpr
        constexpr auto& table = NumRepr::AuxFunc::LUT::base_maxexp_table;
        
        // Caso base implícito por recursión: comprobamos el elemento actual
        if (base >= table[I].first) {
            return table[I].second;
        }
        
        // Paso recursivo: si no hemos encontrado, bajamos al índice anterior
        if constexpr (I > 0) {
          return find_max_exp_recursive_ct<I - 1>(base);
        } else {
        // Fallback por seguridad (tabla[0] suele ser 0, 
        // así que base >= 0 siempre cumple antes)
          return ~0ull; 
        }
    }
} // namespace internal

/**
 * @brief Versión Compile-Time de la seguridad (Totalmente Recursiva).
 * @details Garantiza que no se genere código de bucles 
 * en tiempo de compilación, desenrrollando la búsqueda
 * mediante plantillas.
 */
template <uint64_t base>
consteval uint64_t max_exponent_for_base_ct() noexcept {
    using NumRepr::AuxFunc::LUT::base_maxexp_table;
    // Iniciamos la búsqueda desde el último elemento de la tabla
    constexpr size_t last_idx {base_maxexp_table.size() - 1};
    return internal::find_max_exp_recursive_ct<last_idx>(base);
}

} // close namespace Safety

} // close namespace LUT

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
  using LUT::Safety::max_exponent_for_base;
  // Usamos la utilidad LUT local
  if (exponent > max_exponent_for_base(base)) { return 0; }

  if (base == 0) { return ((exponent == 0) ? 1 : 0); }
  if (base == 1) { return 1; }
  if (exponent == 0) { return 1; }
  if (exponent == 1) { return base; }

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
  requires(exponent <= LUT::Safety::max_exponent_for_base_ct<base>())
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

#endif // NUMREPR_INCLUDE_CORE_INTERNAL_MATH_INTEXPINTLOG_HPP_INCLUDED
