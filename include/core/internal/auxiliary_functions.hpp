#ifndef NUMREPR_CORE_INTERNAL_AUXILIARY_FUNCTIONS_HPP_INCLUDED
#define NUMREPR_CORE_INTERNAL_AUXILIARY_FUNCTIONS_HPP_INCLUDED

#include "append/expected.hpp"
#include "append/integers.hpp"

#include <cstddef>
#include <cstdint>
#include <expected>
#include <limits>
#include <type_traits>

// --- Declaración adelantada de int_log2_ct y bit_width_ct ---
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

template <uint64_t n> constexpr uint64_t int_log2_ct() noexcept {
  if constexpr (n < 2u) {
    // ¡ADVERTENCIA! int_log2_ct<0>() retorna 0 por compatibilidad, pero
    // matemáticamente está indefinido.
    return 0;
  } else {
    return 1 + int_log2_ct<(n >> 1)>();
  }
} // END FUNCTION int_log2_ct compiletime

template <uint64_t n> constexpr uint64_t int_log2_sf_ct() noexcept {
  static_assert(n > 0, "el logaritmo `log_b(x)` no está definido para `x = 0`");
  if constexpr (n < 2u) {
    return 0;
  } else {
    return 1 + int_log2_sf_ct<(n >> 1)>();
  }
} // END FUNCTION int_log2_sf_ct safe compiletime

template <uint64_t n> constexpr uint64_t bit_width_ct() noexcept {
  return (n < 2u) ? 1 : int_log2_sf_ct<n>() + 1;
} // END FUNCTION bit_width_ct compiletime

template <size_t exponent> constexpr uint64_t int_pow2_sf_ct() noexcept {
  static_assert(exponent <= 63, "exponent fuera de rango para int_pow2_ct");
  return 1ull << exponent;
} // END FUNCTION int_pow2_sf_ct compiletime

template <size_t exponent> constexpr uint64_t int_pow2_ct() noexcept {
  return 1ull << exponent;
} // END FUNCTION int_pow2_ct compiletime

template <uint64_t num> constexpr inline bool is_power_of_2_ct() noexcept {
  return num > 0 && (num & (num - 1)) == 0;
} // END FUNCTION is_power_of_2_ct compiletime

constexpr uint64_t int_log2(uint64_t n) noexcept {
  if (n < 2u) {
    return 0;
  }
  uint64_t res = 0;
  while (n >>= 1)
    ++res;
  return res;
} // END FUNCTION int_log2 runtime

constexpr expected<uint64_t, math_error_ec> int_log2_sf(uint64_t n) noexcept {
  if (n == 0) {
    return unexpected(math_error_ec::baddomain);
  }
  if (n == 1) {
    return (0ull);
  }
  uint64_t res = 0;
  while (n >>= 1)
    ++res;
  return (res);
} // END FUNCTION int_log2_sf runtime

constexpr uint64_t bit_width(uint64_t n) noexcept {
  return n == 0u ? 0 : int_log2_sf(n) + 1;
} // END FUNCTION bit_width runtime

constexpr uint64_t int_pow2(size_t exponent) noexcept {
  // debe ser exponent <= 63, no se hace comprobación;
  return 1ull << exponent;
} // END FUNCTION int_pow2 runtime

constexpr expected<uint64_t, math_error_ec> int_pow2(size_t exponent) noexcept {
  if (exponent < 64)
    return (1ull << exponent);
  else
    return unexpected(math_error_ec::overflow);
} // END FUNCTION int_pow2 runtime

template <typename T> constexpr inline bool is_power_of_2(T num) noexcept {
  return num > 0 && (num & (num - 1)) == 0;
} // END FUNCTION is_power_of_2 runtime

/**
 * @brief Método genérico de Newton-Raphson en tiempo de compilación.
 * @details Implementación recursiva del método de Newton-Raphson que converge
 * cuando x_{k+1} >= x_k, indicando que se ha alcanzado el mínimo entero.
 *
 * @tparam n Parámetro fijo (típicamente el número a aproximar).
 * @tparam x0 Estimación actual.
 * @tparam IterExpr Función template que calcula x_{k+1} = IterExpr<x_k,
 * n>::value.
 *
 * @return El valor convergido de la iteración.
 *
 * @note IterExpr debe ser un template struct/class con un miembro estático
 * value:
 * @code
 *   template <uint64_t xk, uint64_t n>
 *   struct IterExpr {
 *       static constexpr uint64_t value = ...;
 *   };
 * @endcode
 *
 * @par Ejemplo completo - Raíz cuadrada:
 * @code
 *   // Definir expresión de iteración: x1 = (x0 + n/x0) / 2
 *   template <uint64_t x0, uint64_t n>
 *   struct sqrt_iteration {
 *       static constexpr uint64_t value = (x0 + n / x0) / 2;
 *   };
 *
 *   // Usar el método genérico
 *   constexpr auto result = newton_raphson_ct<100, 16, sqrt_iteration>();
 *   // result == 10
 * @endcode
 *
 * @par Ejemplo - Raíz cúbica (aproximación):
 * @code
 *   // Expresión: x1 = (2*x0 + n/(x0^2)) / 3
 *   template <uint64_t x0, uint64_t n>
 *   struct cbrt_iteration {
 *       static constexpr uint64_t value =
 *           (x0 == 0) ? 1 : (2 * x0 + n / (x0 * x0)) / 3;
 *   };
 *
 *   constexpr auto result = newton_raphson_ct<27, 4, cbrt_iteration>();
 *   // result aproximadamente 3
 * @endcode
 */
template <uint64_t n, uint64_t x0, template <uint64_t, uint64_t> class IterExpr>
constexpr uint64_t newton_raphson_ct() noexcept {
  constexpr uint64_t x1 = IterExpr<x0, n>::value;
  return (x1 >= x0) ? x0 : newton_raphson_ct<n, x1, IterExpr>();
} // END FUNCTION newton_raphson_ct compiletime (FUNCTOR TEMPLATE)

/**
 * @brief Función auxiliar recursiva para el método de Newton-Raphson en tiempo
 * de compilación.
 * @details Calcula iterativamente una aproximación de la raíz cuadrada entera
 * de `n` comenzando desde la estimación inicial `x0` usando la fórmula: x_{k+1}
 * = (x_k + n/x_k) / 2
 * @tparam n El número cuya raíz cuadrada se calcula.
 * @tparam x0 La estimación actual de la raíz cuadrada.
 * @return La raíz cuadrada entera por defecto de `n`.
 */
template <uint64_t n, uint64_t x0>
constexpr uint64_t floorsqrt_ct_newton() noexcept {
  // Protección: si x0 == 0, la raíz cuadrada de 0 es 0
  return (x0 == 0) ? 0
                   : (((x0 + n / x0) / 2) >= x0
                          ? x0
                          : floorsqrt_ct_newton<n, (x0 + n / x0) / 2>());
} // END FUNCTION floorsqrt_ct_newton compiletime

