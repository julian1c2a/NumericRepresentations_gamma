#ifndef NUMREPR_INCLUDE_CORE_INTERNAL_LOOKUP_TABLES_PRIMES_HPP
#define NUMREPR_INCLUDE_CORE_INTERNAL_LOOKUP_TABLES_PRIMES_HPP

#include "LUTs.hpp"

// Dependencia: Aritmética de 128 bits
#include "../../append/int128_ops.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <type_traits>

namespace NumRepr {
  namespace AuxFunc {
    namespace LUT {

      /**
       * @brief Exponenciación modular binaria: (base^exp) % mod
       * @details Utiliza NumRepr::mulmod para evitar desbordamientos.
       */
      inline uint64_t binpower(uint64_t base, uint64_t exp, uint64_t mod) {
        uint64_t result = 1;
        base %= mod;
        while (exp > 0) {
          if (exp & 1)
            result = NumRepr::mulmod(result, base, mod); // Usa la versión centralizada
            base = NumRepr::mulmod(base, base, mod);       // Usa la versión centralizada
            exp >>= 1;
        }
        return result;
      }

      /**
       * @brief Test de Miller-Rabin para un testigo 'a'
       */
      inline bool check_composite(uint64_t a, uint64_t d, uint64_t n, int s) {
        uint64_t x = binpower(a, d, n);
        if (x == 1 || x == n - 1)
          return false;
        for (int r = 1; r < s; ++r) {
          x = NumRepr::mulmod(x, x, n); // Usa la versión centralizada
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

      // Versión runtime: primalidad general
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
            if (check_composite(a, d, n, s))
              return false;
          }
          return true;
      }

    } // namespace LUT
  } // namespace AuxFunc
} // namespace NumRepr

#endif // NUMREPR_INCLUDE_CORE_INTERNAL_LOOKUP_TABLES_PRIMES_HPP
