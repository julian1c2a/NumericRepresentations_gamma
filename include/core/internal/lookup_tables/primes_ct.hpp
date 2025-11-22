#ifndef NUMREPR_INCLUDE_CORE_INTERNAL_LOOKUP_TABLES_PRIMES_COMPILETIME_HPP
#define NUMREPR_INCLUDE_CORE_INTERNAL_LOOKUP_TABLES_PRIMES_COMPILETIME_HPP

#include <cstdint>
#include <array>
#include <algorithm>
#include <concepts>
#include <type_traits>
#include "LUTs.hpp"

namespace NumRepr
{
    namespace AuxFunc
    {
        namespace LUT
        {
            using std::array;
            using std::binary_search;
            using std::int64_t;
            using std::size_t;
            using std::uint16_t;
            using std::uint32_t;
            using std::uint64_t;

            // --- Búsqueda binaria constexpr (template, para static_assert) ---
            template <
                uint64_t value, 
                size_t left = 0, 
                size_t right = primes_lt_65537.size()
            > struct binary_search_constexpr_impl {
                static constexpr size_t mid = left + (right - left) / 2;
                static constexpr uint16_t mid_value { 
                    primes_lt_65537_lut[mid] 
                };
                static constexpr bool result { 
                    (
                        (mid_value == value)
                    ? 
                        true
                    :   
                    (
                        (mid_value < value) 
                        ?
                        (
                            binary_search_constexpr_impl<
                                value, 
                                mid + 1, 
                                right
                            >::result
                        )
                        : 
                        (
                            binary_search_constexpr_impl<
                                value, 
                                left, 
                                mid
                            >::result
                        )
                    )
                )
                };
            };
            template <uint64_t value, size_t pos>
            struct binary_search_constexpr_impl<value, pos, pos> {
                static constexpr bool result = false;
            };
            template <uint64_t value>
            constexpr bool binary_search_constexpr() {
                return binary_search_constexpr_impl<value>::result;
            }

            // --- Búsqueda binaria runtime ---
            constexpr bool binary_search_runtime(uint64_t value) {
                size_t left = 0, right = primes_lt_65537.size();
                while (left < right)
                {
                    size_t mid = left + (right - left) / 2;
                    uint16_t mid_value = primes_lt_65537[mid];
                    if (mid_value == value)
                        return true;
                    if (mid_value < value)
                        left = mid + 1;
                    else
                        right = mid;
                }
                return false;
            }

            // --- Primalidad para n < 65537 ---
            // Compile-time (template, para static_assert)
            template <uint64_t value>
            constexpr bool is_prime_lt_65537_ct() {
                if constexpr (value < 2 || value >= 65537)
                    return false;
                return binary_search_constexpr<value>();
            }

            // -----------------------------------------------------------------
            // TEST DE MILLER-RABIN
            // -----------------------------------------------------------------

            // --- Miller-Rabin determinista constexpr para n < 2^64 ---
            // Testigos fijos recomendados para n < 2^64:
            static constexpr uint64_t miller_rabin_witnesses[]{
                2, 3, 5, 7, 11, 13, 17,
                19, 23, 29, 31, 37, 41, 43,
                47, 53, 59, 61, 67, 71, 73,
                79, 83, 89, 97};

            // Tipo para la descomposición MILLER-RABIN n-1 = d*2^s
            struct DecompResult {
                uint64_t d;
                int64_t s;
            };

            // Descomponer n-1 = d*2^s
            template <uint64_t n_minus_1, int s = 0>
            constexpr DecompResult decompose_ct() {
                if constexpr (n_minus_1 & 1) {
                    return DecompResult{n_minus_1, s};
                } else {
                    return decompose_ct<(n_minus_1 >> 1), s + 1>();
                }
            }

            // Recursivo constexpr para probar divisibilidad por primos pequeños
            template <uint64_t n, size_t I = 0>
            constexpr bool divides_by_small_prime_ct() {
                if constexpr (I >= primes_lt_65537.size())
                    {return false;}
                else if constexpr (primes_lt_65537[I] * primes_lt_65537[I] > n)
                    {return false;}
                else if constexpr (n % primes_lt_65537[I] == 0)
                    {return true;}
                else
                    {return divides_by_small_prime_ct<n, I + 1>();}
            }

