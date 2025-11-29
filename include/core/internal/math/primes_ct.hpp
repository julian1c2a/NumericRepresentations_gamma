#ifndef NUMREPR_INCLUDE_CORE_INTERNAL_MATH_PRIMES_CT_HPP_INCLUDED
#define NUMREPR_INCLUDE_CORE_INTERNAL_MATH_PRIMES_CT_HPP_INCLUDED

#include "tables/PrimeList.hpp"
#include "tables/EratosthenesSieve.hpp"

#include <cstdint>
#include <array>
#include <algorithm> // Para std::lower_bound
#include <concepts>
#include <type_traits>
#include "core/internal/append/int128_ops.hpp"

namespace NumRepr {
namespace AuxFunc {
namespace LUT {

    using std::array;
    using std::size_t;
    using std::uint16_t;
    using std::uint64_t;

    // =============================================================================
    // UTILIDADES CONSTEXPR ITERATIVAS (Sin recursión de plantillas)
    // =============================================================================

    // Búsqueda binaria iterativa en tiempo de compilación
    constexpr bool binary_search_constexpr(uint64_t value) {
        // Usamos std::lower_bound que es constexpr en C++20/23
        auto it = std::lower_bound(primes_lt_65537.begin(), primes_lt_65537.end(), static_cast<uint16_t>(value));
        return (it != primes_lt_65537.end() && *it == value);
    }

    // Primalidad directa por tabla para n < 65537
    constexpr bool is_prime_lt_65537_ct(uint64_t value) {
        if (value < 2) return false;
        if (value >= 65536) return false;
        return is_prime_lt_65537_lut[value];
    }

    // Multiplicación modular segura (128 bits) iterativa
    constexpr uint64_t mulmod_ct(uint64_t a, uint64_t b, uint64_t mod) {
        // En C++23 y compiladores modernos, __uint128_t funciona en constexpr
        return static_cast<uint64_t>((static_cast<unsigned __int128>(a) * b) % mod);
    }

    // Exponenciación modular binaria iterativa
    // CORRIGE EL BUG DE LA VERSIÓN RECURSIVA: La base se actualiza siempre.
    constexpr uint64_t binpower_ct(uint64_t base, uint64_t exp, uint64_t mod) {
        uint64_t res = 1;
        base %= mod;
        while (exp > 0) {
            if (exp % 2 == 1) res = mulmod_ct(res, base, mod);
            base = mulmod_ct(base, base, mod);
            exp /= 2;
        }
        return res;
    }

    struct DecompResult {
        uint64_t d;
        int s;
    };

    // Descomposición n-1 = d * 2^s iterativa
    constexpr DecompResult decompose_ct(uint64_t n_minus_1) {
        uint64_t d = n_minus_1;
        int s = 0;
        while ((d & 1) == 0) {
            d >>= 1;
            s++;
        }
        return {d, s};
    }

    // Comprobación de divisibilidad por primos pequeños iterativa
    constexpr bool divides_by_small_prime_ct(uint64_t n) {
        for (const auto& p : primes_lt_65537) {
            // Optimización: Si p*p > n, ya no necesitamos comprobar más
            // Cast a 128 bit para evitar overflow en p*p
            if (static_cast<unsigned __int128>(p) * p > n) return false;
            if (n % p == 0) return true;
        }
        return false;
    }

    // =============================================================================
    // TEST DE MILLER-RABIN ITERATIVO
    // =============================================================================

    static constexpr array<uint64_t, 25> miller_rabin_witnesses {{
        2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43,
        47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97
    }};

    // Verifica si 'a' es un testigo de que 'n' es compuesto
    constexpr bool check_composite_ct(uint64_t n, uint64_t a, uint64_t d, int s) {
        uint64_t x = binpower_ct(a, d, n);
        if (x == 1 || x == n - 1) return false;
        
        for (int r = 1; r < s; ++r) {
            x = mulmod_ct(x, x, n);
            if (x == n - 1) return false;
        }
        return true;
    }

    // Algoritmo Miller-Rabin principal iterativo
    constexpr bool miller_rabin_ct(uint64_t n, uint64_t d, int s) {
        for (const auto& a : miller_rabin_witnesses) {
            if (a >= n) break;
            if (check_composite_ct(n, a, d, s)) return false; // Es compuesto
        }
        return true; // Probablemente primo (para n < 2^64 es determinista con estos testigos)
    }

    // =============================================================================
    // INTERFAZ PÚBLICA (Templates)
    // =============================================================================

    // Implementación lógica unificada
    constexpr bool isPrime_impl(uint64_t n) {
        if (n < 2) return false;
        if (n < 65537) return is_prime_lt_65537_lut[n]; // O(1) Lookup

        // Paso 1: Divisibilidad por primos pequeños (Iterativo)
        if (divides_by_small_prime_ct(n)) return false;

        // Paso 2: Miller-Rabin (Iterativo)
        auto decomp = decompose_ct(n - 1);
        return miller_rabin_ct(n, decomp.d, decomp.s);
    }

    // Wrapper consteval para forzar evaluación en tiempo de compilación
    template <uint64_t n>
    consteval bool isPrime_ct() {
        return isPrime_impl(n);
    }

} // namespace LUT
} // namespace AuxFunc
} // namespace NumRepr

#endif // NUMREPR_INCLUDE_CORE_INTERNAL_MATH_PRIMES_CT_HPP_INCLUDED