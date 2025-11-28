#ifndef NUMREPR_INCLUDE_CORE_INTERNAL_MATH_MATH_UTILS_HPP_INCLUDED
#define NUMREPR_INCLUDE_CORE_INTERNAL_MATH_MATH_UTILS_HPP_INCLUDED

#include "../append/integers.hpp"
#include "Int_ExpLog.hpp" // Aquí reside ahora la lógica real

#include <cstdint>
#include <limits>

namespace NumRepr {
namespace AuxFunc {
namespace LUT {
    
    // --- WRAPPERS DE COMPATIBILIDAD ---
    // Redirigen las llamadas antiguas a la nueva implementación 
    // segura en IntExpIntLog.hpp
    // Esto permite borrar la lógica duplicada sin romper código existente.

    /**
     * @brief Calcula en tiempo de compilación el exponente máximo 
     * para una base dada.
     * @deprecated Usar 
     * NumRepr::AuxFunc::LUT::Safety::max_exponent_for_base_ct directamente.
     */
    template <uint64_t base>
    consteval size_t max_exponent_for_base_ct() noexcept {
        return NumRepr::AuxFunc::LUT::Safety::max_exponent_for_base_ct<base>();
    }

    /**
     * @brief Calcula en tiempo de ejecución el exponente máximo.
     * @deprecated Usar 
     * NumRepr::AuxFunc::Safety::max_exponent_for_base directamente.
     */
    constexpr size_t max_exponent_for_base(uint64_t base) noexcept {
        return NumRepr::AuxFunc::LUT::Safety::max_exponent_for_base(base);
    }

} // namespace LUT
} // namespace AuxFunc
} // namespace NumRepr

#endif // NUMREPR_INCLUDE_CORE_INTERNAL_MATH_MATH_UTILS_HPP_INCLUDED