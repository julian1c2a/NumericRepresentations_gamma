/**
 * @file core/dig_t_display_helpers.hpp
 * @brief Funciones helper para visualización correcta de dig_t
 *
 * Resuelve el problema de uint8_t siendo interpretado como carácter ASCII
 * en lugar de número cuando se usa con std::cout.
 */

#ifndef DIG_T_DISPLAY_HELPERS_HPP
#define DIG_T_DISPLAY_HELPERS_HPP

#include <iostream>
#include <type_traits>
#include "dig_t.hpp"

namespace NumRepr
{
    namespace display_helpers
    {

        template <unsigned Base>
        constexpr auto display(const dig_t<Base> &digit) noexcept
        {
            using uint_t = typename dig_t<Base>::uint_t;

            if constexpr (std::is_same_v<uint_t, uint8_t>)
            {
                return static_cast<int>(digit.get());
            }
            else
            {
                return digit.get();
            }
        }

        template <unsigned Base>
        std::ostream &operator<<(std::ostream &os, const dig_t<Base> &digit)
        {
            return os << display(digit);
        }

        template <unsigned Base>
        constexpr const char *type_name() noexcept
        {
            using uint_t = typename dig_t<Base>::uint_t;

            if constexpr (std::is_same_v<uint_t, uint8_t>)
            {
                return "uint8_t";
            }
            else if constexpr (std::is_same_v<uint_t, uint16_t>)
            {
                return "uint16_t";
            }
            else if constexpr (std::is_same_v<uint_t, uint32_t>)
            {
                return "uint32_t";
            }
            else if constexpr (std::is_same_v<uint_t, uint64_t>)
            {
                return "uint64_t";
            }
            else
            {
                return "unknown";
            }
        }

    } // namespace display_helpers

    using display_helpers::display;
    using display_helpers::type_name;

} // namespace NumRepr

#endif // DIG_T_DISPLAY_HELPERS_HPP