/**
 * @brief Calcula la raíz cuadrada entera por defecto de un número entero no
 * negativo.
 * @details Esta función calcula la parte entera de la raíz cuadrada de un
 * número no negativo `n` dado. Encuentra el entero más grande `r` tal que `r*r
 * <= n`. Para mayor eficiencia, utiliza una combinación de una tabla de
 * búsqueda para valores pequeños y el método de Newton-Raphson para valores más
 * grandes.
 *
 * @tparam n El número entero no negativo cuya raíz cuadrada se va a calcular.
 * @return La raíz cuadrada entera por defecto de `n`.
 *
 * @pre `n` debe ser no negativo.
 * @post El valor devuelto `r` satisface `r*r <= n` y `(r+1)*(r+1) > n`.
 *
 * @par Properties
 * - Para `n = 0`, el resultado es `0`.
 * - Para `n = 1`, el resultado es `1`.
 * - La función es monotónicamente creciente: si `a > b`, entonces `floorsqrt(a)
 * >= floorsqrt(b)`.
 * - `floorsqrt(n*n) == n` para cualquier `n` no negativo.
 *
 * @par Invariants
 * - Para la iteración de Newton-Raphson, la secuencia de estimaciones `x_k`
 * converge monotónicamente desde arriba hacia la verdadera raíz cuadrada.
 *
 * @par Things that should never happen
 * - La función no debe entrar en un bucle infinito.
 * - La función no debe devolver un valor `r` tal que `r*r > n`.
 * - La función no debe desbordarse durante los cálculos intermedios si `n` está
 * dentro del rango del tipo `T`.
 */
namespace detail {

template <uint64_t n> struct floorsqrt_ct_helper_big {
  static constexpr uint64_t log2_n = int_log2_ct<n>();
  static constexpr uint64_t x0_base = (1ull << (log2_n / 2));
  static constexpr uint64_t x0 =
      (x0_base * x0_base < n) ? (x0_base * 2) : x0_base;
  static constexpr uint64_t value = floorsqrt_ct_newton<n, x0>();
};

template <uint64_t n> struct floorsqrt_ct_helper {
  static constexpr uint64_t value =
      (n == 0)                ? 0
      : (n >= 1 && n < 4)     ? 1
      : (n >= 4 && n < 9)     ? 2
      : (n >= 9 && n < 16)    ? 3
      : (n >= 16 && n < 25)   ? 4
      : (n >= 25 && n < 36)   ? 5
      : (n >= 36 && n < 49)   ? 6
      : (n >= 49 && n < 64)   ? 7
      : (n >= 64 && n < 81)   ? 8
      : (n >= 81 && n < 100)  ? 9
      : (n >= 100 && n < 121) ? 10
      : (n >= 121 && n < 144) ? 11
      : (n >= 144 && n < 169) ? 12
      : (n >= 169 && n < 196) ? 13
      : (n >= 196 && n < 225) ? 14
      : (n >= 225 && n < 256) ? 15
      : (n >= 256 && n < 289) ? 16
      : (n >= 289 && n < 324) ? 17
      : (n >= 324 && n < 361) ? 18
      : (n >= 361 && n < 400) ? 19
      : (n >= 400 && n < 441) ? 20
      : (n >= 441 && n < 484) ? 21
      : (n >= 484 && n < 529) ? 22
                              : detail::floorsqrt_ct_helper_big<n>::value;
};

} // namespace detail

template <uint64_t n> constexpr uint64_t floorsqrt_ct() noexcept {
  return detail::floorsqrt_ct_helper<n>::value;
} // END FUNCTION floorsqrt_ct compiletime

/**
 * @brief Calcula la raíz cuadrada entera por defecto de un número en tiempo de
 * ejecución.
 * @details Implementación runtime del algoritmo de Newton-Raphson para calcular
 * floor(sqrt(n)). Usa lookup tables para valores pequeños (< 529) y
 * Newton-Raphson para valores mayores.
 * @tparam T Tipo integral del número.
 * @param n El número cuya raíz cuadrada se calcula.
 * @return La raíz cuadrada entera por defecto de `n`.
 */
template <typename T> constexpr T floorsqrt(T n) noexcept {
  /// VALORES PREESTABLECIDO COMO EN UNA TABLA
  /// AL MODO LOOKUP TABLES
  if (n == 0)
    return 0;
  if (n >= 1 && n < 4)
    return 1;
  if (n >= 4 && n < 9)
    return 2;
  if (n >= 9 && n < 16)
    return 3;
  if (n >= 16 && n < 25)
    return 4;
  if (n >= 25 && n < 36)
    return 5;
  if (n >= 36 && n < 49)
    return 6;
  if (n >= 49 && n < 64)
    return 7;
  if (n >= 64 && n < 81)
    return 8;
  if (n >= 81 && n < 100)
    return 9;
  if (n >= 100 && n < 121)
    return 10;
  if (n >= 121 && n < 144)
    return 11;
  if (n >= 144 && n < 169)
    return 12;
  if (n >= 169 && n < 196)
    return 13;
  if (n >= 196 && n < 225)
    return 14;
  if (n >= 225 && n < 256)
    return 15;
  if (n >= 256 && n < 289)
    return 16;
  if (n >= 289 && n < 324)
    return 17;
  if (n >= 324 && n < 361)
    return 18;
  if (n >= 361 && n < 400)
    return 19;
  if (n >= 400 && n < 441)
    return 20;
  if (n >= 441 && n < 484)
    return 21;
  if (n >= 484 && n < 529)
    return 22;

  // bit_width_ct requiere un tipo sin signo.
  using UnsignedT = std::make_unsigned_t<T>;
  T x0 = T(1) << (bit_width(static_cast<UnsignedT>(n)) / 2);

  // La iteración de Newton-Raphson converge desde arriba si la conjetura es >=
  // sqrt(n). Si nuestra conjetura x0 es una subestimación, la primera iteración
  // x1 será mayor. Nos aseguramos de que la conjetura inicial sea siempre una
  // sobreestimación o igual.
  if (x0 * x0 < n) {
    x0 = x0 * 2;
  }

  T x1 = (x0 + n / x0) / 2;
  while (x1 < x0) {
    x0 = x1;
    x1 = (x0 + n / x0) / 2;
  }

  // La iteración se detiene cuando x0 <= sqrt(n).
  // x0 es el resultado correcto.
  return x0;
} // END FUNCTION floorsqrt runtime