            // mulmod_ct constexpr seguro para evitar overflow
            // Implementación recursiva de mulmod_ct_impl
            template <uint64_t a, uint64_t b, uint64_t mod, uint64_t result = 0>
            constexpr uint64_t mulmod_ct_impl() {
                constexpr uint64_t new_a { (a << 1) % mod };
                constexpr uint64_t new_b { b >> 1 };
                constexpr uint64_t new_result { 
                    (result + ((b & 1) ? a : 0)) % mod 
                };
                return  ((b == 0)   ? 
                            result  : 
                            mulmod_ct_impl<
                                new_a, 
                                new_b, 
                                mod, 
                                new_result
                            >()
                        );
            }

            template <uint64_t a, uint64_t b, uint64_t mod>
            constexpr uint64_t mulmod_ct() {
                return mulmod_ct_impl<a % mod, b, mod, 0>();
            }

            // binpower recursivo para Miller-Rabin
            template <  uint64_t base, 
                        uint64_t exp, 
                        uint64_t mod, 
                        uint64_t result = 1 >
            constexpr uint64_t binpower_ct() {
                if constexpr (exp == 0) {
                    return result;
                } else {
                    if constexpr (exp & 1) {
                        return binpower_ct<
                                    base, 
                                    (exp >> 1), 
                                    mod, 
                                    mulmod_ct<result, base % mod, mod>()
                            >();
                    } else {
                        return 
                            binpower_ct<
                                    mulmod_ct<
                                        base % mod, 
                                        base % mod, 
                                        mod
                                    >(), 
                                    (exp >> 1), 
                                    mod, 
                                    result
                            >();
                    }
                }
            }

            // check_composite_ct_impl: recursivo para Miller-Rabin
            template <uint64_t x, uint64_t n, int s, int r = 1>
            constexpr bool check_composite_ct_impl() {
                if constexpr (r >= s) {
                    return true;
                } else if constexpr (
                    binpower_ct<x, (1ULL << r), n>() == n - 1) {
                    return false;
                } else {
                    return check_composite_ct_impl<x, n, s, r + 1>();
                }
            }

            // check_composite para Miller-Rabin
            template <uint64_t n, uint64_t a, uint64_t d, int s>
            constexpr bool check_composite_ct() {
                constexpr uint64_t x = binpower_ct<a, d, n>();
                if (x == 1 || x == n - 1) {
                    return false;
                }
                return check_composite_ct_impl<x, n, s>();
            }

            // Recursivo constexpr para Miller-Rabin con testigos fijos
            template <uint64_t n, uint64_t d, int s, size_t W = 0>
            constexpr bool miller_rabin_ct() {
                constexpr size_t sz_miller_rabin_witnesses {
                    sizeof(miller_rabin_witnesses) 
                };
                constexpr size_t num_witnesses_ind_0 {
                    sizeof(miller_rabin_witnesses[0]) 
                };
                constexpr size_t total_witnesses {
                    sz_miller_rabin_witnesses / num_witnesses_ind_0 
                };

                if constexpr (W >= total_witnesses) {
                    return true;
                } else if constexpr (miller_rabin_witnesses[W] >= n) {
                    return true;
                } else if constexpr (
                    check_composite_ct<n, miller_rabin_witnesses[W], d, s>()
                ) {
                    return false;
                } else {
                    return miller_rabin_ct<n, d, s, W + 1>();
                }
            }

            // isPrime_ct principal
            template <uint64_t n>
            constexpr bool isPrime_ct() {
                if constexpr (n < 2) {
                    return false;
                }
                if constexpr (n < 65537) {
                    return is_prime_lt_65537_ct<n>();
                }
                if constexpr (divides_by_small_prime_ct<n>()) {
                    return false;
                }
                constexpr auto decomp = decompose_ct<n - 1>();
                return miller_rabin_ct<n, decomp.d, decomp.s>();
            }
        }
    }
}

#endif // NUMREPR_INCLUDE_CORE_INTERNAL_LOOKUP_TABLES_PRIMES_COMPILETIME_HPP
