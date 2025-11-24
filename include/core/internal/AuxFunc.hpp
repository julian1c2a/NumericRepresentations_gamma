
#ifndef NUMREPR_INCLUDE_CORE_INTERNAL_AUX_FUNC_HPP_INCLUDED
#define NUMREPR_INCLUDE_CORE_INTERNAL_AUX_FUNC_HPP_INCLUDED

#include "append/expected.hpp"
#include "append/integers.hpp"

// Incluimos los nuevos módulos donde ahora vive la lógica real
#include "math/IntExpIntLog.hpp"
#include "math/IntRoot.hpp"
#include "conversions.hpp" // Para conversion_to_int_safe

#include <limits>
#include <type_traits>

namespace NumRepr {
namespace AuxFunc {
    // Este namespace ahora se puebla automáticamente al incluir los headers de arriba.
    // No redefinimos nada aquí para evitar errores de "multiple definition".
    
    using std::numeric_limits;
    using LUT::isPrime;
    using LUT::isPrime_ct;

} // namespace AuxFunc
} // namespace NumRepr

#endif // NUMREPR_INCLUDE_CORE_INTERNAL_AUX_FUNC_HPP_INCLUDED