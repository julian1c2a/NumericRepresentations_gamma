#ifndef NUMREPR_INCLUDE_CORE_INTERNAL_BASIC_TYPES_HPP_INCLUDED
#define NUMREPR_INCLUDE_CORE_INTERNAL_BASIC_TYPES_HPP_INCLUDED

#include <cstdint>

#include "append/expected.hpp"
#include "append/int128_ops.hpp"
#include "append/integers.hpp"

#include <compare>
#include <concepts>
#include <cstring>
#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include <array>
#include <list>
#include <tuple>
#include <vector>

#include <climits>
#include <cmath>
#include <limits>
#include <numeric>

#include <functional>
#include <iterator>

/**
 * @file basic_types.hpp
 * @brief Tipos básicos y meta-funciones para el sistema de representaciones
 * numéricas
 */
namespace NumRepr {

/**
 * @brief Representación del signo de un número.
 * @details Enumeración actualmente utilizada en operaciones aritméticas y
 * comparaciones. Valores: vzero (0), vminus (-1), vplus (+1).
 */
enum class sign_funct_e : char { 
    zero = 0,   // Antes: vzero
    minus = -1, // Antes: vminus
    plus = +1   // Antes: vplus
};

/**
 * @brief Formatos de representación de dígitos.
 */
enum class dig_format_e : unsigned char {
  BINnat = 0,    ///< Binario natural
  BCDnat = 1,    ///< BCD natural (8421)
  BCDexc3 = 2,   ///< BCD exceso-3
  BCDaitk = 3,   ///< BCD Aiken (2421)
  johnson_n = 4, ///< Código Johnson
  biquinario = 5 ///< Código biquinario
};

/**
 * @brief Clasificación de tipos numéricos.
 */
enum class num_type_e : unsigned char {
  natural = 0,  ///< Naturales (ℕ) - IMPLEMENTADO
  entero = 1,   ///< Enteros (ℤ) - IMPLEMENTADO
  racional = 2, ///< Racionales (ℚ) - PENDIENTE
  real = 3      ///< Reales (ℝ) - PENDIENTE
};

enum class nat_num_format_e : bool {
  natural = false, ///< Representación natural estándar
  exc_n = true     ///< Representación en exceso-n (futuro)
};

enum class int_num_format_e : unsigned char {
  int_CB = 0,   ///< Complemento a la base - IMPLEMENTADO
  int_CBm1 = 1, ///< Complemento a base-1 - PENDIENTE
  int_MS = 2,   ///< Magnitud y signo - PENDIENTE
  int_EXC_n = 3 ///< Exceso-n - PENDIENTE
};

enum class rat_num_format_e : unsigned char {
  pair_num_den = 0, ///< Par (numerador, denominador)
  fxd_pt = 1,       ///< Punto fijo
  flt_pt = 2        ///< Punto flotante
};

enum class binop_e : unsigned char {
  add = 0,   ///< Adición
  sub = 1,   ///< Sustracción
  mult = 2,  ///< Multiplicación
  div = 3,   ///< División
  rem = 4,   ///< Resto/módulo
  fediv = 5, ///< Floor division (futuro)
  other = 6  ///< Otras operaciones (futuro)
};

constexpr inline int to_int(sign_funct_e sign) noexcept {
  return static_cast<int>(sign);
}

constexpr inline bool is_positive(sign_funct_e sign) noexcept {
  return sign == sign_funct_e::plus;
}

constexpr inline bool is_negative(sign_funct_e sign) noexcept {
  return sign == sign_funct_e::minus;
}

constexpr inline bool is_zero(sign_funct_e sign) noexcept {
  return sign == sign_funct_e::zero;
}

constexpr inline sign_funct_e opposite_sign(sign_funct_e sign) noexcept {
  return (sign == sign_funct_e::plus)    ? sign_funct_e::minus
         : (sign == sign_funct_e::minus) ? sign_funct_e::plus
                                          : sign_funct_e::zero;
}

// -----------------------------------------------------------------------------
// FIXED STRING (CNTTP)
// -----------------------------------------------------------------------------

/**
 * @brief Wrapper estructural para cadenas fijas en tiempo de compilación.
 * @details Permite pasar literales de cadena como parámetros de plantilla en C++20.
 */
template <size_t N>
struct fixed_string {
    static constexpr size_t size = N; 
    
