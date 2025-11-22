#ifndef NUMREPR_INCLUDE_CORE_INTERNAL_LOOKUP_TABLES_PRIMES_HPP
#define NUMREPR_INCLUDE_CORE_INTERNAL_LOOKUP_TABLES_PRIMES_HPP

#include "LUTs.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <type_traits>

// Incluir intrínsecos para MSVC (_umul128, _udiv128)
#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace NumRepr {
namespace AuxFunc {
namespace LUT {

/**
 * @brief Multiplicación modular segura: (a * b) % m
 * @details Calcula (a * b) % m usando 128 bits para evitar desbordamiento.
 * Implementación portable para MSVC y GCC/Clang.
 */
inline uint64_t mulmod(uint64_t a, uint64_t b, uint64_t m) {
#ifdef _MSC_VER
  // Implementación para MSVC usando intrínsecos para 128 bits
  unsigned __int64 high;
  unsigned __int64 low = _umul128(a, b, &high);
  unsigned __int64 rem;
  _udiv128(high, low, m, &rem);
  return rem;
#else
  // Implementación para GCC/Clang usando __uint128_t
  return (static_cast<unsigned __int128>(a) * b) % m;
#endif
}

/**
 * @brief Exponenciación modular binaria: (base^exp) % mod
 * @details Calcula la potencia modular de forma eficiente O(log exp).
 * Utiliza mulmod para evitar desbordamientos en los pasos intermedios.
 */
inline uint64_t binpower(uint64_t base, uint64_t exp, uint64_t mod) {
  uint64_t result = 1;
  base %= mod;
  while (exp > 0) {
    if (exp & 1) {
      result = mulmod(result, base, mod); // Usa mulmod seguro
    }
    base = mulmod(base, base, mod);       // Usa mulmod seguro
    exp >>= 1;
  }
  return result;
}

/**
 * @brief Test de Miller-Rabin para un testigo 'a'
 * @param a Testigo (base) para la prueba
 * @param d Parte impar de n-1 (n-1 = d * 2^s)
 * @param n Número a probar
 * @param s Potencia de 2 en la descomposición de n-1
 * @return true si 'n' es compuesto, false si es probablemente primo
 */
inline bool check_composite(uint64_t a, uint64_t d, uint64_t n, int s) {
  uint64_t x = binpower(a, d, n);
  if (x == 1 || x == n - 1)
    return false;
  for (int r = 1; r < s; ++r) {
    x = mulmod(x, x, n); // Usa mulmod seguro
    if (x == n - 1)
      return false;
  }
  return true;
};

// Versión runtime: primalidad para n < 65537
inline bool is_prime_leq_65537(uint16_t value) {
  return std::binary_search(primes_lt_65537.begin(), primes_lt_65537.end(),
                            value);
}

// Versión runtime: primalidad general (usa la tabla y Miller-Rabin para n >
// 65536)
inline bool isPrime(uint64_t n) {
  if (n < 2)
    return false;
  if (n < 65537)
    return is_prime_leq_65537(static_cast<uint16_t>(n));
  // Prueba de divisibilidad por primos pequeños
  for (auto p : primes_lt_65537) {
    if (uint64_t(p) * uint64_t(p) > n)
      break;
    if (n % p == 0)
      return false;
  }
  // Miller-Rabin determinista para n < 2^64
  static constexpr uint64_t witnesses[] = {2,  3,  5,  7,  11, 13, 17, 19, 23,
                                           29, 31, 37, 41, 43, 47, 53, 59, 61,
                                           67, 71, 73, 79, 83, 89, 97};
  uint64_t d = n - 1;
  int s = 0;
  while ((d & 1) == 0) {
    d >>= 1;
    ++s;
  }

  for (uint64_t a : witnesses) {
    if (a >= n)
      break;
    // Llama a la función externa check_composite
    if (check_composite(a, d, n, s))
      return false;
  }
  return true;
}

} // namespace LUT
} // namespace AuxFunc
} // namespace NumRepr

#endif // NUMREPR_INCLUDE_CORE_INTERNAL_LOOKUP_TABLES_PRIMES_HPP
