
#ifndef NUMREPR_INCLUDE_CORE_INTERNAL_AUX_FUNC_HPP_INCLUDED
#define NUMREPR_INCLUDE_CORE_INTERNAL_AUX_FUNC_HPP_INCLUDED

#include "append/expected.hpp"
#include "append/integers.hpp"

// CORRECCIÓN: Incluir los nuevos módulos matemáticos donde se movió la lógica
#include "math/IntExpIntLog.hpp"
#include "math/IntRoot.hpp"

#include <limits>
#include <type_traits>

namespace NumRepr {
namespace AuxFunc {
using std::numeric_limits;

// El enum math_error_ec ahora vive en IntExpIntLog.hpp, no lo redefinimos aquí.

// --- Logaritmos y Potencias ---
// Las definiciones se han movido a math/IntExpIntLog.hpp. 
// Al incluir ese archivo, las funciones están disponibles en el namespace NumRepr::AuxFunc.

// --- Runtime functions ---
// También movidas a math/IntExpIntLog.hpp

// --- Raíces (Newton-Raphson) ---
// Movidas a math/IntRoot.hpp

// --- Conteo de dígitos y Conversiones ---
// Movidas a math/IntExpIntLog.hpp (conteo) y conversions.hpp (conversiones)

// Mantenemos conversion_to_int_safe aquí solo si no causa conflicto con conversions.hpp.
// Dado el error de redefinición con conversions.hpp, es mejor delegar o eliminar si ya está allá.
// El error original mostraba conflicto en ConversionError y conversion_to_int_safe.
// conversions.hpp es el lugar correcto para esto.

} // namespace AuxFunc
} // namespace NumRepr

#endif // NUMREPR_INCLUDE_CORE_INTERNAL_AUX_FUNC_HPP_INCLUDED