    // FIX MSVC: Restaurada inicialización {} para evitar error C2131/C2737 (objeto no inicializado).
    // Combinado con el fix en atoull_ct (no usar string_view), esto satisface a MSVC.
    char data[N]{}; 

    constexpr fixed_string() = default;

    constexpr fixed_string(const char (&str)[N]) {
        for (size_t i = 0; i < N; ++i) {
            data[i] = str[i];
        }
    }

    consteval operator std::string_view() const {
        if (N > 0 && data[N-1] == '\0')
            return {data, N - 1};
        else
            return {data, N};
    }
};

// GUÍA DE DEDUCCIÓN
template <size_t N> fixed_string(const char (&)[N]) -> fixed_string<N>;

// -----------------------------------------------------------------------------
// TYPE TRAITS
// -----------------------------------------------------------------------------
namespace type_traits {

template <typename IntT>
concept arith_integral_c =
    std::is_arithmetic_v<IntT> && std::is_integral_v<IntT>;

template <typename NatIntT>
concept arith_natural_c =
    std::is_arithmetic_v<NatIntT> && std::is_unsigned_v<NatIntT> &&
    std::is_integral_v<NatIntT>;

template <typename CharT>
concept char_type_c =
    std::is_same_v<CharT, char> || std::is_same_v<CharT, signed char> ||
    std::is_same_v<CharT, unsigned char> || std::is_same_v<CharT, wchar_t>;

template <char_type_c CharT> constexpr inline CharT nullchar{CharT('\0')};

// -----------------------------------------------------------------------------
// ATOULL UTILITIES
// -----------------------------------------------------------------------------

inline constexpr ullint_t atoull(const char *text) noexcept {
  ullint_t i = 0;
  while (*text) {
    i = (i << 3) + (i << 1) + static_cast<ullint_t>(*text - '0');
    ++text;
  }
  return i;
}

inline constexpr ullint_t atoull(std::string_view sv) noexcept {
  ullint_t i = 0;
  for (char c : sv) {
    i = (i << 3) + (i << 1) + static_cast<ullint_t>(c - '0');
  }
  return i;
}

enum class atoull_err_t : int {
  empty_str, 
  no_digit,  
  overflow,  
  unknown    
};

inline std::expected<ullint_t, atoull_err_t>
atoull_checked(const char *text) noexcept {
  if (text == nullptr)
    return std::unexpected(atoull_err_t::empty_str);
  ullint_t i = 0;
  bool any = false;
  constexpr ullint_t maxv = std::numeric_limits<ullint_t>::max();
  while (*text) {
    char c = *text;
    if (c < '0' || c > '9')
      return std::unexpected(atoull_err_t::no_digit);
    unsigned digit = static_cast<unsigned>(c - '0');
    if (i > (maxv - digit) / 10)
      return std::unexpected(atoull_err_t::overflow);
    i = i * 10 + digit;
    any = true;
    ++text;
  }
  if (!any)
    return std::unexpected(atoull_err_t::empty_str);
  return std::expected<ullint_t, atoull_err_t>(i);
}

inline std::expected<ullint_t, atoull_err_t>
atoull_checked(std::string_view sv) noexcept {
  if (sv.data() == nullptr || sv.size() == 0)
    return std::unexpected(atoull_err_t::empty_str);
  ullint_t i = 0;
  bool any = false;
  constexpr ullint_t maxv = std::numeric_limits<ullint_t>::max();
  for (char c : sv) {
    if (c < '0' || c > '9')
      return std::unexpected(atoull_err_t::no_digit);
    unsigned digit = static_cast<unsigned>(c - '0');
    if (i > (maxv - digit) / 10)
      return std::unexpected(atoull_err_t::overflow);
    i = i * 10 + digit;
    any = true;
  }
  if (!any)
    return std::unexpected(atoull_err_t::empty_str);
  return std::expected<ullint_t, atoull_err_t>(i);
}

inline std::expected<std::pair<ullint_t, size_t>, atoull_err_t>
atoull_consume(const char *text) noexcept {
  if (text == nullptr)
    return std::unexpected(atoull_err_t::empty_str);
  ullint_t i = 0;
  size_t idx = 0;
  constexpr ullint_t maxv = std::numeric_limits<ullint_t>::max();
  while (text[idx]) {
    char c = text[idx];
    if (c < '0' || c > '9')
      break;
    unsigned digit = static_cast<unsigned>(c - '0');
    if (i > (maxv - digit) / 10)
      return std::unexpected(atoull_err_t::overflow);
    i = i * 10 + digit;
    ++idx;
  }
  if (idx == 0)
    return std::unexpected(atoull_err_t::no_digit);
  return std::expected<std::pair<ullint_t, size_t>, atoull_err_t>(
      std::pair<ullint_t, size_t>{i, idx});
}

inline std::expected<std::pair<ullint_t, size_t>, atoull_err_t>
atoull_consume(std::string_view sv) noexcept {
  if (sv.data() == nullptr || sv.size() == 0)
    return std::unexpected(atoull_err_t::empty_str);
  ullint_t i = 0;
  size_t idx = 0;
  constexpr ullint_t maxv = std::numeric_limits<ullint_t>::max();
  while (idx < sv.size()) {
    char c = sv[idx];
    if (c < '0' || c > '9')
      break;
    unsigned digit = static_cast<unsigned>(c - '0');
    if (i > (maxv - digit) / 10)
      return std::unexpected(atoull_err_t::overflow);
    i = i * 10 + digit;
    ++idx;
  }
  if (idx == 0)
    return std::unexpected(atoull_err_t::no_digit);
  return std::expected<std::pair<ullint_t, size_t>, atoull_err_t>(
      std::pair<ullint_t, size_t>{i, idx});
}

/**
 * @brief Convierte string literal a ullint_t en tiempo de compilación.
 * @details Versión optimizada para MSVC: Itera directamente sobre el array
 * crudo STR.data mediante índices explícitos.
 */
template <NumRepr::fixed_string STR> 
consteval ullint_t atoull_ct() {
  ullint_t i = 0;
  constexpr ullint_t maxv = std::numeric_limits<ullint_t>::max();
  bool any_digit = false;

  // FIX MSVC: Bucle for indexado tradicional sobre el array crudo.
  // Evita abstracciones de iteradores que pueden fallar en consteval MSVC.
  for (size_t k = 0; k < STR.size; ++k) {
    char c = STR.data[k];
    
    if (c == '\0') break; 

    if (c < '0' || c > '9') {
      throw "atoull_ct: non-digit character found";
    }

    unsigned digit = static_cast<unsigned>(c - '0');

    if (i > (maxv - digit) / 10) {
      throw "atoull_ct: overflow detected";
    }

    i = i * 10 + digit;
    any_digit = true;
  }

  if (!any_digit) {
    throw "atoull_ct: empty string";
  }

  return i;
}

//================================================
// TYPE TRAITS: Concepts and Type Deduction
//================================================

template <typename UINT_T>
constexpr bool is_uint_type_for_radix_v =
    std::is_unsigned_v<UINT_T> && (sizeof(UINT_T) < 8);

template <typename UINT_T>
concept uint_type_for_radix_c = is_uint_type_for_radix_v<UINT_T>;

template <typename UINT_T>
constexpr bool is_unsigned_type_v =
    is_uint_type_for_radix_v<UINT_T> || std::is_same_v<UINT_T, std::uint64_t> ||
    std::is_same_v<UINT_T, ullint_t> || std::is_same_v<UINT_T, ulint_t>;

template <typename UINT_T>
concept unsigned_integral_c = is_unsigned_type_v<UINT_T>;

template <typename SINT_T>
constexpr bool is_signed_type_v =
    std::is_signed_v<SINT_T> || std::is_same_v<SINT_T, sint64_t>;

template <typename SINT_T>
concept signed_integral_c = is_signed_type_v<SINT_T>;

template <typename INT_TYPE>
inline constexpr bool is_integral_type_v = std::is_integral_v<INT_TYPE>;

template <typename INT_T>
concept integral_c = is_integral_type_v<INT_T>;

//================================================
// SIZE COMPARISON UTILITIES
//================================================

template <typename T, typename S>
constexpr bool eq_sz_v = (sizeof(T) == sizeof(S));
template <typename T, typename S>
constexpr bool gt_sz_v = (sizeof(T) > sizeof(S));
template <typename T, typename S>
constexpr bool lt_sz_v = (sizeof(T) < sizeof(S));
template <typename T, typename S>
constexpr bool ge_sz_v = gt_sz_v<T, S> || eq_sz_v<T, S>;
template <typename T, typename S>
constexpr bool le_sz_v = lt_sz_v<T, S> || eq_sz_v<T, S>;

//================================================
// TYPE DEDUCTION: UInt → UInt Conversion
//================================================

namespace ugly_details_UInt_for_UInt {

template <unsigned_integral_c UInt_t> struct __nextsz_UInt_for_UInt_t {
  using type = std::conditional_t<
      sizeof(UInt_t) < 2, std::uint16_t, 
      std::conditional_t<
          sizeof(UInt_t) < 4, std::uint32_t, 
          std::conditional_t<sizeof(UInt_t) < 8, std::uint64_t, 
                             void 
                             >>>;

  static_assert(!std::is_same_v<type, void>,
                "No larger standard unsigned type exists for this type size. "
                "64-bit unsigned types have no standard larger type.");
};

template <> struct __nextsz_UInt_for_UInt_t<std::uint64_t> {
  using type = std::uint64_t; 
};

} // namespace ugly_details_UInt_for_UInt

template <typename UInt_t>
using nextsz_UInt_for_UInt_t =
    typename ugly_details_UInt_for_UInt::__nextsz_UInt_for_UInt_t<UInt_t>::type;

//================================================
// TYPE DEDUCTION: UInt → SInt Conversion
//================================================

namespace ugly_details_nextsz_SInt_for_UInt {
template <typename UInt> struct __nextsz_SInt_for_UInt_t {
  using type = std::conditional_t<
      sizeof(UInt) == 1, std::int16_t, 
      std::conditional_t<
          sizeof(UInt) == 2, std::int32_t, 
          std::conditional_t<
              sizeof(UInt) == 4, std::int64_t, 
              std::conditional_t<sizeof(UInt) == 8, sint64_t, 
                                 void 
                                 >>>>;

  static_assert(
      !std::is_same_v<type, void>,
      "No standard signed type mapping exists for this unsigned type size. "
      "Only 8, 16, 32, and 64-bit unsigned types are supported.");
};

} // namespace ugly_details_nextsz_SInt_for_UInt

template <typename UInt_t>
using nextsz_SInt_for_UInt_t =
    typename ugly_details_nextsz_SInt_for_UInt::__nextsz_SInt_for_UInt_t<
        UInt_t>::type;

//================================================
// TYPE DEDUCTION: SInt → UInt Conversion
//================================================

namespace ugly_details_UInt_for_SInt {
template <typename SInt> struct __nextsz_UInt_for_SInt_t {
  using type = std::conditional_t<
      sizeof(SInt) == 1, std::uint8_t,
      std::conditional_t<
          sizeof(SInt) == 2, std::uint16_t,
          std::conditional_t<
              sizeof(SInt) == 4, std::uint32_t,
              std::conditional_t<sizeof(SInt) == 8, std::uint64_t,
                                 void 
                                 >>>>;

  static_assert(
      !std::is_same_v<type, void>,
      "No standard unsigned type mapping exists for this signed type size. "
      "Only 8, 16, 32, and 64-bit signed types are supported.");
};

} // namespace ugly_details_UInt_for_SInt

template <typename Int_t>
using nextsz_UInt_for_SInt_t =
    typename ugly_details_UInt_for_SInt::__nextsz_UInt_for_SInt_t<Int_t>::type;

//================================================
// TYPE DEDUCTION: SInt → SInt Conversion
//================================================

namespace ugly_details_SInt_for_SInt {

template <typename SInt> struct __nextsz_SInt_for_SInt_t {
  using type = std::conditional_t<
      sizeof(SInt) < 2, std::int16_t, 
      std::conditional_t<
          sizeof(SInt) < 4, std::int32_t, 
          std::conditional_t<sizeof(SInt) < 8, std::int64_t, 
                             void 
                             >>>;

  static_assert(!std::is_same_v<type, void>,
                "No larger standard signed type exists for this type size. "
                "64-bit signed types have no standard larger type.");
};

template <> struct __nextsz_SInt_for_SInt_t<std::int64_t> {
  using type = std::int64_t; 
};

} // namespace ugly_details_SInt_for_SInt

template <typename SInt_t>
using nextsz_SInt_for_SInt_t =
    typename ugly_details_SInt_for_SInt::__nextsz_SInt_for_SInt_t<SInt_t>::type;

//================================================
// BASE AND DIGIT UTILITIES
//================================================

template <typename UINT_T> consteval UINT_T maxbase() {
  return (static_cast<UINT_T>(std::numeric_limits<UINT_T>::max()));
}
template <typename UINT_T> consteval UINT_T submaxbase() {
  return static_cast<UINT_T>(maxbase<UINT_T>() - 1);
}
template <typename UINT_T> consteval UINT_T minbase() {
  return static_cast<UINT_T>(2);
}
template <typename UINT_T> consteval UINT_T subminbase() {
  return static_cast<UINT_T>(3);
}
template <typename UINT_T> consteval UINT_T monobase() {
  return static_cast<UINT_T>(1);
}
template <typename UINT_T> consteval UINT_T nobase() {
  return static_cast<UINT_T>(0);
}
template <typename UINT_T> consteval UINT_T maxdigit() {
  return static_cast<UINT_T>(submaxbase<UINT_T>());
}
template <typename UINT_T> consteval UINT_T submaxdigit() {
  return static_cast<UINT_T>(submaxbase<UINT_T>() - 1);
}
template <typename UINT_T> consteval UINT_T digit_0() {
  return static_cast<UINT_T>(0u);
}
template <typename UINT_T> consteval UINT_T digit_1() {
  return static_cast<UINT_T>(1u);
}
template <typename UINT_T> consteval UINT_T base_2() {
  return static_cast<UINT_T>(2u);
}

template <typename UINT_T, UINT_T B> consteval bool base_geqt_2() {
  return (B >= base_2<UINT_T>());
}

template <typename UINT_T, UINT_T B> consteval bool base_leqt_max() {
  return (B <= maxbase<UINT_T>());
}

template <typename UINT_T, UINT_T B> consteval bool suitable_base() {
  return (base_geqt_2<UINT_T, B>() && base_leqt_max<UINT_T, B>() &&
          B <= static_cast<UINT_T>(std::numeric_limits<std::uint32_t>::max()));
}

template <typename UINT_T> consteval UINT_T middle_max() {
  using NEXTSZ_UINT_T = nextsz_UInt_for_UInt_t<UINT_T>;
  constexpr NEXTSZ_UINT_T maximo = maxbase<UINT_T>();
  constexpr NEXTSZ_UINT_T uno{1};
  constexpr NEXTSZ_UINT_T dos{2};
  return static_cast<UINT_T>((maximo + uno) / dos);
}

template <typename UINT_T> consteval UINT_T sqrt_max() {
  using NEXTSZ_UINT_T = nextsz_UInt_for_UInt_t<UINT_T>;
  constexpr NEXTSZ_UINT_T n =
      static_cast<NEXTSZ_UINT_T>(maxbase<UINT_T>()) + NEXTSZ_UINT_T{1};

  if constexpr (n == 0)
    return static_cast<UINT_T>(0);
  if constexpr (n == 1)
    return static_cast<UINT_T>(1);

  NEXTSZ_UINT_T x = n / 2;
  if (x == 0)
    x = 1;

  for (int i = 0; i < 50; ++i) {
    NEXTSZ_UINT_T x_new = (x + n / x) / 2;
    if (x_new >= x)
      break; 
    x = x_new;
  }

  return static_cast<UINT_T>(x);
}


namespace ugly_details_for_suitable_type_deduction {
template <typename T, T Radix> struct UIntTypeForRadix;
template <integral_c T, T Radix> struct UIntTypeForRadix<T, Radix> {
  using UIntType = std::conditional_t<
      static_cast<uint64_t>(Radix) <= maxbase<uint8_t>(), 
      uint8_t,
        std::conditional_t<
          static_cast<uint64_t>(Radix) <= maxbase<uint16_t>(), 
          uint16_t,
          std::conditional_t<static_cast<uint64_t>(Radix) <= maxbase<uint32_t>(), 
          uint32_t, 
          uint64_t>>>;
};
} 
using namespace ugly_details_for_suitable_type_deduction;

template <ullint_t Radix>
using TypeFromIntNumber_t = 
  typename UIntTypeForRadix<
    decltype(Radix), Radix
  >::UIntType;

namespace ugly_details_for_greater_suitable_type_deduction {

template <typename T, T Radix> struct UIntTypeForRadixContainsMultResult;
template <integral_c T, T Radix>
  requires(Radix > 1)
struct UIntTypeForRadixContainsMultResult<T, Radix> {
  static constexpr ullint_t uint_value_0_max =
      static_cast<ullint_t>(sqrt_max<uint8_t>());
  static constexpr ullint_t uint_value_1_max =
      static_cast<ullint_t>(sqrt_max<uint16_t>());
  static constexpr ullint_t uint_value_2_max =
      static_cast<ullint_t>(sqrt_max<uint32_t>());
  static constexpr ullint_t uint_value_3_max =
      static_cast<ullint_t>(sqrt_max<uint64_t>());
  static constexpr ullint_t uint_value = static_cast<ullint_t>(Radix);