/**
 * @brief Calcula la raíz cuadrada entera por exceso (techo) de un número.
 * @tparam T El tipo de dato del número, debe ser un tipo integral.
 * @param n El número no negativo del que se calculará la raíz.
 * @return La raíz cuadrada entera por exceso de n.
 *
 * @pre `n` debe ser no negativo.
 * @post El valor devuelto `r` satisface `r*r >= n` y `(r-1)*(r-1) < n`.
 *
 * @par Properties
 * - Para `n = 0`, el resultado es `0`.
 * - Para `n = 1`, el resultado es `1`.
 * - Para `n` tal que `is_perfect_square(n)` es true, el resultado es
 * `ceilsqrt(n)`.
 * - Para `n` tal que `is_perfect_square(n)` es true, `ceilsqrt(n)*ceilsqrt(n)
 * == n`.
 * - La función es monotónicamente creciente: si `a > b`, entonces `ceilsqrt(a)
 * >= ceilsqrt(b)`.
 * - `ceilsqrt(n*n) == n` para cualquier `n` no negativo.
 *
 * @par Invariants
 *
 *
 * @par Things that should never happen
 * - La función no debe entrar en un bucle infinito.
 * - La función no debe devolver un valor `r` tal que `r*r < n`.
 * - La función no debe desbordarse durante los cálculos intermedios si `n` está
 * dentro del rango del tipo `T`.
 */
template <uint64_t n> consteval uint64_t ceilsqrt_ct() noexcept {
  if constexpr (n == 0)
    return 0;
  constexpr uint64_t root = floorsqrt_ct<n>();
  // Si n no es un cuadrado perfecto, la raíz cuadrada entera superior es root
  // + 1.
  if constexpr (root * root == n) {
    return root;
  } else {
    return root + 1;
  }
} // END FUNCTION ceilsqrt_ct compiletime

template <typename T> constexpr T ceilsqrt(T n) noexcept {
  if (n == 0)
    return 0;
  T root = floorsqrt(n);
  // Si n no es un cuadrado perfecto, la raíz cuadrada entera superior es root
  // + 1.
  return (root * root == n) ? root : root + 1;
} // END FUNCTION ceilsqrt runtime

/**
 * @brief Comprueba si un número es un cuadrado perfecto.
 * @tparam T El tipo de dato del número, debe ser un tipo integral.
 * @param n El número a comprobar.
 * @return true si n es un cuadrado perfecto, false en caso contrario.
 *
 * @pre `n` negativo daría necesariamente false.
 * @post 'Existe un entero r tal que r*r == n, r == ceilsqrt(n) ==
 * floorsqrt(n)'.
 *
 * @par Properties
 * - Para `n = 0`, el resultado es `true`.
 * - Para `n = 1`, el resultado es `true`.
 * - Para `n = 2`, el resultado es `false`.
 * - Para `n = 3`, el resultado es `false`.
 * - Para `n = 4`, el resultado es `true`.
 * - Para `n = 5`, el resultado es `false`.
 * - Para `n = 6`, el resultado es `false`.
 * - Para `n = 7`, el resultado es `false`.
 * - Para `n = 8`, el resultado es `false`.
 * - Para `n = 9`, el resultado es `true`.
 * - Para `n = 10`, el resultado es `false`.
 * - Para `n = 11`, el resultado es `false`.
 * - Para `n = 12`, el resultado es `false`.
 * - Para `n = 13`, el resultado es `false`.
 * - Para `n = 14`, el resultado es `false`.
 * - Para `n = 15`, el resultado es `false`.
 * - Para `n = 16`, el resultado es `true`.
 * - Para `n = 25`, el resultado es `true`.
 * - Para `n = 36`, el resultado es `true`.
 * - Para `n = 49`, el resultado es `true`.
 * - Para `n = 64`, el resultado es `true`.
 * - Para `n = 81`, el resultado es `true`.
 * - Para `n = 100`, el resultado es `true`.
 * - Para `n` tal que `is_perfect_square(n)` es true, `ceilsqrt(n) ==
 * floorsqrt(n)`.
 * - Si `r^2 == n`, entonces `(r+1)^2 == n + (2r + 1)`.
 * - Si `n` y `m` son cuadrados perfectos consecutivos, entonces para todo 'k'
 * en el rango (n, m), `is_perfect_square(k)` es false. no puede ser cuadrado
 * perfecto.
 * - Si `n,m,l` son cuadrados perfectos, tales que `n < m < l`, entonces `l-m >
 * m-n.
 *
 * @par Invariants
 *
 *
 * @par Things that should never happen
 * - La función no debe entrar en un bucle infinito.
 * - La función no debe desbordarse durante los cálculos intermedios si `n` está
 * dentro del rango del tipo `T`.
 */
template <typename T> constexpr bool is_perfect_square(T n) noexcept {
  if ((n == 0) || (n == 1))
    return true;
  T root = floorsqrt(n);
  return root * root == n;
} // END FUNCTION is_perfect_square runtime

// --- Backward compatibility wrappers for size_t ---
// This overload is kept for compatibility with code that might call it with 3
// arguments.
constexpr inline size_t ceilsqrt(size_t n, size_t, size_t) noexcept {
  return ceilsqrt<size_t>(n);
} // END FUNCTION ceilsqrt size_t with 3 args DEPRECATED

constexpr inline size_t ceilsqrt(size_t n) noexcept {
  return ceilsqrt<size_t>(n);
} // END FUNCTION ceilsqrt size_t DEPRECATED (WRAPPER)

/**
 * @brief Comprueba si un número es una potencia de 2.
 * @param num El número a comprobar.
 * @return true si el número es una potencia de 2, false en caso contrario.
 *
 * @pre `n` no será negativo ni `0`.
 *
 * @post
 *
 * @par Properties
 * - `is_prime(n) and is_power_of_2(n)` entonces `n == 2`.
 * - Existen exactamente 64 potencias de 2 en el rango de uint64_t: 2^0, 2^1,
 * 2^2, ..., 2^63.
 *
 * @par Invariants
 * - Si `n` es una potencia de 2, entonces, siempre que el cociente sea distinto
 * de 1 el resto de dividir por 2 será 0, en las sucesivas divisiones del
 * cociente.
 * - Si `n` no es una potencia de 2, entonces, en algún momento, el resto de
 * dividir por 2 será 1, sin ser el cociente 1.
 * - La única potencia de 2 que es impar es 2^0 == 1.
 * - Si `n` es potencia de 2, entonces `n & (n - 1) == 0`.
 *
 * @par Things that should never happen
 * - La función no debe entrar en un bucle infinito.
 * - La función no debe desbordarse durante los cálculos intermedios si `n`
 * está dentro del rango del tipo `T`.
 */
