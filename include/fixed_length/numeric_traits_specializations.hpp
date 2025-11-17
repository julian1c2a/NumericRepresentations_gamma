/**
 * @file numeric_traits_specializations.hpp
 * @brief Especializaciones de traits estándar para nat_reg_digs_t e int_reg_digs_t
 *
 * Este archivo implementa las especializaciones necesarias para hacer que nuestros
 * tipos numéricos sean completamente compatibles con el ecosistema de C++ estándar,
 * incluyendo std::numeric_limits, type traits, hash, y otras facilidades.
 *
 * @author Julia
 * @date 6 de noviembre de 2025
 */

#ifndef NUMERIC_TRAITS_SPECIALIZATIONS_HPP
#define NUMERIC_TRAITS_SPECIALIZATIONS_HPP

#include <limits>
#include <type_traits>
#include <functional>
#include <iterator>
#include <cmath>
#include <bit>

#include "nat_reg_digs_t.hpp"
#include "int_reg_digs_t.hpp"

/************************************************
 *
 * STD::NUMERIC_LIMITS ESPECIALIZACIONES
 *
 ************************************************/

namespace std
{

    // ========================================
    // NUMERIC_LIMITS para nat_reg_digs_t
    // ========================================

    template <uint64_t B, size_t L>
    class numeric_limits<NumRepr::nat_reg_digs_t<B, L>>
    {
    private:
        using value_type = NumRepr::nat_reg_digs_t<B, L>;

        // Calcular número de bits representables
        static constexpr int calculate_digits() noexcept
        {
            // Para base B y L dígitos: aproximación constexpr
            return (B == 2) ? L : (B == 10) ? static_cast<int>(L * 3)
                                            : static_cast<int>(L * 2);
        }

        static constexpr int calculate_digits10() noexcept
        {
            // Para base B y L dígitos: aproximación constexpr
            return (B == 10) ? L : (B == 2) ? static_cast<int>(L / 3)
                                            : static_cast<int>(L / 2);
        }

    public:
        static constexpr bool is_specialized = true;
        static constexpr bool is_signed = false;
        static constexpr bool is_integer = true;
        static constexpr bool is_exact = true;
        static constexpr bool has_infinity = false;
        static constexpr bool has_quiet_NaN = false;
        static constexpr bool has_signaling_NaN = false;
        static constexpr float_denorm_style has_denorm = denorm_absent;
        static constexpr bool has_denorm_loss = false;
        static constexpr float_round_style round_style = round_toward_zero;
        static constexpr bool is_iec559 = false;
        static constexpr bool is_bounded = true;
        static constexpr bool is_modulo = true;

        static constexpr int digits = calculate_digits();
        static constexpr int digits10 = calculate_digits10();
        static constexpr int max_digits10 = 0;
        static constexpr int radix = static_cast<int>(B);

        static constexpr int min_exponent = 0;
        static constexpr int min_exponent10 = 0;
        static constexpr int max_exponent = 0;
        static constexpr int max_exponent10 = 0;

        static constexpr bool traps = false;
        static constexpr bool tinyness_before = false;

        // Valores límite
        static constexpr value_type min() noexcept
        {
            return value_type::regd_0(); // Mínimo es 0
        }

        static constexpr value_type lowest() noexcept
        {
            return min(); // Para unsigned, lowest == min
        }

        static constexpr value_type max() noexcept
        {
            value_type result;
            for (size_t i = 0; i < L; ++i)
            {
                result[i] = value_type::dig_Bm1(); // Todos los dígitos al máximo
            }
            return result;
        }

        static constexpr value_type epsilon() noexcept
        {
            return value_type::regd_1(); // Para enteros, epsilon es 1
        }

        static constexpr value_type round_error() noexcept
        {
            return value_type::regd_0(); // No hay error de redondeo para enteros
        }

        static constexpr value_type infinity() noexcept
        {
            return value_type::regd_0(); // No hay infinito
        }

        static constexpr value_type quiet_NaN() noexcept
        {
            return value_type::regd_0(); // No hay NaN
        }

        static constexpr value_type signaling_NaN() noexcept
        {
            return value_type::regd_0(); // No hay NaN
        }

        static constexpr value_type denorm_min() noexcept
        {
            return min(); // Mismo que min para enteros
        }
    };

    // ========================================
    // NUMERIC_LIMITS para int_reg_digs_t
    // ========================================

    template <uint64_t B, size_t R>
    class numeric_limits<NumRepr::int_reg_digs_t<B, R>>
    {
    private:
        using value_type = NumRepr::int_reg_digs_t<B, R>;

        static constexpr int calculate_digits() noexcept
        {
            // Para base B y R dígitos sin signo: aproximación constexpr
            return (B == 2) ? R : (B == 10) ? static_cast<int>(R * 3)
                                            : static_cast<int>(R * 2);
        }

