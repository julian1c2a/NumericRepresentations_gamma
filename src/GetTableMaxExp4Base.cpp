
#include "../include/core/internal/append/integers.hpp" // Para uint128_t
#include "../include/core/internal/math/IntExpIntLog.hpp" // Para int_pow

#include <fstream>
#include <iostream>
#include <utility>
#include <type_traits>
#include <limits>
#include <concepts>
#include <vector>
#include <iomanip>

// Usamos namespace para evitar conflictos, aunque sea un .cpp
using namespace NumRepr::AuxFunc;

// Helper para calcular raíz cuadrada entera redondeada hacia arriba
// (Duplicado localmente si no está disponible en headers accesibles desde src)
template <typename T>
constexpr T local_ceilsqrt(T n) {
    if (n <= 1) return n;
    T x0 = n / 2;
    T x1 = (x0 + n / x0) / 2;
    while (x1 < x0) {
        x0 = x1;
        x1 = (x0 + n / x0) / 2;
    }
    return (x0 * x0 == n) ? x0 : x0 + 1;
}

// Dada la base y el tipo de entero, calcula el exponente máximo seguro
template <std::integral TInt>
size_t compute_max_exponent_for_base(TInt base) noexcept {
    if (base == 0) return std::numeric_limits<size_t>::max();
    if (base == 1) return std::numeric_limits<size_t>::max();
    
    // Límite máximo representable
    constexpr TInt max_val = std::numeric_limits<TInt>::max();
    
    size_t exp = 1;
    TInt current = base;
    
    // Buscamos el exponente tal que current * base <= max_val
    // Comprobamos overflow antes de multiplicar: current <= max_val / base
    while (current <= max_val / base) {
        current *= base;
        ++exp;
    }

    return exp;
}

// Generamos la tabla comprimida (solo guardamos cuando el exponente cambia)
template <std::integral TInt>
std::vector<std::pair<TInt,size_t>> generate_max_exponent_table() noexcept {
    std::vector<std::pair<TInt,size_t>> table;
    
    // Casos especiales
    table.emplace_back(0, std::numeric_limits<size_t>::max());
    table.emplace_back(1, std::numeric_limits<size_t>::max());

    // Empezamos desde 2
    TInt base = 2;
    
    // Calculamos hasta sqrt(MAX), ya que a partir de ahí el exponente máximo es 1
    const TInt max_base_to_check = local_ceilsqrt(std::numeric_limits<TInt>::max());
    
    size_t current_max_exp = compute_max_exponent_for_base<TInt>(base);
    table.emplace_back(base, current_max_exp);

    // Iteramos inteligentemente
    // En lugar de ++base, podríamos buscar binariamente el cambio, 
    // pero dado que la base crece exponencialmente respecto al decremento del exp,
    // un bucle lineal saltando bases con el mismo exponente es aceptable si se optimiza,
    // pero para este generador simple, iteramos y guardamos solo cambios.
    
    // Optimización: Iteramos todos, pero solo guardamos si cambia.
    // Para bases grandes, el exponente no cambia en mucho tiempo.
    
    while (base < max_base_to_check) {
        ++base;
        size_t new_exp = compute_max_exponent_for_base<TInt>(base);
        if (new_exp != current_max_exp) {
            current_max_exp = new_exp;
            table.emplace_back(base, current_max_exp);
        }
    }
    
    // Entrada final para bases que solo soportan exponente 1
    table.emplace_back(max_base_to_check + 1, 1);
    
    return table;
}

int main() {
    // Generar para uint64_t
    const auto vector_limits = generate_max_exponent_table<uint64_t>();

    std::ofstream out("MaxExp4Base_lookup.hpp");
    if (!out) {
        std::cerr << "Error creando el archivo.\n";
        return 1;
    }

    out << "#ifndef NUMREPR_INCLUDE_CORE_INTERNAL_MATH_TABLES_MAXEXP4BASE_LOOKUP_HPP_INCLUDED\n";
    out << "#define NUMREPR_INCLUDE_CORE_INTERNAL_MATH_TABLES_MAXEXP4BASE_LOOKUP_HPP_INCLUDED\n\n";
    out << "#include <array>\n";
    out << "#include <cstdint>\n";
    out << "#include <utility>\n\n";
    out << "namespace NumRepr {\n";
    out << "namespace AuxFunc {\n";
    out << "namespace LUT {\n\n";
    
    out << "/**\n";
    out << " * @brief Tabla generada de exponentes máximos para uint64_t.\n";
    out << " */\n";
    out << "constexpr std::array<std::pair<uint64_t, uint64_t>, " 
        << vector_limits.size() 
        << "> base_maxexp_table = {{\n";
    
    for (size_t i = 0; i < vector_limits.size(); ++i) {
        const auto& [base, maxexp] = vector_limits[i];
        out << "    {" << base << "ull, ";
        if (maxexp == std::numeric_limits<size_t>::max()) {
            out << "~0ull";
        } else {
            out << maxexp;
        }
        out << "}";
        if (i < vector_limits.size() - 1) out << ",";
        out << "\n";
    }
    
    out << "}};\n\n";
    out << "} // namespace LUT\n";
    out << "} // namespace AuxFunc\n";
    out << "} // namespace NumRepr\n\n";
    out << "#endif // NUMREPR_INCLUDE_CORE_INTERNAL_MATH_TABLES_MAXEXP4BASE_LOOKUP_HPP_INCLUDED\n";
    
    out.close();
    std::cout << "Header MaxExp4Base_lookup.hpp generado correctamente (" << vector_limits.size() << " entradas).\n";
    return 0;
}