template <uint64_t n> struct floorsqrt_ct_helper_big {
  static constexpr uint64_t log2_n = (n == 0) ? 0 : int_log2_ct<n>();
  static constexpr uint64_t x0_base = (1ull << (log2_n / 2));
  static constexpr uint64_t x0 =
      (x0_base * x0_base < n) ? (x0_base * 2) : x0_base;
  static constexpr uint64_t value = floorsqrt_ct_newton<n, x0>();
};

enum class factor_error { invalid_range, domain_error };

/**
 * @brief Busca recursivamente un factor impar de un número en un rango en
 * tiempo de compilación.
 * @details Función auxiliar helper que implementa búsqueda binaria recursiva
 * para encontrar factores de la forma (2*k + 1) en el rango [low, high). Usada
 * internamente por `is_prime_ct`.
 * @tparam T Tipo integral.
 * @tparam n Número a factorizar.
 * @tparam low Límite inferior (inclusivo) del rango de búsqueda.
 * @tparam high Límite superior (exclusivo) del rango de búsqueda.
 * @return `true` si existe k en [low, high) tal que (2*k + 1) divide a n,
 * `false` en caso contrario.
 */
template <typename T, T n, T low, T high>
constexpr bool find_factor_ct() noexcept {
  if constexpr (low + 1 >= high) {
    return (n % (2 * low + 1)) == 0;
  } else {
    constexpr auto mid = std::midpoint(low, high);
    return find_factor_ct<T, n, low, mid>() ||
           find_factor_ct<T, n, mid, high>();
  }
} // END FUNCTION find_factor_ct compiletime

/**
 * @brief Busca recursivamente un factor impar de un número en un rango.
 * @details Esta es una función auxiliar para `is_prime`. Realiza una búsqueda
 * recursiva en el rango de enteros `[low, high)` para encontrar un entero `k`
 * tal que `2*k + 1` es un factor de `n`.
 *
 * @param n El número a factorizar.
 * @param low El límite inferior (inclusivo) del rango de búsqueda para `k`.
 * @param high El límite superior (exclusivo) del rango de búsqueda para `k`.
 * @return Un `expected<bool, factor_error>`. Contiene `true` si se encuentra un
 * factor, `false` si no se encuentra. Contiene un error si los parámetros de
 * entrada son inválidos.
 *
 * @pre `n > high`, `high > low` y `low > 1`.
 *
 * @post Si el valor es `true`, existe un entero `k` con `low <= k < high` tal
 * que `n % (2*k + 1) == 0`.
 * @post Si el valor es `false`, para todo `k` con `low <= k < high`, `n % (2*k
 * + 1) != 0`.
 */
template <typename T>
constexpr inline expected<bool, factor_error> find_factor(T n, T low,
                                                          T high) noexcept {
  if (low <= 1)
    return unexpected(factor_error::domain_error);
  if (n <= high)
    return unexpected(factor_error::domain_error);
  if (high <= low)
    return false; // Empty range, no factor found.

  const auto mid{std::midpoint(low, high)};
  if (low + 1 >= high) {
    return (n % (2 * low + 1)) == 0;
  }

  auto res_low = find_factor(n, low, mid);
  if (!res_low)
    return res_low; // propagate error
  if (*res_low)
    return true; // found

  return find_factor(n, mid, high);
} // END FUNCTION find_factor runtime

/**
 * @brief Comprueba si un número es primo en tiempo de compilación.
 * @tparam n El número a comprobar.
 * @return true si n es primo, false en caso contrario.
 *
 * @pre `n` debe ser no negativo.
 * @post El valor devuelto `r` satisface `r*r <= n` y `(r+1)*(r+1) > n`.
 *
 * @invariant
 * - La función siempre termina (los naturales tienen una buena ordenación, el
 * mínimo es 1).
 * - Los valores intermedios no causan desbordamiento (serán menores que el que
 * se le pasa).
 * - Para todo `k en [2, ceilsqrt_ct<n>()]`, si `n % k != 0`, entonces `n` no es
 * primo. Se puede comprobar sacando el primer factor `f` y verificando que `n %
 * f != 0`.
 * - Si `is_prime_ct<n>()` entonces `para todo k en [2, ceilsqrt_ct<n>()], n % k
 * == 0`. Esto se puede testear con números aleatorios una vez sabemos que
 * `is_prime_ct<n>()` es true.
 *
 * @par Properties
 * - Para `n = 0`, no se compila.
 * - Para `n = 1`, no se compila.
 * - Para `n = 2`, el resultado es `true`.
 * - Para `n = 3`, el resultado es `true`.
 * - Para `n = 4`, el resultado es `false`.
 * - Para `n = 5`, el resultado es `true`.
 * - Para `n = 6`, el resultado es `false`.
 * - Para `n = 7`, el resultado es `true`.
 * - Para `n = 8`, el resultado es `false`.
 * - Para `n = 9`, el resultado es `false`.
 * - Para `n = 10`, el resultado es `false`.
 * - Para `n = 11`, el resultado es `true`.
 * - Para `n = 12`, el resultado es `false`.
 * - Para `n = 13`, el resultado es `true`.
 * - Para `n = 14`, el resultado es `false`.
 * - Para `n = 15`, el resultado es `false`.
 * - Para `n = 16`, el resultado es `false`.
 * - Para `n = 17`, el resultado es `true`.
 * - Para `n = 18`, el resultado es `false`.
 * - Para `n = 19`, el resultado es `true`.
 * - Para `n = 20`, el resultado es `false`.
 * - Para `n = 21`, el resultado es `false`.
 * - Para `n = 22`, el resultado es `false`.
 * - Para `n = 23`, el resultado es `true`.
 * - Para `n = 24`, el resultado es `false`.
 * - Para `n = 25`, el resultado es `false`.
 * - Para `n = 36`, el resultado es `false`.
 * - Para `n = numeric_limits<uint64_t>::max()`, el resultado es `true`.
 * - Hay grandes tablas para probar aleatoriamente.
 *
 * @par Invariants
 *
 *
 * @par Things that should never happen
 * - La función no debe entrar en un bucle infinito.
 * - Si `n != 2`, entonces no se puede dar que `is_prime_ct<n>() &&
 * is_prime_ct<n+1>()`.
 * - La función no debe desbordarse durante los cálculos intermedios si `n` está
 * dentro del rango del tipo `T`.
 *
 * @note
 */