        static constexpr int calculate_digits10() noexcept
        {
            return (B == 10) ? R : (B == 2) ? static_cast<int>(R / 3)
                                            : static_cast<int>(R / 2);
        }

    public:
        static constexpr bool is_specialized = true;
        static constexpr bool is_signed = true;
        static constexpr bool is_integer = true;
        static constexpr bool is_exact = true;
        static constexpr bool has_infinity = false;
        static constexpr bool has_quiet_NaN = false;
        static constexpr bool has_signaling_NaN = false;
        static constexpr float_denorm_style has_denorm = denorm_absent;
        static constexpr bool has_denorm_loss = false;
        static constexpr float_round_style round_style = round_toward_zero;
        static constexpr bool is_iec559 = false;
        static constexpr bool is_bounded = true;
        static constexpr bool is_modulo = false; // Signed no es modular

        static constexpr int digits = calculate_digits();
        static constexpr int digits10 = calculate_digits10();
        static constexpr int max_digits10 = 0;
        static constexpr int radix = static_cast<int>(B);

        static constexpr int min_exponent = 0;
        static constexpr int min_exponent10 = 0;
        static constexpr int max_exponent = 0;
        static constexpr int max_exponent10 = 0;

        static constexpr bool traps = false;
        static constexpr bool tinyness_before = false;

        // Valores límite
        static constexpr value_type min() noexcept
        {
            return value_type::min_value();
        }

        static constexpr value_type lowest() noexcept
        {
            return min(); // Para enteros, lowest == min
        }

        static constexpr value_type max() noexcept
        {
            return value_type::max_value();
        }

        static constexpr value_type epsilon() noexcept
        {
            return value_type::sregd_1(); // Para enteros, epsilon es 1
        }

        static constexpr value_type round_error() noexcept
        {
            return value_type::sregd_0(); // No hay error de redondeo para enteros
        }

        static constexpr value_type infinity() noexcept
        {
            return value_type::sregd_0(); // No hay infinito
        }

        static constexpr value_type quiet_NaN() noexcept
        {
            return value_type::sregd_0(); // No hay NaN
        }

        static constexpr value_type signaling_NaN() noexcept
        {
            return value_type::sregd_0(); // No hay NaN
        }

        static constexpr value_type denorm_min() noexcept
        {
            return min(); // Mismo que min para enteros
        }
    };

} // namespace std

/************************************************
 *
 * TYPE TRAITS FUNDAMENTALES
 *
 ************************************************/

namespace std
{

    // ========================================
    // TYPE TRAITS para nat_reg_digs_t
    // ========================================

    template <uint64_t B, size_t L>
    struct is_arithmetic<NumRepr::nat_reg_digs_t<B, L>> : true_type
    {
    };

    template <uint64_t B, size_t L>
    struct is_integral<NumRepr::nat_reg_digs_t<B, L>> : true_type
    {
    };

    template <uint64_t B, size_t L>
    struct is_signed<NumRepr::nat_reg_digs_t<B, L>> : false_type
    {
    };

    template <uint64_t B, size_t L>
    struct is_unsigned<NumRepr::nat_reg_digs_t<B, L>> : true_type
    {
    };

    template <uint64_t B, size_t L>
    struct is_fundamental<NumRepr::nat_reg_digs_t<B, L>> : false_type
    {
    };

    template <uint64_t B, size_t L>
    struct is_scalar<NumRepr::nat_reg_digs_t<B, L>> : true_type
    {
    };

    // ========================================
    // TYPE TRAITS para int_reg_digs_t
    // ========================================

    template <uint64_t B, size_t R>
    struct is_arithmetic<NumRepr::int_reg_digs_t<B, R>> : true_type
    {
    };

    template <uint64_t B, size_t R>
    struct is_integral<NumRepr::int_reg_digs_t<B, R>> : true_type
    {
    };

    template <uint64_t B, size_t R>
    struct is_signed<NumRepr::int_reg_digs_t<B, R>> : true_type
    {
    };

    template <uint64_t B, size_t R>
    struct is_unsigned<NumRepr::int_reg_digs_t<B, R>> : false_type
    {
    };

    template <uint64_t B, size_t R>
    struct is_fundamental<NumRepr::int_reg_digs_t<B, R>> : false_type
    {
    };

    template <uint64_t B, size_t R>
    struct is_scalar<NumRepr::int_reg_digs_t<B, R>> : true_type
    {
    };

} // namespace std

/************************************************
 *
 * STD::HASH ESPECIALIZACIONES
 *
 ************************************************/

namespace std
{

    // ========================================
    // HASH para nat_reg_digs_t
    // ========================================