  template <typename TRAD_T>
  using fixed_size_type = std::conditional_t<
      sizeof(TRAD_T) == 1, uint8_t,
      std::conditional_t<
          sizeof(TRAD_T) == 2, uint16_t,
          std::conditional_t<sizeof(TRAD_T) == 4, uint32_t, uint64_t>>>;

  using UIntType = std::conditional_t<
      uint_value <= uint_value_0_max, uint8_t,
      std::conditional_t<uint_value <= uint_value_1_max, uint16_t,
                         std::conditional_t<uint_value <= uint_value_2_max,
                                            uint32_t, uint64_t>>>;
};
} // namespace ugly_details_for_greater_suitable_type_deduction
using namespace ugly_details_for_greater_suitable_type_deduction;

} // namespace type_traits

template <type_traits::unsigned_integral_c UINT_T>
using uintspair = typename std::array<UINT_T, 2>;

template <type_traits::unsigned_integral_c UINT_T, UINT_T B>
using uintspairlist = typename std::array<uintspair<UINT_T>, B>;

template <type_traits::unsigned_integral_c UINT_T, UINT_T B>
using uintspairtbl = typename std::array<uintspairlist<UINT_T, B>, B>;

namespace ugly_pack2tuple_details { 
}

namespace ugly_details {
struct local_void_t {};

template <class Head_t, class... Tail_t> struct for_each_same_type {
  using second_t = typename std::tuple_element<0, std::tuple<Tail_t...>>::type;
  constexpr static bool are_same_type_v =
      ((std::is_same_v<Head_t, second_t>) &&
       (for_each_same_type<Tail_t...>::are_same_type_v));
};

template <class Head_t> struct for_each_same_type<Head_t> {
  constexpr static bool are_same_type_v = true;
};
} // namespace ugly_details

template <typename... Ts>
concept all_are_the_same_type_c = requires(Ts...) {
  ugly_details::for_each_same_type<Ts...>::are_same_type_v;
};

template <typename... Ts>
concept there_is_one_or_more_c = requires(Ts...) { ((sizeof...(Ts)) > 0); };

namespace ugly_pack_details {

template <typename... Ts>
  requires(all_are_the_same_type_c<Ts...> && there_is_one_or_more_c<Ts...>)
struct pack2array {

