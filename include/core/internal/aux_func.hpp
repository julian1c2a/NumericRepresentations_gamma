#ifndef NUMREPR_CORE_INTERNAL_AUXILIARY_FUNCTIONS_HPP_INCLUDED
#define NUMREPR_CORE_INTERNAL_AUXILIARY_FUNCTIONS_HPP_INCLUDED

#include "append/expected.hpp"
#include "append/integers.hpp"

#include <limits>
#include <type_traits>
// ELIMINADO: #include <expected> (usamos el wrapper de append)

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

// --- Logaritmos y Potencias ---

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
  return 1ull << exponent;
}

template <uint64_t num> constexpr inline bool is_power_of_2_ct() noexcept {
  return num > 0 && (num & (num - 1)) == 0;
}

// --- Runtime functions ---
constexpr uint64_t int_log2(uint64_t n) noexcept {
  if (n < 2u)
    return 0;
  uint64_t res = 0;
  while (n >>= 1)
    ++res;
  return res;
}

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
  return n == 0u ? 0 : int_log2_sf(n).value() + 1;
}

template <typename T> constexpr inline bool is_power_of_2(T num) noexcept {
  return num > 0 && (num & (num - 1)) == 0;
}

// --- Raíces (Newton-Raphson) ---

template <uint64_t n, uint64_t x0>
constexpr uint64_t floorsqrt_ct_newton() noexcept {
  return (x0 == 0) ? 0
                   : (((x0 + n / x0) / 2) >= x0
                          ? x0
                          : floorsqrt_ct_newton<n, (x0 + n / x0) / 2>());
}

namespace detail {
template <uint64_t n> struct floorsqrt_ct_helper_big {
  static constexpr uint64_t log2_n = (n == 0) ? 0 : int_log2_ct<n>();
  static constexpr uint64_t x0_base = (1ull << (log2_n / 2));
  static constexpr uint64_t x0 =
      (x0_base * x0_base < n) ? (x0_base * 2) : x0_base;
  static constexpr uint64_t value = floorsqrt_ct_newton<n, x0>();
};
// (Aquí iría tu tabla helper completa floorsqrt_ct_helper para optimización, la
// mantengo implícita)
} // namespace detail

// Función principal floorsqrt_ct simplificada para el ejemplo (usando
// helper_big por defecto)
template <uint64_t n> constexpr uint64_t floorsqrt_ct() noexcept {
  if constexpr (n < 2)
    return n;
  return detail::floorsqrt_ct_helper_big<n>::value;
}

template <typename T> constexpr T floorsqrt(T n) noexcept {
  if (n == 0)
    return 0;
  if (n == 1)
    return 1;

  using UnsignedT = std::make_unsigned_t<T>;
  T x0 = T(1) << (bit_width(static_cast<UnsignedT>(n)) / 2);
  if (x0 * x0 < n)
    x0 = x0 * 2;

  T x1 = (x0 + n / x0) / 2;
  while (x1 < x0) {
    x0 = x1;
    x1 = (x0 + n / x0) / 2;
  }
  return x0;
}

template <uint64_t n> consteval uint64_t ceilsqrt_ct() noexcept {
  if constexpr (n == 0)
    return 0;
  constexpr uint64_t root = floorsqrt_ct<n>();
  return (root * root == n) ? root : root + 1;
}

template <typename T> constexpr T ceilsqrt(T n) noexcept {
  if (n == 0)
    return 0;
  T root = floorsqrt(n);
  return (root * root == n) ? root : root + 1;
}

template <typename T> constexpr bool is_perfect_square(T n) noexcept {
  if ((n == 0) || (n == 1))
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

// --- Conteo de dígitos y Conversiones ---

constexpr inline bool pow_leq_limit(uint64_t b, uint32_t exp,
                                    uint64_t limit) noexcept {
  // (Tu implementación segura de pow_leq_limit)
  // ... [Mismo código que tenías] ...
  if (b == 0)
    return (exp == 0 ? 1 <= limit : 0 <= limit);
  if (b == 1 || exp == 0)
    return 1 <= limit;
  uint64_t result{1ull};
  uint64_t cur{b};
  uint32_t e{exp};
  while (e > 0) {
    if (e & 1u) {
      if (result > limit / cur)
        return false;
      result *= cur;
    }
    e >>= 1u;
    if (e > 0) {
      if (cur > numeric_limits<uint64_t>::max() / cur)
        return false;
      cur *= cur;
    }
  }
  return result <= limit;
}

constexpr inline size_t count_digits_base(uint64_t n, uint64_t base) noexcept {
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

enum class ConversionError { InvalidDigit, Overflow, InvalidBase };

template <auto B, auto L, typename A>
constexpr inline expected<uint64_t, ConversionError>
conversion_to_int_safe(const A &arg) noexcept {
  constexpr uint64_t base{static_cast<uint64_t>(B)};
  if constexpr (base < 2)
    return unexpected(ConversionError::InvalidBase);

  uint64_t digit_val = static_cast<uint64_t>(arg[L - 1]());
  if (digit_val >= base)
    return unexpected(ConversionError::InvalidDigit);

  uint64_t acc{digit_val};
  constexpr uint64_t max_before_mult = numeric_limits<uint64_t>::max() / base;

  for (sint64_t ix{L - 2}; ix > -1; --ix) {
    if (acc > max_before_mult)
      return unexpected(ConversionError::Overflow);
    digit_val = static_cast<uint64_t>(arg[ix]());
    if (digit_val >= base)
      return unexpected(ConversionError::InvalidDigit);
    acc *= base;
    if (acc > numeric_limits<uint64_t>::max() - digit_val)
      return unexpected(ConversionError::Overflow);
    acc += digit_val;
  }
  return acc;
}

// NOTA: Eliminamos is_prime_ct y find_factor_ct de aquí.
// Se deben usar desde "primes_compiletime.hpp" que es más robusto.

} // namespace AuxFunc
} // namespace NumRepr

#endif // NUMREPR_CORE_INTERNAL_AUXILIARY_FUNCTIONS_HPP_INCLUDED
