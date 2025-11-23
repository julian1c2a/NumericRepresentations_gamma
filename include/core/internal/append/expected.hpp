#ifndef NUMREPR_INCLUDE_CORE_INTERNAL_APPEND_EXPECTED_HPP_INCLUDED
#define NUMREPR_INCLUDE_CORE_INTERNAL_APPEND_EXPECTED_HPP_INCLUDED

/**
 * @file expected.hpp
 * @brief Abstracción para std::expected (C++23).
 * @details Este archivo gestiona la inclusión de <expected> de forma robusta.
 * Verifica no solo la existencia del archivo, sino el soporte real de la feature
 * (__cpp_lib_expected).
 */

// Intentamos incluir <version> para tener acceso a las macros de features (C++20 standard)
#if __has_include(<version>)
#include <version>
#endif

// Verificamos si la librería realmente soporta expected (C++23 feature)
// Solo confiar en __has_include(<expected>) falla en modo C++20 con compiladores nuevos
#if defined(__cpp_lib_expected)

#include <expected>

namespace NumRepr {
// Importamos la implementación estándar al namespace del proyecto
using std::expected;
using std::unexpected;
} // namespace NumRepr

#else

// --- POLYFILL PARA C++20 O ANTERIOR ---

#include <exception> 
#include <utility>
#include <variant>

namespace NumRepr {

/**
 * @brief Envoltorio para indicar que un valor representa un error.
 */
template <typename E> struct unexpected {
  E value_;
  constexpr explicit unexpected(E v) : value_(std::move(v)) {}
  constexpr const E &error() const & noexcept { return value_; }
  constexpr E &error() & noexcept { return value_; }
  constexpr E &&error() && noexcept { return std::move(value_); }
};

// Guía de deducción (CTAD) para unexpected
template <typename E> unexpected(E) -> unexpected<E>;

/**
 * @brief Clase expected (polyfill parcial de C++23).
 */
template <typename T, typename E> class expected {
  std::variant<T, E> v_;

public:
  // Constructores de valor (éxito)
  constexpr expected(const T &t) : v_(t) {}
  constexpr expected(T &&t) : v_(std::move(t)) {}

  // Constructor de error (usando el envoltorio unexpected)
  template <typename G>
  constexpr expected(const NumRepr::unexpected<G> &ue) : v_(ue.value_) {}

  template <typename G>
  constexpr expected(NumRepr::unexpected<G> &&ue) : v_(std::move(ue.value_)) {}

  // Observadores
  constexpr bool has_value() const noexcept {
    return std::holds_alternative<T>(v_);
  }
  constexpr explicit operator bool() const noexcept { return has_value(); }

  constexpr T &value() & {
    // std::get lanza bad_variant_access si no contiene T
    return std::get<T>(v_);
  }
  constexpr const T &value() const & { return std::get<T>(v_); }

  constexpr E error() const { return std::get<E>(v_); }
};

} // namespace NumRepr

#endif // defined(__cpp_lib_expected)

#endif // NUMREPR_INCLUDE_CORE_INTERNAL_APPEND_EXPECTED_HPP_INCLUDED