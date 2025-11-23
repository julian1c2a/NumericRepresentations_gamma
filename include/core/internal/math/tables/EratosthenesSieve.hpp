#ifndef NUMREPR_INCLUDE_CORE_INTERNAL_MATH_TABLES_ERATOSTHENES_SIEVE_HPP_INCLUDED
#define NUMREPR_INCLUDE_CORE_INTERNAL_MATH_TABLES_ERATOSTHENES_SIEVE_HPP_INCLUDED

#include "core/internal/math/tables/EratosthenesSieve_table.hpp"
#include <bitset>
#include <cstddef>

namespace NumRepr {
namespace AuxFunc {
namespace LUT {

constexpr size_t N = 65536;

// --- Criba de Eratóstenes completamente recursiva constexpr (plantillas) ---
template <size_t N, size_t I, size_t J, typename Bitset>
constexpr Bitset mark_multiples_ct(Bitset bs) {
	if constexpr (J < N) {
		bs[J] = false;
		return mark_multiples_ct<N, I, J + I, Bitset>(bs);
	} else {
		return bs;
	}
}

template <size_t N, size_t I, typename Bitset>
constexpr Bitset sieve_step_ct(Bitset bs) {
	if constexpr (I * I < N) {
		if (bs[I]) {
			bs = mark_multiples_ct<N, I, I * I, Bitset>(bs);
		}
		return sieve_step_ct<N, I + 1, Bitset>(bs);
	} else {
		return bs;
	}
}

template <size_t N>
constexpr std::bitset<N> make_prime_bitset_ct() {
	std::bitset<N> bs{};
	bs.set();
	bs[0] = false;
	bs[1] = false;
	return sieve_step_ct<N, 2, std::bitset<N>>(bs);
}

// --- Tabla principal heredada del lookup generado offline ---
using ::NumRepr::AuxFunc::LUT::is_prime_lt_65537_lut;

// --- Función iterativa (opcional, no usada por defecto) ---
inline std::bitset<N> make_prime_bitset() {
	std::bitset<N> is_prime{};
	is_prime.set(); // Marca todos como verdaderos
	is_prime[0] = false;
	is_prime[1] = false;
	for (size_t i = 2; i * i < N; ++i) {
		if (is_prime[i]) {
			for (size_t j = i * i; j < N; j += i) {
				is_prime[j] = false;
			}
		}
	}
	return is_prime;
}

} // namespace LUT
} // namespace AuxFunc
} // namespace NumRepr

#endif // NUMREPR_INCLUDE_CORE_INTERNAL_MATH_TABLES_ERATOSTHENES_SIEVE_HPP_INCLUDED
