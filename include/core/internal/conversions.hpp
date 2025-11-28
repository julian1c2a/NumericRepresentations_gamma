#ifndef NUMREPR_INCLUDE_CORE_INTERNAL_CONVERSIONS_HPP_INCLUDED
#define NUMREPR_INCLUDE_CORE_INTERNAL_CONVERSIONS_HPP_INCLUDED

#include "append/expected.hpp"
#include "append/integers.hpp"

// CORRECCIÓN: Nombres de archivo correctos
#include "math/Int_ExpLog.hpp"
#include "math/IntRoot.hpp"

#include <limits>
#include <type_traits>

namespace NumRepr {
namespace AuxFunc {

using std::numeric_limits;

// =============================================================================
// CONVERSIONES DE REPRESENTACIÓN (DIGITS -> INT)
// =============================================================================

enum class ConversionError { InvalidDigit, Overflow, InvalidBase };

/**
 * @brief Conversión segura de array de dígitos a entero.
 */
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

  // CORRECCIÓN: Usar un tipo con signo para el índice del bucle para evitar overflow al comparar >= 0
  for (int64_t ix{static_cast<int64_t>(L) - 2}; ix >= 0; --ix) {
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

/**
 * @brief Versión compile-time sin checks (rápida).
 */
template <auto B, auto L, typename A>
consteval inline uint64_t conversion_to_int_ct(const A &arg) noexcept {
  constexpr uint64_t base{static_cast<uint64_t>(B)};
  uint64_t acc{arg[L - 1]()};
  for (int64_t ix{static_cast<int64_t>(L) - 2}; ix >= 0; --ix) {
    acc *= base;
    acc += static_cast<uint64_t>(arg[ix]());
  }
  return acc;
}

} // namespace AuxFunc
} // namespace NumRepr

#endif // NUMREPR_INCLUDE_CORE_INTERNAL_CONVERSIONS_HPP_INCLUDED