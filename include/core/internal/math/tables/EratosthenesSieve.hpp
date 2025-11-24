#ifndef NUMREPR_INCLUDE_CORE_INTERNAL_MATH_TABLES_ERATOSTHENES_SIEVE_HPP_INCLUDED
#define NUMREPR_INCLUDE_CORE_INTERNAL_MATH_TABLES_ERATOSTHENES_SIEVE_HPP_INCLUDED

#include "EratosthenesSieve_table.hpp" // Tabla pre-calculada para 65536
#include <bitset>
#include <cstddef>

namespace NumRepr {
namespace AuxFunc {
namespace LUT {

// Tamaño por defecto alineado con la tabla precalculada
constexpr size_t N = 65536;

// --- Criba Genérica en Tiempo de Compilación (C++20 Iterativo) ---
// Reemplaza a toda la maquinaria recursiva anterior (mark_multiples_ct, sieve_step_ct).
// Al ser 'consteval', se garantiza que se ejecuta en compilación.

template <size_t Size>
consteval std::bitset<Size> make_prime_bitset_ct() {
    std::bitset<Size> bs;
    bs.set(); // Inicializar todos a true (1)
    
    if (Size > 0) bs[0] = 0;
    if (Size > 1) bs[1] = 0;
    
    // Algoritmo estándar iterativo (válido en C++20 constexpr/consteval)
    for (size_t i = 2; i * i < Size; ++i) {
        if (bs[i]) {
            for (size_t j = i * i; j < Size; j += i) {
                bs[j] = 0;
            }
        }
    }
    return bs;
}

// --- Tabla principal heredada del lookup generado ---
using ::NumRepr::AuxFunc::LUT::is_prime_lt_65537_lut;

// --- Función Runtime ---
// Genera la criba en tiempo de ejecución.
// Útil para tamaños dinámicos o muy grandes donde compile-time sería muy lento.
inline std::bitset<N> make_prime_bitset() {
    // Si el tamaño coincide con la tabla precalculada, devolvemos una copia directa.
    // Esto es O(N/64) operaciones de copia vs O(N log log N) de cálculo.
    if constexpr (N == 65536) {
        return is_prime_lt_65537_lut;
    } else {
        // Implementación iterativa idéntica a la CT pero en runtime
        std::bitset<N> bs;
        bs.set();
        bs[0] = 0;
        bs[1] = 0;
        for (size_t i = 2; i * i < N; ++i) {
            if (bs[i]) {
                for (size_t j = i * i; j < N; j += i) {
                    bs[j] = false;
                }
            }
        }
        return bs;
    }
}

} // namespace LUT
} // namespace AuxFunc
} // namespace NumRepr

#endif // NUMREPR_INCLUDE_CORE_INTERNAL_MATH_TABLES_ERATOSTHENES_SIEVE_HPP_INCLUDED