template <uint64_t n> consteval bool is_prime_ct() noexcept {
  if constexpr (n < 2)
    return false;
  if constexpr (n == 2 || n == 3)
    return true;
  if constexpr (n % 2 == 0 || n % 3 == 0)
    return false;
  if constexpr (n < 9)
    return true; // 5, 7 are prime
  if constexpr (n % 5 == 0)
    return false;
  if constexpr (n < 25)
    return true; // 11, 13, 17, 19, 23 are prime
  if constexpr (n % 7 == 0 || n % 11 == 0 || n % 13 == 0 || n % 17 == 0 ||
                n % 19 == 0 || n % 23 == 0)
    return false;
  if constexpr (n < 29 * 29) { // 29^2 = 841
    // For numbers in [25, 841), just check remaining small primes
    return true; // If we got here, n is not divisible by any prime < 29
  }
  // For numbers >= 841, check divisibility by more primes
  if constexpr (n % 29 == 0 || n % 31 == 0 || n % 37 == 0 || n % 41 == 0)
    return false;
  if constexpr (n % 43 == 0 || n % 47 == 0 || n % 53 == 0 || n % 59 == 0)
    return false;

  // For larger numbers, use find_factor_ct
  constexpr uint64_t high = (ceilsqrt_ct<n>() + 1) / 2;
  constexpr uint64_t low = 2;
  if constexpr (high <= low) {
    return true; // No factors to check
  } else {
    return !find_factor_ct<uint64_t, n, low, high>();
  }
} // END OF FUNCTION is_prime_ct

/**
 * @brief Comprueba si un número es primo.
 * @param n El número a comprobar.
 * @return true si n es primo, false en caso contrario.
 */
template <integral T> constexpr inline bool is_prime(T n) noexcept {
  if (n < 2)
    return false;
  if (n == 2 || n == 3)
    return true;
  if (n % 2 == 0 || n % 3 == 0)
    return false;
  if (n < 9)
    return true; // 5, 7 are prime
  if (n % 5 == 0)
    return false;
  if (n < 25)
    return true; // 11, 13, 17, 19, 23 are prime
  if (n % 7 == 0 || n % 11 == 0 || n % 13 == 0 || n % 17 == 0 || n % 19 == 0 ||
      n % 23 == 0)
    return false;
  if (n < 29 * 29) { // 29^2 = 841
    // For numbers in [25, 841), just check remaining small primes
    return true; // If we got here, n is not divisible by any prime < 29
  }
  // For numbers >= 841, check divisibility by more primes
  if (n % 29 == 0 || n % 31 == 0 || n % 37 == 0 || n % 41 == 0 || n % 43 == 0)
    return false;
  if (n % 47 == 0 || n % 53 == 0 || n % 59 == 0)
    return false;

  // Use find_factor for even larger numbers
  auto result = find_factor(n, static_cast<T>(2), (ceilsqrt(n) + 1) / 2);
  return !result.value_or(false);
} // END OF FUNCTION is_prime

/**
 * @brief Calcula el máximo común divisor (MCD) de dos números.
 * Algoritmo de Euclides implementado recursivamente.
 * @param a El primer número.
 * @param b El segundo número.
 * @return El MCD de a y b.
 */
template <uint64_t a, uint64_t b> consteval uint64_t gcd_ct() noexcept {
  if constexpr (b == 0) {
    return a;
  } else if constexpr (a == 0) {
    return b;
  } else if constexpr (a == b) {
    return a;
  } else if constexpr (b > a) {
    return gcd_ct<a, b % a>();
  } else {
    return gcd_ct<b, a % b>();
  }
} // END OF FUNCTION gcd_ct

/**
 * @brief Algoritmo de Euclides iterativo para calcular el MCD en tiempo de
 * ejecución.
 * @param a Primer número.
 * @param b Segundo número.
 * @return El máximo común divisor de a y b.
 */
constexpr inline uint64_t gcd(uint64_t a, uint64_t b) noexcept {
  if (a == 0)
    return b;
  if (b == 0)
    return a;
  if (a == 1)
    return 1;
  if (b == 1)
    return 1;
  if (a == b)
    return a;
  while (b != 0) {
    const auto temp = b;
    b = a % b;
    a = temp;
  }
  return a;
} // END OF FUNCTION gcd

/**
 * @brief Calcula el mínimo común múltiplo (mcm) de dos números,
 * sabiendo que `lcm(a,b)*gcd(a,b) == a*b.`
 * @param a El primer número.
 * @param b El segundo número.
 * @return El mcm de a y b.
 */
template <uint64_t a, uint64_t b> consteval uint64_t lcm_ct() noexcept {
  constexpr auto minimum{std::min(a, b)};
  constexpr auto maximum{std::max(a, b)};
  return (a == 0 || b == 0) ? 0 : // a o b es 0 <=> mcm es 0
             (minimum / gcd_ct<a, b>()) * maximum;
} // END OF FUNCTION lcm_ct

/**
 * @brief Calcula el mínimo común múltiplo en tiempo de ejecución.
 * @details Usa la relación lcm(a,b) * gcd(a,b) = a * b para calcular el mcm
 * eficientemente.
 * @param a Primer número.
 * @param b Segundo número.
 * @return El mínimo común múltiplo de a y b.
 */
constexpr inline uint64_t lcm(uint64_t a, uint64_t b) noexcept {
  return (a == 0 || b == 0) ? 0 : // a o b es 0 <=> mcm es 0
             (std::min(a, b) / gcd(a, b)) * std::max(a, b);
} // END OF FUNCTION lcm

/**
 * @brief Calcula la potencia de un número en tiempo de compilación.
 * @tparam base La base.
 * @tparam exponent El exponente.
 * @return base elevado al exponente.
 *
 * @pre exponent <= max_exponent_for_base_ct<base>()
 *
 * @post Para todo exponente, el valor devuelto es mayor que 0 si base > 0.
 *
 * @invariant int_pow_ct<base, exponent>() == base * int_pow_ct<base, exponent -
 * 1>() para exponent > 0.
 * @invariant int_pow_ct<base, 0>() == 1.
 * @invariant Si base == 0 y exponent > 0, entonces int_pow_ct<base, exponent>()
 * == 0.
 * @invariant Si base == 1, entonces int_pow_ct<base, exponent>() == 1 para
 * cualquier exponent.
 * @invariant Para todo exp_1 y exp_2 (exponentes) enteros, entonces se cumple
 * int_pow_ct<base, exp_1 + exp_2>() == int_pow_ct<base, exp_1>() *
 * int_pow_ct<base, exp_2>().
 *
 * @par Si exponent > max_exponent_for_base_ct<base>(), no compila.
 *
 */
template <uint64_t base, size_t exponent>
  requires(exponent <= max_exponent_for_base_ct<base>())
constexpr inline uint64_t int_pow_ct() noexcept {
  if constexpr (exponent == 0) {
    return 1;
  } else if constexpr (exponent == 1) {
    return base;
  } else if constexpr (base == 0) {
    return 0;
  } else if constexpr (base == 1) {
    return 1;
  } else {
    uint64_t result{1ull};
    uint64_t current_base{base};
    uint32_t exp{exponent};
    while (exp > 0) {
      if (exp & 1) {
        result *= current_base;
      }
      current_base *= current_base;
      exp >>= 1;
    }
    return result;
  }
} // END OF FUNCTION int_pow_ct

