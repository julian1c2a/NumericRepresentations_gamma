#ifndef NUMREPR_INCLUDE_CORE_INTERNAL_APPEND_INTEGERS_HPP_INCLUDED
#define NUMREPR_INCLUDE_CORE_INTERNAL_APPEND_INTEGERS_HPP_INCLUDED

/**
 * @file integers.hpp
 * @brief Definiciones de tipos enteros y alias fundamentales para el sistema.
 * @details Centraliza todos los typedefs de enteros (signed/unsigned), tipos
 * de tamaño fijo y utilidades básicas como std::array.
 */
#include "int128_ops.hpp"

#include <cstddef> // Para std::size_t
#include <cstdint> // Para tipos intN_t
#include <array>   // Para std::array

namespace NumRepr {

  // --- Tipos Fundamentales "Legacy" ---
  using chint_t = char;
  using schint_t = signed char;
  using ssint_t = signed short int;
  using sint_t = signed int;
  using slint_t = signed long int;
  using sllint_t = signed long long int;

  using uchint_t = unsigned char;
  using usint_t = unsigned short int;
  using uint_t = unsigned int;
  using ulint_t = unsigned long int;
  using ullint_t = unsigned long long int;

  // --- Tipos de Tamaño Fijo (Stdint) ---
  using size_t = std::size_t;

  // Signed Fixed Width
  using sint8_t = std::int8_t;
  using sint16_t = std::int16_t;
  using sint32_t = std::int32_t;
  using sint64_t = std::int64_t;
  using int8_t = std::int8_t;
  using int16_t = std::int16_t;
  using int32_t = std::int32_t;
  using int64_t = std::int64_t;

  // Fast Types
  using fast8_t = std::int_fast8_t;
  using fast16_t = std::int_fast16_t;
  using fast32_t = std::int_fast32_t;
  using fast64_t = std::int_fast64_t;

  // Least Types
  using least8_t = std::int_least8_t;
  using least16_t = std::int_least16_t;
  using least32_t = std::int_least32_t;
  using least64_t = std::int_least64_t;

  // Unsigned Fixed Width
  using uint8_t = std::uint8_t;
  using uint16_t = std::uint16_t;
  using uint32_t = std::uint32_t;
  using uint64_t = std::uint64_t;

  // Unsigned Fast Types
  using ufast8_t = std::uint_fast8_t;
  using ufast16_t = std::uint_fast16_t;
  using ufast32_t = std::uint_fast32_t;
  using ufast64_t = std::uint_fast64_t;

  // Unsigned Least Types
  using uleast8_t = std::uint_least8_t;
  using uleast16_t = std::uint_least16_t;
  using uleast32_t = std::uint_least32_t;
  using uleast64_t = std::uint_least64_t;

  // Max Types
  using intmax_t = std::intmax_t;
  using uintmax_t = std::uintmax_t;

  // --- Utilidades del Espacio de Nombres ---
  using std::array; // Disponible como NumRepr::array

} // namespace NumRepr

#endif // NUMREPR_INCLUDE_CORE_INTERNAL_APPEND_INTEGERS_HPP_INCLUDED
