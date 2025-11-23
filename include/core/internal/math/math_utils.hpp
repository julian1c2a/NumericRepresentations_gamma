#ifndef NUMREPR_INCLUDE_CORE_INTERNAL_MATH_MATH_UTILS_HPP_INCLUDED
#define NUMREPR_INCLUDE_CORE_INTERNAL_MATH_MATH_UTILS_HPP_INCLUDED

#include "append/integers.hpp"


#include <cstdint>
#include <limits>

namespace NumRepr {
  namespace AuxFunc {
    namespace LUT {
      using std::numeric_limits;
      /**
       * @brief Calcula en tiempo de compilación el exponente máximo 
       * para una base dada.
       */
      template <uint64_t base>
      consteval size_t max_exponent_for_base_ct() noexcept {
        if constexpr (base < 2) return numeric_limits<uint64_t>::max();
        else if constexpr (base == 2) return 63;
        else if constexpr (base == 3) return 40;
        else if constexpr (base == 10) return 19;
        // ... (puedes mantener tu tabla completa de ifs aquí) ...
        else return 1; // Default safe
      }

      /**
       * @brief Calcula en tiempo de ejecución el exponente máximo.
       */
      constexpr size_t max_exponent_for_base(uint64_t base) noexcept {
        if (base < 2) return numeric_limits<uint64_t>::max();
        else if (base == 2) return 63;
        else if (base == 3) return 40;
        else if (base == 10) return 19;
        // ...
        else return 1;
      }
    } // namespace LUT
  } // namespace AuxFunc
} // namespace NumRepr

#endif // NUMREPR_INCLUDE_CORE_INTERNAL_MATH_MATH_UTILS_HPP_INCLUDED
