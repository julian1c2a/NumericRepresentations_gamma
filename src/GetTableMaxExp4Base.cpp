

#include "core/internal/append/integers.hpp" // Para uint128_t
#include "core/internal/math/IntExpIntLog.hpp" // Para int_pow


#include <fstream>
#include <iostream>
#include <utility>
#include <type_traits>
#include <limits>
#include <concepts>
#include <vector>

using namespace NumRepr::AuxFunc;

// Dada la base, el tipo de entero y un exponente exp, 
// nos dice si base^(exp-2) < numeric_limits<T>::max()/(base^2)
// a la vez que base^(exp-1) >= numeric_limits<T>::max()/base

template <std::integral TInt>
constexpr bool is_max_exponent_for_base(TInt base, size_t exp) noexcept {
    if (base == 0) return false;
    if (base == 1) return false;
    if (base == 2) return exp == 63;
    if (base == 3) return exp == 40;
    
    const uint64_t limit_exp_ant {
        std::numeric_limits<TInt>::max() / (base * base)
    };
    const uint64_t limit_exp {
        limit_exp_ant * base
    };
    
    uint64_t base2exp_ant {int_pow(base, exp-2)};
    uint64_t base2exp {base2exp_ant * base};
    
    if( base2exp_ant < limit_exp_ant &&
        base2exp < limit_exp) {
        return false;
    } else {
        return true;
    }
}

// Dada la base y el tipo de entero de la base, 
// nos da el exponente exp tal que, 
//      base^(exp-2) < numeric_limits<T>::max()/(base^2) &&
//      base^(exp-1) >= numeric_limits<T>::max()/base
template <std::integral TInt>
size_t compute_max_exponent_for_base(TInt base) noexcept {
    if (base == 0) return std::numeric_limits<size_t>::max();
    if (base == 1) return std::numeric_limits<size_t>::max();
    if (base == 2) return 63;
    if (base == 3) return 40;
    
    const uint64_t limit_exp_ant {
        std::numeric_limits<TInt>::max() / (base * base)
    };
    const uint64_t limit_exp {
        limit_exp_ant * base
    };
    
    size_t exp = 2;

    uint64_t base2exp_ant {1ull};
    uint64_t base2exp {base};

    while (!(   
        base2exp_ant < limit_exp_ant 
        &&
        base2exp < limit_exp)
        ) {
        ++exp;
        base2exp_ant *= base;
        base2exp *= base;
    }

    return exp;
}

// Dada la base y el tipo de entero de la base, 
// nos dice si la siguiente base da el mismo exponente máximo o no, 
//      ( base^(exp-2) < numeric_limits<T>::max()/(base^2) &&
//        base^(exp-1) >= numeric_limits<T>::max()/base      ) ==
//      ( (base+1)^(exp-2) < numeric_limits<T>::max()/((base+1)^2) &&
//        (base+1)^(exp-1) >= numeric_limits<T>::max()/(base+1)      )
template <std::integral TInt>
bool next_base_same_max_exponent(TInt base) noexcept {
    if (base == 0) return true;
    if (base == 1) return false;
    if (base == 2) return false;
    if (base == 3) return false;
    

    // Límites para la siguiente base
    const uint64_t limit_exp_ant_next_base {
        std::numeric_limits<TInt>::max() / ((base+1) * (base+1))
    };
    
    const uint64_t limit_exp_next_base {
        limit_exp_ant_next_base * (base+1)
    };
    
    // Inicialización de la constante max_exp
    const size_t max_exp { compute_max_exponent_for_base<TInt>(base) };
    
    // Cálculo de las potencias para la siguiente base
    uint64_t base2exp_ant_next { int_pow(base + 1, max_exp - 2) };
    uint64_t base2exp_next { base2exp_ant_next * (base + 1) };
    
    const bool next_base_condition {
        base2exp_ant_next < limit_exp_ant_next &&
        base2exp_next >= limit_exp_next
    };
    
    return next_base_condition;
}

// Generamos los límites para las bases desde 0 hasta
// floor(sqrt(numeric_limits<TInt>::max()))
template <std::integral TInt>
std::vector<std::pair<TInt,size_t>> generate_max_exponent_table() noexcept {
    std::vector<std::pair<TInt,size_t>> table;
    
    const TInt max_base {
        static_cast<TInt>(
            ceilsqrt(
                static_cast<
                    typename std::make_unsigned<TInt>::type
                >(std::numeric_limits<TInt>::max())
            )
        )
    };
    
    TInt base = 0;
    while (base <= max_base) {
        const size_t max_exp = compute_max_exponent_for_base<TInt>(base);
        table.emplace_back(base, max_exp);
        
        // Avanzamos a la siguiente base que cambie el exponente máximo
        do {
            ++base;
        } while (   (next_base_same_max_exponent<TInt>(base - 1)) && 
                    (base <= max_base)                             );
    }
    
    return table;
}

// Write the bitset as a C++ header
int main() {
    
    const std::vector<std::pair<uint64_t,size_t>> vector_limits {
        generate_max_exponent_table<uint64_t>()
    };

    std::ofstream out("MaxExp4Base_lookup.hpp");
    out << "#pragma once\n";
    out << "#include <array>\n\n";
    out << "constexpr std::array<std::pair<uint64_t,size_t>, " 
        << vector_limits.size() 
        << "> base_maxexp_table = {\n";
    size_t count{0};
    for (auto& [base, maxexp]: vector_limits) {
        out << "    {" << base << "ull, " << maxexp << "},";
        if (count > 0 && (count % 8 == 0)) {
            out << "\n";
        }
        ++count;
    }
    out << "};\n";
    out.close();
    std::cout << "Header MaxExp4Base_lookup.hpp generado.\n";
    return 0;
}