/**
 * @brief Potencia de 2 en tiempo de ejecución (bitwise, solo base==2).
 * @tparam T tipo entero (deducido)
 * @param base Debe ser 2.
 * @param exponent Exponente (debe ser <= max_exponent_for_base_ct<2>()).
 * @return 2 elevado a exponent, usando desplazamiento de bits.
 * @note Si base != 2, el resultado es indefinido.
 * @warning Para exponent >= 64, el resultado es indefinido (overflow).
 * @pre base == 2 && exponent <= max_exponent_for_base_ct<2>()
 * @note Esta función es una optimización específica para la base 2.
 * @note Si el exponente es mayor que 63, retorna 0 como indicativo de error.
 */
constexpr uint64_t int_pow(size_t exponent) noexcept {
  return (exponent <= max_exponent_for_base_ct<2>()) ? (1ull << exponent)
                                                     : 0ull;
} // END OF FUNCTION int_pow

/**
 * @brief Calcula la potencia de un número en tiempo de ejecución.
 * @param base La base.
 * @param exponent El exponente.
 * @return base elevado al exponente, o 0 si ocurre un desbordamiento.
 *
 * @pre exponent <= max_exponent_for_base(base)
 *
 * @post Para todo exponente, el valor devuelto es mayor que 0 si base > 0.
 *
 * @invariant int_pow(base, exponent) == base * int_pow(base, exponent - 1) para
 * exponent > 0.
 * @invariant int_pow(base, 0) == 1.
 * @invariant Si base == 0 y exponent > 0, entonces int_pow(base, exponent) ==
 * 0.
 * @invariant Si base == 1, entonces int_pow(base, exponent) == 1 para cualquier
 * exponent.
 * @invariant Para todo exp_1 y exp_2 (exponentes) enteros, entonces se cumple
 * int_pow(base, exp_1 + exp_2) == int_pow(base, exp_1) * int_pow(base, exp_2).
 *
 * @par Si exponent > max_exponent_for_base(base), retorna 0 como error.
 *
 */
constexpr inline uint64_t int_pow(uint64_t base, uint32_t exponent) noexcept {
  if (exponent <= max_exponent_for_base(base)) {
    // safe to compute
    if (exponent == 0)
      return 1;
    if (exponent == 1)
      return base;
    if (base == 0)
      return 0;
    if (base == 1)
      return 1;
    uint64_t result = 1;
    uint64_t current_base = base;
    while (exponent > 0) {
      if (exponent & 1) {
        result *= current_base;
      }
      current_base *= current_base;
      exponent >>= 1;
    }
    return result;
  } else {
    return 0; // overflow case
  }
} // END OF FUNCTION int_pow

/**
 * @brief Comprueba si un número es un cuadrado perfecto (sobrecarga para
 * uint64_t).
 * @param n El número a comprobar.
 * @return true si n es un cuadrado perfecto, false en caso contrario.
 */
template <uint64_t n> consteval bool is_perfect_square_ct() noexcept {
  if constexpr (n == 0ull || n == 1ull) {
    return true;
  }
  constexpr auto root = floorsqrt_ct<n>();
  return root * root == n;
} // END OF FUNCTION is_perfect_square_ct

constexpr inline bool is_perfect_square(uint64_t n) noexcept {
  if (n == 0 || n == 1) {
    return true;
  }
  const auto root = ceilsqrt(n);
  return root * root == n;
} // END OF FUNCTION is_perfect_square

// Declaración anticipada (definición más abajo)
// constexpr uint64_t int_log2(uint64_t n) noexcept;
// forward declaration for count_digits_base (defined below)
constexpr size_t count_digits_base(uint64_t n, uint64_t base) noexcept;

/**
 * @brief Calcula el logaritmo entero en tiempo de compilación.
 * @tparam base La base del logaritmo.
 * @tparam n El número del que se calcula el logaritmo.
 * @return El logaritmo entero de n en base `base`.
 *
 * @pre n > 0
 *
 * @post int_pow_ct<base, int_log_ct<base, n>>() <= n < int_pow_ct<base,
 * int_log_ct<base, n> + 1>()
 */
template <uint64_t base, int64_t n> constexpr int64_t int_log_ct() noexcept {
  if constexpr (n <= 0) {
    return -1; // Not in domain of the function log_base(n)
               // Domain(log_base) = ]0, +infinity[ = [1, +infinity[
  } else if constexpr (n < base) {
    return 0;
  } else {
    return 1 + int_log_ct<base, n / base>();
  }
} // END OF FUNCTION int_log_ct

/**
 * @brief Calcula el logaritmo entero en tiempo de ejecución.
 * @param base La base del logaritmo.
 * @param n El número del que se calcula el logaritmo.
 * @return El logaritmo entero de n en base `base`.
 *
 * @pre n > 0
 *
 * @post  int_pow<base, int_log(base, n)> <= n < int_pow<base, int_log(base, n)
 * + 1>()
 */
constexpr int64_t int_log(uint64_t base, int64_t n) noexcept {
  if (n <= 0) {
    return -1; // Not in domain of the function log_base(n)
               // Domain(log_base) = ]0, +infinity[ = [1, +infinity[
  } else if (static_cast<uint64_t>(n) < base) {
    return 0;
  } else {
    return 1 + int_log(base, n / base);
  }
} // END OF FUNCTION int_log

/**
 * @brief Cuenta el número de dígitos de un número en base 10.
 * @param n El número.
 * @return El número de dígitos en base 10.
 */
constexpr inline size_t count_digits_base10(uint64_t n) noexcept {
  return count_digits_base(n, 10);
} // END OF FUNCTION count_digits_base10

/**
 * @brief Función auxiliar recursiva para verificar si base^exponent <= limit.
 * @details Implementa exponenciación por cuadrados de forma recursiva con
 * detección de overflow. Usada internamente por `pow_leq_limit_ct` y
 * `count_digits_base`.
 * @tparam result Resultado acumulado parcial.
 * @tparam cur Base actual elevada a potencia de 2.
 * @tparam e Exponente restante.
 * @tparam limit Límite superior a verificar.
 * @return `true` si result * cur^e <= limit sin overflow, `false` en caso
 * contrario.
 */