    template <uint64_t B, size_t L>
    struct hash<NumRepr::nat_reg_digs_t<B, L>>
    {
        size_t operator()(const NumRepr::nat_reg_digs_t<B, L> &obj) const noexcept
        {
            size_t seed = 0;

            // Combinar hashes de todos los dígitos
            for (size_t i = 0; i < L; ++i)
            {
                auto digit_value = obj[i].get(); // Obtener valor uint_t del dígito

                // Algoritmo de combinación de hash boost-style
                seed ^= hash<uint64_t>{}(digit_value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }

            // Incluir la base y tamaño en el hash para diferenciación
            seed ^= hash<uint64_t>{}(B) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= hash<size_t>{}(L) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

            return seed;
        }
    };

    // ========================================
    // HASH para int_reg_digs_t
    // ========================================

    template <uint64_t B, size_t R>
    struct hash<NumRepr::int_reg_digs_t<B, R>>
    {
        size_t operator()(const NumRepr::int_reg_digs_t<B, R> &obj) const noexcept
        {
            size_t seed = 0;

            // Combinar hashes de todos los dígitos usando el valor uint del dígito
            for (size_t i = 0; i <= R; ++i)
            {
                auto digit_value = obj[i].get(); // Obtener valor uint_t del dígito
                seed ^= hash<uint64_t>{}(digit_value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }

            // Incluir la base y tamaño
            seed ^= hash<uint64_t>{}(B) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= hash<size_t>{}(R) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

            return seed;
        }
    };

} // namespace std

/************************************************
 *
 * CONVERSION TRAITS
 *
 ************************************************/

namespace std
{

    // ========================================
    // MAKE_SIGNED/MAKE_UNSIGNED
    // ========================================

    template <uint64_t B, size_t L>
    struct make_signed<NumRepr::nat_reg_digs_t<B, L>>
    {
        // nat_reg_digs_t<B,L> -> int_reg_digs_t<B,L-1>
        using type = NumRepr::int_reg_digs_t<B, L - 1>;
    };

    template <uint64_t B, size_t R>
    struct make_unsigned<NumRepr::int_reg_digs_t<B, R>>
    {
        // int_reg_digs_t<B,R> -> nat_reg_digs_t<B,R+1>
        using type = NumRepr::nat_reg_digs_t<B, R + 1>;
    };

    // ========================================
    // COMMON_TYPE para operaciones mixtas
    // ========================================

    template <uint64_t B1, size_t L1, uint64_t B2, size_t L2>
    struct common_type<NumRepr::nat_reg_digs_t<B1, L1>,
                       NumRepr::nat_reg_digs_t<B2, L2>>
    {
        // Solo permitir mismo base por ahora
        static_assert(B1 == B2, "Different bases not supported for common_type");

        // Usar el tamaño mayor
        static constexpr size_t common_length = (L1 > L2) ? L1 : L2;
        using type = NumRepr::nat_reg_digs_t<B1, common_length>;
    };

    template <uint64_t B1, size_t R1, uint64_t B2, size_t R2>
    struct common_type<NumRepr::int_reg_digs_t<B1, R1>,
                       NumRepr::int_reg_digs_t<B2, R2>>
    {
        static_assert(B1 == B2, "Different bases not supported for common_type");

        static constexpr size_t common_length = (R1 > R2) ? R1 : R2;
        using type = NumRepr::int_reg_digs_t<B1, common_length>;
    };

    // Mezcla entre signed y unsigned -> usar signed con tamaño apropiado
    template <uint64_t B1, size_t L1, uint64_t B2, size_t R2>
    struct common_type<NumRepr::nat_reg_digs_t<B1, L1>,
                       NumRepr::int_reg_digs_t<B2, R2>>
    {
        static_assert(B1 == B2, "Different bases not supported for common_type");

        static constexpr size_t common_length = (L1 > R2) ? L1 - 1 : R2;
        using type = NumRepr::int_reg_digs_t<B1, common_length>;
    };

    template <uint64_t B1, size_t R1, uint64_t B2, size_t L2>
    struct common_type<NumRepr::int_reg_digs_t<B1, R1>,
                       NumRepr::nat_reg_digs_t<B2, L2>>
    {
        static_assert(B1 == B2, "Different bases not supported for common_type");

        static constexpr size_t common_length = (R1 > L2 - 1) ? R1 : L2 - 1;
        using type = NumRepr::int_reg_digs_t<B1, common_length>;
    };

} // namespace std

/************************************************
 *
 * ALGORITHM SUPPORT
 *
 ************************************************/

namespace std
{

    // ========================================
    // SWAP ESPECIALIZACIONES
    // ========================================

    template <uint64_t B, size_t L>
    void swap(NumRepr::nat_reg_digs_t<B, L> &a,
              NumRepr::nat_reg_digs_t<B, L> &b) noexcept
    {
        a.swap(b);
    }

    template <uint64_t B, size_t R>
    void swap(NumRepr::int_reg_digs_t<B, R> &a,
              NumRepr::int_reg_digs_t<B, R> &b) noexcept
    {
        a.swap(b);
    }

} // namespace std

#endif // NUMERIC_TRAITS_SPECIALIZATIONS_HPP