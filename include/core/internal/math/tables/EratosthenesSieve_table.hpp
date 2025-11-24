#ifndef NUMREPR_INCLUDE_CORE_INTERNAL_MATH_TABLES_ERATOSTHENESSIEVE_TABLE_HPP_INCLUDED
#define NUMREPR_INCLUDE_CORE_INTERNAL_MATH_TABLES_ERATOSTHENESSIEVE_TABLE_HPP_INCLUDED

#include <bitset>
#include <array>
#include <cstddef>

namespace NumRepr {
namespace AuxFunc {
namespace LUT {

// Generador consteval de la criba.
// Calcula los primos < 65537 en tiempo de compilación y devuelve un bitset.
// Esto reemplaza al string literal gigante y es mucho más robusto y rápido de compilar.
consteval std::bitset<65536> generate_prime_lut_ct() {
    std::bitset<65536> bs;
    bs.set(); // Todos true
    bs[0] = 0;
    bs[1] = 0;
    
    // Criba de Eratóstenes simple en CT
    for (std::size_t i = 2; i * i < 65536; ++i) {
        if (bs[i]) {
            for (std::size_t j = i * i; j < 65536; j += i) {
                bs[j] = 0;
            }
        }
    }
    return bs;
}

// La tabla se genera automáticamente al compilar.
constexpr std::bitset<65536> is_prime_lt_65537_lut = generate_prime_lut_ct();

} // namespace LUT
} // namespace AuxFunc
} // namespace NumRepr

#endif // NUMREPR_INCLUDE_CORE_INTERNAL_MATH_TABLES_ERATOSTHENESSIEVE_TABLE_HPP_INCLUDED