template <uint64_t result, uint64_t cur, uint32_t e, uint64_t limit>
constexpr bool pow_leq_limit_ct_helper() noexcept {
  // 0^0 = 1 por convención matemática <= limit si limit > 0
  // 0^e = 0 para e > 0 <= limit si limit >= 0
  if constexpr (cur == 0) {
    if constexpr (e == 0) {
      return 1 <= limit;
    } else {
      return 0 <= limit;
    }
  } else if constexpr (e == 0) {
    return result <= limit;
  } else {
    // Protección contra división por cero y overflow
    if constexpr (cur == 0 || result > numeric_limits<uint64_t>::max() / cur) {
      return false;
    }
    if constexpr (e & 1u) {
      constexpr uint64_t new_result = result * cur;
      constexpr uint32_t new_e = e >> 1u;
      if constexpr (new_e == 0) {
        return new_result <= limit;
      } else {
        if constexpr (cur > numeric_limits<uint64_t>::max() / cur) {
          return false;
        } else {
          constexpr uint64_t new_cur = cur * cur;
          return pow_leq_limit_ct_helper<new_result, new_cur, new_e, limit>();
        }
      }
    } else {
      constexpr uint32_t new_e = e >> 1u;
      if constexpr (new_e == 0) {
        return result <= limit;
      } else {
        if constexpr (cur > numeric_limits<uint64_t>::max() / cur) {
          return false;
        } else {
          constexpr uint64_t new_cur = cur * cur;
          return pow_leq_limit_ct_helper<result, new_cur, new_e, limit>();
        }
      }
    }
  }
} // END OF FUNCTION pow_leq_limit_ct

/**
 * @brief Checks if base^exponent is less than or equal to a limit
 * using exponentiation by squaring.
 * @param b The base.
 * @param exp The exponent.
 * @param limit The limit to check against.
 * @return True if b^exp <= limit, false otherwise. This function is
 * overflow-safe.
 */
template <uint64_t b, uint32_t exp, uint64_t limit>
constexpr bool pow_leq_limit_ct() noexcept {
  if constexpr (b == 0) {
    if constexpr (exp == 0) {
      return 1 <= limit;
    } else {
      return 0 <= limit;
    }
  }
  if constexpr (b == 1)
    return 1 <= limit;
  if constexpr (exp == 0)
    return 1 <= limit;
  return pow_leq_limit_ct_helper<1ull, b, exp, limit>();
}

constexpr inline bool pow_leq_limit(uint64_t b, uint32_t exp,
                                    uint64_t limit) noexcept {

  if (b == 0)
    if (exp == 0)
      return 1 <= limit;
    else {
      return 0 <= limit;
    }
  if (b == 1)
    return 1 <= limit;
  if (exp == 0)
    return 1 <= limit;

  uint64_t result{1ull};
  uint64_t cur{b};
  uint32_t e{exp};

  while (e > 0) {
    if (e & 1u) {
      if (result > limit / cur)
        return false; // would exceed limit
      result *= cur;
    }
    e >>= 1u;
    if (e > 0) {
      if (cur > numeric_limits<uint64_t>::max() / cur) {
        // cur * cur would overflow, so the mathematical result is definitely >
        // limit.
        return false;
      }
      cur *= cur;
    }
  }
  return result <= limit;
} // END OF FUNCTION pow_leq_limit

/**
 * @brief Cuenta el número de dígitos de un número en una base dada.
 * @param n El número.
 * @param base La base.
 * @return El número de dígitos de n en la base dada.
 */
constexpr inline size_t count_digits_base(uint64_t n, uint64_t base) noexcept {
  if (base < 2)
    return 0; // invalid base
  if (n == 0)
    return 1; // zero has one digit in any base

  // Fast path for base 2 using int_log2
  if (base == 2) {
    return static_cast<size_t>(int_log2(n)) + 1u;
  }

  uint32_t lo = 0u;
  uint32_t hi = 1u;
  while (pow_leq_limit(base, hi, n)) {
    lo = hi;
    hi = hi * 2u;
    if (hi == 0u)
      break; // overflow guard
  }

  while (lo + 1u < hi) {
    uint32_t mid = lo + (hi - lo) / 2u;
    if (pow_leq_limit(base, mid, n))
      lo = mid;
    else
      hi = mid;
  }

  return static_cast<size_t>(lo) + 1u;
} // END OF FUNCTION count_digits_base

// Eliminar aliases ambiguos y versiones duplicadas: log2, log2ct, pow2ct, pow2,
// etc.

/**
 * @brief Errores posibles en conversión segura de representación a entero.
 */
enum class ConversionError {
  InvalidDigit, ///< Dígito >= base (dígito inválido para la base)
  Overflow,     ///< Resultado excede uint64_t::max
  InvalidBase   ///< Base < 2 (base inválida)
};

/**
 * @brief Convierte representación numérica base-B a uint64_t.
 * @details Transforma secuencia de dígitos en base B a su valor decimal
 * equivalente. Algoritmo de Horner: para dígitos [d₀, d₁, ..., dₙ]: resultado =
 * ((...((dₙ * B) + dₙ₋₁) * B) + ...) * B) + d₀
 *
 * @tparam B Base del sistema numérico (2, 8, 10, 16, 256, etc.).
 * @tparam L Longitud (número de dígitos).
 * @tparam A Tipo del contenedor de dígitos.
 *
 * @param arg Objeto contenedor que provee acceso a dígitos via
 * operator[](index)(). Cada dígito debe ser accesible como arg[i](),
 * devolviendo uint64_t.
 *
 * @return Valor entero equivalente en uint64_t.
 *
 * @pre Todos los dígitos son válidos (< B).
 * @pre No hay overflow (resultado < B^L debe caber en uint64_t).
 * @pre arg[i]() devuelve valor convertible a uint64_t.
 *
 * @note Complejidad: O(L) - una multiplicación y una suma por dígito.
 * @note Sin validación de errores. Para versión segura, usa
 * conversion_to_int_safe.
 * @note Orden little-endian: arg[0] es el dígito menos significativo.
 *
 * @code
 * // Ejemplo: Convertir 456 en base 10
 * reg_digs_t<10, 3> rd;
 * rd[0] = dig_t<10>(6); // unidades
 * rd[1] = dig_t<10>(5); // decenas
 * rd[2] = dig_t<10>(4); // centenas
 *
 * uint64_t value = conversion_to_int<10, 3>(rd); // value == 456
 *
 * // Proceso interno:
 * // acc = rd[2]() = 4
 * // acc = 4*10 + rd[1]() = 40 + 5 = 45
 * // acc = 45*10 + rd[0]() = 450 + 6 = 456
 * @endcode
 */
template <auto B, auto L, typename A>
constexpr inline uint64_t conversion_to_int(const A &arg) noexcept {
  constexpr uint64_t base{static_cast<uint64_t>(B)};
  uint64_t acc{arg[L - 1]()};
  for (sint64_t ix{L - 2}; ix > -1; --ix) {
    acc *= base;
    acc += static_cast<uint64_t>(arg[ix]());
  };
  return acc;
} // END OF FUNCTION conversion_to_int

