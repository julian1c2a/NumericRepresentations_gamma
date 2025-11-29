#ifndef NUMREPR_INCLUDE_CORE_INTERNAL_MATH_PRIMES_CT_HPP_INCLUDED
#define NUMREPR_INCLUDE_CORE_INTERNAL_MATH_PRIMES_CT_HPP_INCLUDED

#include "tables/PrimeList.hpp"
#include "tables/EratosthenesSieve.hpp"

#include <cstdint>
#include <array>
#include <algorithm>
#include <concepts>
#include <type_traits>
#include "core/internal/append/int128_ops.hpp"

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
			template <  uint64_t value, 
						size_t left = 0, 
						size_t right = primes_lt_65537.size()
			> struct binary_search_constexpr_impl {
				static constexpr size_t mid { left + (right - left) / 2 };
				static constexpr uint16_t mid_value { primes_lt_65537[mid] };
				static constexpr bool result { 
					(mid_value == value)
						? true
						:   (	mid_value < value	)		?
						    ( binary_search_constexpr_impl<
									value, 
									mid + 1, 
									right
								>::result
							)								: 
						  	( binary_search_constexpr_impl<
									value, 
									left, 
									mid
								>::result
							)  
				}; // end constexpr result
			}; // end struct binary_search_constexpr_impl
			template <uint64_t value, size_t pos>
			struct binary_search_constexpr_impl<value, pos, pos> {
				static constexpr bool result = false;
			};
			template <uint64_t value>
			consteval bool binary_search_constexpr() {
				return binary_search_constexpr_impl<value>::result;
			}

			// --- Primalidad para n < 65537 ---
			// Compile-time (template, para static_assert)
			template <uint64_t value>
			constexpr bool is_prime_lt_65537_ct() {
				if constexpr (value < 2 || value >= 65536) {
					return false;
				}
				return is_prime_lt_65537_lut[value];
			}

			//------------------------------------------------------------------
			// TEST DE MILLER-RABIN
			//------------------------------------------------------------------

			// --- Miller-Rabin determinista constexpr para n < 2^64 ---
			// Testigos fijos recomendados para n < 2^64.
            // Usamos std::array para mayor seguridad y métodos constexpr.
			static constexpr array<uint64_t, 25> miller_rabin_witnesses {{
				2, 3, 5, 7, 11, 13, 17,
				19, 23, 29, 31, 37, 41, 43,
				47, 53, 59, 61, 67, 71, 73,
				79, 83, 89, 97
            }};

			// Tipo para la descomposición MILLER-RABIN n-1 = d*2^s
			struct DecompResult
			{
				uint64_t d;
				int64_t s;
			};

			// Descomponer n-1 = d*2^s
			template <uint64_t n_minus_1, int s = 0>
			consteval DecompResult decompose_ct()
			{
				if constexpr (n_minus_1 & 1) {
					return DecompResult{n_minus_1, s};
				} else {
					return decompose_ct<(n_minus_1 >> 1), s + 1>();
				}
			}
			
			// Recursivo consteval para probar divisibilidad por primos pequeños
			consteval bool divides_by_small_prime_ct(uint64_t n) {
        		for (const auto& p : primes_lt_65537) {
            	// Optimización: Si p*p > n, ya no necesitamos comprobar más
            	// Cast a 128 bit para evitar overflow en p*p
            		if (static_cast<unsigned __int128>(p) * p > n) { return false; }
            		if (n % p == 0) { return true; }
        		}
        		return false;
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
				if constexpr (b == 0) {
					return result;
				} else {
					return	mulmod_ct_impl<new_a, new_b, mod, new_result>();
				}
			}

			template <uint64_t a, uint64_t b, uint64_t mod>
			constexpr uint64_t mulmod_ct() {
				return mulmod_ct_impl<a % mod, b, mod, 0>();
			}

			// binpower recursivo para Miller-Rabin
			template <  uint64_t base, 
						uint64_t exp, 
						uint64_t mod, 
						uint64_t result = 1
			> consteval uint64_t binpower_ct() {
				if constexpr (exp == 0) {
					return result;
				}	else	{
					if constexpr (exp & 1)	{
						return (
							binpower_ct<
								base, 
								(exp >> 1), 
								mod, 
								mulmod_ct<
									result, 
									base % mod, 
									mod
								>()
							>()
						);
					}	else	{
						return (
							binpower_ct<
								mulmod_ct<
									base % mod, 
									base % mod, 
									mod
								>(), 
								(exp >> 1), 
								mod, 
								result
							>()
						);
					}
				}
			}

			// check_composite_ct_impl: recursivo para Miller-Rabin
			template <uint64_t x, uint64_t n, int s, int r = 1>
			constexpr bool check_composite_ct_impl() {
				if constexpr (r >= s) {
					return true;
				} else if constexpr (binpower_ct<x, (1ULL << r), n>() == n - 1) {
					return false;
				} else {
					return check_composite_ct_impl<x, n, s, r + 1>();
				}
			}

			// check_composite para Miller-Rabin
			template <uint64_t n, uint64_t a, uint64_t d, int s>
			constexpr bool check_composite_ct()
			{
				constexpr uint64_t x { binpower_ct<a, d, n>() };
				if (x == 1 || x == n - 1) {
					return false;
				}
				return check_composite_ct_impl<x, n, s>();
			}

			// Recursivo constexpr para Miller-Rabin con testigos fijos
			template <uint64_t n, uint64_t d, int s, size_t W = 0>
			constexpr bool miller_rabin_ct() {
                // SIMPLIFICACIÓN: Usamos .size() directamente gracias a std::array
				constexpr size_t total_witnesses { miller_rabin_witnesses.size() };
                
				// CAMBIO CRÍTICO: Validar límite ANTES de instanciar check_composite_ct
                // Esto previene la recursión infinita y el acceso fuera de límites
				if constexpr (W >= total_witnesses)	{
					return true;
				} else {
					// Ahora es seguro acceder a miller_rabin_witnesses[W]
					if constexpr (miller_rabin_witnesses[W] >= n)	{
						return true;
					} else	{
						// Instanciar check_composite SOLO si estamos en rango y es necesario
						constexpr bool composite {
							check_composite_ct<
								n, 
								miller_rabin_witnesses[W], 
								d, 
								s
							>()
						};

						if constexpr (composite) {
							return false;
						} else {
							return miller_rabin_ct<n, d, s, W + 1>();
						}
					}
				}
			}

			// isPrime_ct principal
			template <uint64_t n>
			constexpr bool isPrime_ct()
			{
				if constexpr (n < 2) {
					return false;
				} else if constexpr (n < 65537) {
					return is_prime_lt_65537_ct<n>();
				} else if constexpr (divides_by_small_prime_ct(n)) {
					return false;
				} else {
					constexpr auto decomp { decompose_ct<n - 1>() };
					return miller_rabin_ct<n, decomp.d, decomp.s>();
				}
			}
		}
	}
}

#endif // NUMREPR_INCLUDE_CORE_INTERNAL_MATH_PRIMES_CT_HPP_INCLUDED