  static constexpr std::size_t size = sizeof...(Ts);
  using inner_type = std::remove_cvref_t<std::common_type_t<Ts...>>;
  using array_type = std::array<inner_type, size>;
  using elem_type = inner_type;

  template <typename... Us>
  constexpr array_type operator()(Us &&...args) const noexcept {
    return array_type{static_cast<inner_type>(std::forward<Us>(args))...};
  } 

  template <std::size_t J> static constexpr elem_type get(Ts... args) noexcept {
    array_type content{static_cast<inner_type>(args)...};
    return content[J];
  } 

  template <std::size_t... I>
  static constexpr void for_each_impl(array_type &iarray, const Ts... args,
                                      std::index_sequence<I...>) noexcept {
    ((iarray[I] = static_cast<inner_type>(args)), ...);
  } 

  static constexpr void for_each(array_type &iarray,
                                 const Ts... args) noexcept {
    for_each_impl(iarray, args..., std::make_index_sequence<sizeof...(args)>{});
  } 
};
} // namespace ugly_pack_details

template <class... Ts>
  requires(all_are_the_same_type_c<Ts...> && there_is_one_or_more_c<Ts...>)
void assign_with_order(auto &dest, const Ts &...args) noexcept {
  using type = typename ugly_pack_details::pack2array<Ts...>;
  type::for_each(dest, args...);
  return;
} 

} // namespace NumRepr

#endif // NUMREPR_INCLUDE_CORE_INTERNAL_BASIC_TYPES_HPP_INCLUDED