/**
 * @brief Versión segura de conversion_to_int con validación y manejo de
 * errores.
 * @details Convierte representación base-B a uint64_t verificando:
 *          - Cada dígito < base (validez)
 *          - No overflow durante multiplicación y suma
 *          - Base >= 2 (válida)
 *
 * @tparam B Base del sistema numérico (debe ser >= 2).
 * @tparam L Longitud (número de dígitos).
 * @tparam A Tipo del contenedor de dígitos.
 *
 * @param arg Objeto contenedor con dígitos accesibles via operator[](index)().
 *
 * @return expected<uint64_t, ConversionError>
 *         - Éxito: valor convertido
 *         - Error: InvalidDigit, Overflow, o InvalidBase
 *
 * @note Usa expected (C++23) para manejo de errores sin excepciones.
 * @note Complejidad: O(L) con verificaciones de overflow en cada paso.
 *
 * @code
 * // Ejemplo: Conversión segura
 * reg_digs_t<10, 3> rd;
 * rd[0] = dig_t<10>(6); rd[1] = dig_t<10>(5); rd[2] = dig_t<10>(4);
 *
 * auto result = conversion_to_int_safe<10, 3>(rd);
 * if (result) {
 *     std::cout << "Valor: " << *result << '\n'; // 456
 * } else {
 *     switch (result.error()) {
 *         case ConversionError::InvalidDigit:
 *             std::cout << "Dígito inválido\n"; break;
 *         case ConversionError::Overflow:
 *             std::cout << "Overflow detectado\n"; break;
 *         case ConversionError::InvalidBase:
 *             std::cout << "Base inválida\n"; break;
 *     }
 * }
 * @endcode
 */
template <auto B, auto L, typename A>
constexpr inline expected<uint64_t, ConversionError>
conversion_to_int_safe(const A &arg) noexcept {
  constexpr uint64_t base{static_cast<uint64_t>(B)};

  // Validar base
  if constexpr (base < 2) {
    return std::unexpected(ConversionError::InvalidBase);
  }

  // Obtener primer dígito y validar
  uint64_t digit_val = static_cast<uint64_t>(arg[L - 1]());
  if (digit_val >= base) {
    return std::unexpected(ConversionError::InvalidDigit);
  }

  uint64_t acc{digit_val};
  constexpr uint64_t max_before_mult = numeric_limits<uint64_t>::max() / base;

  for (sint64_t ix{L - 2}; ix > -1; --ix) {
    // Verificar overflow en multiplicación
    if (acc > max_before_mult) {
      return std::unexpected(ConversionError::Overflow);
    }

    digit_val = static_cast<uint64_t>(arg[ix]());

    // Validar dígito
    if (digit_val >= base) {
      return std::unexpected(ConversionError::InvalidDigit);
    }

    acc *= base;

    // Verificar overflow en suma
    if (acc > numeric_limits<uint64_t>::max() - digit_val) {
      return std::unexpected(ConversionError::Overflow);
    }

    acc += digit_val;
  }

  return acc;
} // END OF FUNCTION conversion_to_int_safe

/**
 * @brief Versión compile-time de conversion_to_int.
 * @details Idéntica a conversion_to_int pero marcada como consteval para
 * garantizar evaluación en tiempo de compilación. Sin validación de errores.
 *
 * @tparam B Base del sistema numérico.
 * @tparam L Longitud (número de dígitos).
 * @tparam A Tipo del contenedor de dígitos.
 *
 * @param arg Objeto contenedor con dígitos.
 * @return Valor convertido en uint64_t.
 *
 * @pre Todos los dígitos son válidos (< B)
 * @pre No hay overflow (resultado < B^L <= uint64_t::max)
 *
 * @note Uso: Cuando necesitas garantía compile-time sin overhead de validación.
 *
 * @code
 * constexpr auto value = conversion_to_int_ct<10, 3>(rd); // Compile-time
 * @endcode
 */
template <auto B, auto L, typename A>
consteval inline uint64_t conversion_to_int_ct(const A &arg) noexcept {
  constexpr uint64_t base{static_cast<uint64_t>(B)};
  uint64_t acc{arg[L - 1]()};
  for (sint64_t ix{L - 2}; ix > -1; --ix) {
    acc *= base;
    acc += static_cast<uint64_t>(arg[ix]());
  }
  return acc;
} // END OF FUNCTION conversion_to_int_ct

/**
 * @brief Versión compile-time SEGURA con validación de errores.
 * @details Combina evaluación compile-time (consteval) con validación completa
 *          usando expected. Ideal para inicializaciones constexpr seguras.
 *
 * @tparam B Base del sistema numérico (debe ser >= 2).
 * @tparam L Longitud (número de dígitos).
 * @tparam A Tipo del contenedor de dígitos.
 *
 * @param arg Objeto contenedor con dígitos.
 * @return expected<uint64_t, ConversionError>
 *
 * @note Si falla en compile-time, el compilador generará error descriptivo.
 * @note Uso: Validación en compile-time con manejo explícito de errores.
 *
 * @code
 * // Compile-time con validación
 * constexpr auto result = conversion_to_int_safe_ct<10, 3>(rd);
 * static_assert(result.has_value(), "Conversión inválida");
 * constexpr uint64_t value = *result; // 456
 * @endcode
 */
template <auto B, auto L, typename A>
consteval inline expected<uint64_t, ConversionError>
conversion_to_int_safe_ct(const A &arg) noexcept {
  constexpr uint64_t base{static_cast<uint64_t>(B)};

  // Validar base
  if constexpr (base < 2) {
    return std::unexpected(ConversionError::InvalidBase);
  }

  // Obtener primer dígito y validar
  uint64_t digit_val = static_cast<uint64_t>(arg[L - 1]());
  if (digit_val >= base) {
    return std::unexpected(ConversionError::InvalidDigit);
  }

  uint64_t acc{digit_val};
  constexpr uint64_t max_before_mult = numeric_limits<uint64_t>::max() / base;

  for (sint64_t ix{L - 2}; ix > -1; --ix) {
    // Verificar overflow en multiplicación
    if (acc > max_before_mult) {
      return std::unexpected(ConversionError::Overflow);
    }

    digit_val = static_cast<uint64_t>(arg[ix]());

    // Validar dígito
    if (digit_val >= base) {
      return std::unexpected(ConversionError::InvalidDigit);
    }

    acc *= base;

    // Verificar overflow en suma
    if (acc > numeric_limits<uint64_t>::max() - digit_val) {
      return std::unexpected(ConversionError::Overflow);
    }

    acc += digit_val;
  }

  return acc;
} // END OF FUNCTION conversion_to_int_safe_ct

namespace special {
// ...existing code...
} // namespace special
} // namespace AuxFunc
} // namespace NumRepr

#endif // AUXILIARY_FUNCTIONS_HPP_INCLUDED
