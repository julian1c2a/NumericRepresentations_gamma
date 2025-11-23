#ifndef NUMREPR_INCLUDE_CORE_INTERNAL_APPEND_EXPECTED_HPP_INCLUDED
#define NUMREPR_INCLUDE_CORE_INTERNAL_APPEND_EXPECTED_HPP_INCLUDED

/**
 * @file expected.hpp
 * @brief Abstracción para std::expected (C++23).
 * @details Este archivo gestiona la inclusión de <expected>.
 * - Si <expected> está disponible (C++23), importa std::expected y
 * std::unexpected al namespace NumRepr.
 * - Si no está disponible, define una implementación propia (polyfill) dentro
 * de NumRepr.
 * * De esta forma, el resto del código solo debe hacer #include
 * "append/expected.hpp" y usar NumRepr::expected.
 */

#if __has_include(<expected>)

#include <expected>

namespace NumRepr {
// Importamos la implementación estándar al namespace del proyecto
using std::expected;
using std::unexpected;
} // namespace NumRepr

#else

#include <exception> // Para std::bad_variant_access (usualmente en <variant>, pero por seguridad)
#include <utility>
#include <variant>

namespace NumRepr {

/**
 * @brief Envoltorio para indicar que un valor representa un error.
 * @details Necesario para desambiguar tipos cuando T y E son convertibles entre
 * sí.
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
 * @tparam T Tipo del valor esperado.
 * @tparam E Tipo del error.
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

#endif // __has_include(<expected>)

#endif // NUMREPR_INCLUDE_CORE_INTERNAL_APPEND_EXPECTED_HPP_INCLUDED
