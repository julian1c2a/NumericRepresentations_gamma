#ifndef NUMREPR_INCLUDE_CORE_INTERNAL_APPEND_INT128_OPS_HPP_INCLUDED
#define NUMREPR_INCLUDE_CORE_INTERNAL_APPEND_INT128_OPS_HPP_INCLUDED

/**
 * @file int128_ops.hpp
 * @brief Gestión de tipos de 128 bits y aritmética modular extendida.
 * @details
 * 1. Detecta soporte nativo (__int128).
 * 2. Define uint128_t/sint128_t con aritmética completa y conversiones nativas.
 * 3. Proporciona mulmod (runtime) y mulmod_ct (compile-time).
 */


#include <cstdint>
#include <type_traits>
#include <concepts>
#include <limits>
#include <compare>
#include <utility>

#ifdef _MSC_VER
#  include <intrin.h>
#endif

namespace NumRepr {
    // --- Tipos de Tamaño Fijo (Stdint) ---
    using size_t = ::std::size_t;

    // Signed Fixed Width
    using sint8_t = ::std::int8_t;
    using sint16_t = ::std::int16_t;
    using sint32_t = ::std::int32_t;
    using sint64_t = ::std::int64_t;
    using int8_t = ::std::int8_t;
    using int16_t = ::std::int16_t;
    using int32_t = ::std::int32_t;
    using int64_t = ::std::int64_t;

    // Unsigned Fixed Width
    using uint8_t = ::std::uint8_t;
    using uint16_t = ::std::uint16_t;
    using uint32_t = ::std::uint32_t;
    using uint64_t = ::std::uint64_t;

    // Max Types
    using intmax_t = ::std::intmax_t;
    using uintmax_t = ::std::uintmax_t;

    // --- Utilidades del Espacio de Nombres ---
    using ::std::array; // Disponible como NumRepr::array
    using ::std::integral; // Concepto C++20
    using ::std::is_signed_v; // Trait para tipos con signo en type_traits
    using ::std::strong_ordering;
    using ::std::unsigned_integral; // Concepto C++20

    //==========================================================================
    // 1. DETECCIÓN DE SOPORTE NATIVO
    //==========================================================================

    #if !defined(NUMREPR_DISABLE_INT128)
    #if defined(__SIZEOF_INT128__)
    #define NUMREPR_HAS_NATIVE_INT128
    #elif defined(__clang__) && defined(__has_extension)
    #if __has_extension(int128)
    #define NUMREPR_HAS_NATIVE_INT128
    #endif
    #elif defined(__GNUC__)
    #define NUMREPR_HAS_NATIVE_INT128
    #endif
    #endif

    // =========================================================================
    // 2. DEFINICIÓN DE TIPOS (Nativo vs Emulado)
    // =========================================================================

    #if defined(NUMREPR_HAS_NATIVE_INT128)
    using uint128_t = unsigned __int128;
    using sint128_t = __int128;

    #else
    // --- CASO B: Emulación (Structs) ---

    struct uint128_t {
        uint64_t low;
        uint64_t high;

        // --- Constructores (Rule of Five) ---
        constexpr uint128_t() = default;
        constexpr uint128_t(const uint128_t&) = default;
        constexpr uint128_t(uint128_t&&) = default;
        constexpr uint128_t& operator=(const uint128_t&) = default;
        constexpr uint128_t& operator=(uint128_t&&) = default;

        // Constructor parte a parte
        constexpr uint128_t(uint64_t l, uint64_t h) noexcept : 
            low(l), high(h) {}

        // Constructor desde cualquier entero (incluido signed y literales)
        // Comportamiento estándar: si es negativo, sign-extend a todo el ancho (wrap-around)
        template <integral T>
        constexpr uint128_t(T v) noexcept : 
            low(static_cast<uint64_t>(v)),
            high( (is_signed_v<T> && v < 0) ? ~uint64_t{0} : 0 ) 
        {}

        // --- Asignación desde cualquier entero ---
        template <integral T>
        constexpr const uint128_t& operator=(T v) noexcept {
            low = static_cast<uint64_t>(v);
            // Manejo correcto de números negativos asignados a unsigned 
            // (ej: a = -1)
            high = (is_signed_v<T> && v < 0) ? ~uint64_t{0} : 0;
            return *this;
        }

        // --- Casts ---
        constexpr explicit operator bool() const noexcept { 
            return low != 0 || high != 0; 
        }

        template <integral T>
        constexpr explicit operator T() const noexcept { 
            return static_cast<T>(low); 
        }

        // --- Comparaciones ---
        constexpr bool operator==(const uint128_t&) const = default;

        constexpr inline 
        strong_ordering operator<=>(const uint128_t& rhs) const noexcept {
            if (auto cmp = high <=> rhs.high; cmp != 0) { return cmp; }
            return (low <=> rhs.low);
        }

        // --- Operadores Bit a Bit ---
        constexpr uint128_t operator|(const uint128_t& rhs) const noexcept 
        { return {low | rhs.low, high | rhs.high}; }
        constexpr uint128_t operator&(const uint128_t& rhs) const noexcept 
        { return {low & rhs.low, high & rhs.high}; }
        constexpr uint128_t operator^(const uint128_t& rhs) const noexcept 
        { return {low ^ rhs.low, high ^ rhs.high}; }
        constexpr uint128_t operator~() const noexcept 
        { return {~low, ~high}; }

        // --- Desplazamientos ---
        constexpr uint128_t operator<<(int shift) const noexcept {
            if (shift == 0) { return *this; }
            if (shift >= 128) { return {0, 0}; }
            if (shift >= 64) { return {0, low << (shift - 64)}; }
            return { (low << shift), (high << shift) | (low >> (64 - shift)) };
        }

        constexpr uint128_t operator>>(int shift) const noexcept {
            if (shift == 0) { return *this; }
            if (shift >= 128) { return {0, 0}; }
            if (shift >= 64) { return {high >> (shift - 64), 0}; }
            return { (low >> shift) | (high << (64 - shift)), high >> shift };
        }

        constexpr const uint128_t& operator<<=(int shift) noexcept {
            *this = *this << shift; 
            return *this; 
        }
        
        constexpr const uint128_t& operator>>=(int shift) noexcept { 
            *this = *this >> shift; 
            return *this; 
        }

        // --- Incremento/Decremento ---
        constexpr const uint128_t& operator++() noexcept {
            ++low; if (low == 0) ++high; return *this;
        }
        constexpr uint128_t operator++(int) noexcept {
            uint128_t temp = *this; ++(*this); return temp;
        }
        constexpr const uint128_t& operator--() noexcept {
            if (low == 0) --high; --low; return *this;
        }
        constexpr uint128_t operator--(int) noexcept {
            uint128_t temp = *this; --(*this); return temp;
        }

        // --- Aritmética Binaria ---
        constexpr uint128_t operator+(const uint128_t& rhs) const noexcept {
            uint128_t res{0, 0};
            res.low = low + rhs.low;
            res.high = high + rhs.high + (res.low < low);
            return res;
        }
        constexpr const uint128_t& operator+=(const uint128_t& rhs) noexcept {
            *this = *this + rhs; return *this; 
        }

        constexpr uint128_t operator-(const uint128_t& rhs) const noexcept {
            uint128_t res{0, 0};
            res.low = low - rhs.low;
            res.high = high - rhs.high - (low < rhs.low);
            return res;
        }
        constexpr const uint128_t& operator-=(const uint128_t& rhs) noexcept {
            *this = *this - rhs; return *this; 
        }

        constexpr uint128_t operator*(const uint128_t& rhs) const noexcept {
            uint128_t res{0, 0};
            res.low = low * rhs.low;
            res.high = (high * rhs.low) + (low * rhs.high);
            uint64_t a_lo = low & 0xFFFFFFFF;
            uint64_t a_hi = low >> 32;
            uint64_t b_lo = rhs.low & 0xFFFFFFFF;
            uint64_t b_hi = rhs.low >> 32;
            uint64_t p0 = a_lo * b_lo;
            uint64_t p1 = a_lo * b_hi;
            uint64_t p2 = a_hi * b_lo;
            uint64_t p3 = a_hi * b_hi;
            uint64_t cy = (p0 >> 32) + (p1 & 0xFFFFFFFF) + (p2 & 0xFFFFFFFF);
            res.high += p3 + (p1 >> 32) + (p2 >> 32) + (cy >> 32);
            return res;
        }
        
        constexpr const uint128_t& operator*=(const uint128_t& rhs) noexcept {
            *this = *this * rhs; return *this; 
        }

        static constexpr 
        std::pair<uint128_t, uint128_t> divmod(uint128_t n, uint128_t d)
            noexcept {
            if (d == uint128_t{0, 0}) return {{0,0}, {0,0}};
            if (d > n) return {{0,0}, n};
            if (d == n) return {{1,0}, {0,0}};
            uint128_t q{0,0}, r{0,0};
            for(int i = 127; i >= 0; --i) {
                r <<= 1;
                uint64_t bit {
                    (i >= 64) ? 
                        ((n.high >> (i - 64)) & 1) : 
                        ((n.low >> i) & 1)
                };
                if (bit) { r.low |= 1; }
                if (r >= d) {
                    r -= d;
                    if (i >= 64) {
                        q.high |= (1ULL << (i - 64));
                    } else {
                        q.low |= (1ULL << i);
                    }
                }
            }
            return {q, r};
        }

        constexpr uint128_t operator/(const uint128_t& rhs) const noexcept 
        { return divmod(*this, rhs).first; }
        constexpr uint128_t operator%(const uint128_t& rhs) const noexcept 
        { return divmod(*this, rhs).second; }
        constexpr const uint128_t& operator/=(const uint128_t& rhs) noexcept 
        { *this = *this / rhs; return *this; }
        constexpr const uint128_t& operator%=(const uint128_t& rhs) noexcept 
        { *this = *this % rhs; return *this; }
    };

    struct sint128_t {
        uint64_t low;
        int64_t high;

        // --- Rule of Five ---
        constexpr sint128_t() = default;
        constexpr sint128_t(const sint128_t&) = default;
        constexpr sint128_t(sint128_t&&) = default;
        constexpr sint128_t& operator=(const sint128_t&) = default;
        constexpr sint128_t& operator=(sint128_t&&) = default;

        constexpr sint128_t(uint64_t l, int64_t h) noexcept : low(l), high(h) {}

        // Constructor desde cualquier entero (extensión de signo)
        template <integral T>
        constexpr sint128_t(T v) noexcept
        : low(static_cast<uint64_t>(v)), high(v < 0 ? -1 : 0) {}

        // Conversiones
        constexpr explicit operator bool() const noexcept { 
            return low != 0 || high != 0; 
        }

        template <integral T>
        constexpr explicit operator T() const noexcept { 
            return static_cast<T>(low); 
        }

        // Asignación desde enteros
        template <integral T>
        constexpr const sint128_t& operator=(T v) noexcept {
            low = static_cast<uint64_t>(v);
            high = (v < 0 ? -1 : 0);
            return *this;
        }

        // Helpers
        constexpr bool operator==(const sint128_t&) const = default;
        constexpr strong_ordering operator<=>(const sint128_t& rhs) 
            const noexcept {
            if (auto cmp = high <=> rhs.high; cmp != 0) { return cmp; }
            return (low <=> rhs.low);
        }

        constexpr uint128_t as_unsigned() const noexcept { 
            return {low, static_cast<uint64_t>(high)}; 
        }
        static constexpr sint128_t from_unsigned(uint128_t u) noexcept { 
            return {u.low, static_cast<std::int64_t>(u.high)}; 
        }

        // Operadores
        constexpr sint128_t operator-() const noexcept {
            return from_unsigned(~as_unsigned() + uint128_t(1, 0));
        }

        constexpr const sint128_t& operator++() noexcept {
            uint128_t u {as_unsigned()}; ++u; 
            *this = from_unsigned(u); 
            return *this;
        }
        constexpr sint128_t operator++(int) noexcept {
            sint128_t temp {*this}; ++(*this); return temp;
        }
        constexpr const sint128_t& operator--() noexcept {
            uint128_t u {as_unsigned()}; --u; 
            *this = from_unsigned(u); 
            return *this;
        }
        constexpr sint128_t operator--(int) noexcept {
            sint128_t temp {*this}; --(*this); return temp;
        }

        constexpr sint128_t operator+(const sint128_t& rhs) const noexcept {
            return from_unsigned(as_unsigned() + rhs.as_unsigned());
        }
        constexpr const sint128_t& operator+=(const sint128_t& rhs) noexcept {
            *this = *this + rhs; return *this; 
        }

        constexpr sint128_t operator-(const sint128_t& rhs) const noexcept {
            return from_unsigned(as_unsigned() - rhs.as_unsigned());
        }
        constexpr const sint128_t& operator-=(const sint128_t& rhs) noexcept {
            *this = *this - rhs; 
            return *this; 
        }

        constexpr sint128_t operator*(const sint128_t& rhs) const noexcept {
            return from_unsigned(as_unsigned() * rhs.as_unsigned());
        }
        
        constexpr const sint128_t& operator*=(const sint128_t& rhs) noexcept {
            *this = *this * rhs; 
            return *this; 
        }

        constexpr sint128_t operator/(const sint128_t& rhs) const noexcept {
            const bool neg_lhs {high < 0};
            const bool neg_rhs {rhs.high < 0};
            // CORRECCIÓN MSVC: Usar != en lugar de ^ para booleanos en inicialización con {}
            // ^ promueve a int, != mantiene bool y evita error de estrechamiento.
            const bool neg_res {neg_lhs != neg_rhs}; 
            
            uint128_t u_lhs {
                neg_lhs ? 
                (-*this).as_unsigned() : 
                as_unsigned()
            };
            uint128_t u_rhs {
                neg_rhs ? 
                (-rhs).as_unsigned() : 
                rhs.as_unsigned()
            };
            uint128_t res {u_lhs / u_rhs};
            return (neg_res ? -from_unsigned(res) : from_unsigned(res));
        }
        constexpr const sint128_t& operator/=(const sint128_t& rhs) noexcept {
            *this = *this / rhs; 
            return *this; 
        }

        constexpr sint128_t operator%(const sint128_t& rhs) const noexcept {
            const bool neg_lhs {high < 0};
            const uint128_t u_lhs {
                neg_lhs ? 
                (-*this).as_unsigned() : 
                as_unsigned() 
            };
            const uint128_t u_rhs {
                (rhs.high < 0) ? 
                (-rhs).as_unsigned() : 
                rhs.as_unsigned()
            };
            const uint128_t res {u_lhs % u_rhs};
            return (neg_lhs ? -from_unsigned(res) : from_unsigned(res));
        }
        constexpr const sint128_t& operator%=(const sint128_t& rhs) noexcept {
            *this = *this % rhs; return *this; 
        }
    };
    #endif

    // ... (Resto de funciones mulmod se mantienen igual)

    template<unsigned_integral T>
    inline constexpr T mulmod(T a, T b, T m) noexcept {
        if (m == 0) return 0;
        if (a == 0 || b == 0) return 0;
        if (m == 1) return 0;
        if (a == 1) return (b % m);
        if (b == 1) return (a % m);

        T res = 0;
        a %= m;
        while (b > 0) {
            if (b & 1) {
                if (m - res > a)
                    res += a;
                else
                    res = res + a - m;
            }
            if (m - a > a)
                a *= 2;
            else
                a = a * 2 - m;
            b >>= 1;
        }
        return res;
    }

    template<>
    constexpr uint64_t mulmod<uint64_t>(uint64_t a, uint64_t b, uint64_t m)
        noexcept {

        if (m == 0) { return 0; }
        if (a == 0 || b == 0) { return 0; }
        if (m == 1)  { return 0; }
        if (a == 1) { return (b % m); }
        if (b == 1) { return (a % m); }

        #if defined(NUMREPR_HAS_NATIVE_INT128)
        return static_cast<uint64_t>(
            (static_cast<unsigned __int128>(a) * b) % m
        );
        #elif defined(_MSC_VER) && !defined(NUMREPR_DISABLE_INT128)
        if (std::is_constant_evaluated()) {
            uint64_t res = 0;
            a %= m;
            while (b > 0) {
                if (b & 1) { 
                    if (m - res > a) {
                        res += a;
                    } else {
                        res = res + a - m; 
                    }
                }
                if (m - a > a) {
                    a *= 2; 
                } else {
                    a = a * 2 - m;
                }
                b >>= 1;
            }
            return res;
        } else {
            unsigned __int64 high, low, rem;
            low = _umul128(a, b, &high);
            _udiv128(high, low, m, &rem);
            return rem;
        }
        #else
        uint64_t res = 0;
        a %= m;
        while (b > 0) {
            if (b & 1) { if (m - res > a) res += a; else res = res + a - m; }
            if (m - a > a) a *= 2; else a = a * 2 - m;
            b >>= 1;
        }
        return res;
        #endif
    }

    namespace internal {
        template <unsigned_integral T, T a, T b, T mod, T result = 0>
        consteval T mulmod_ct_impl() {
            if constexpr (b == 0) return result;
            else {
                constexpr T next_a { (a >= mod - a) ? (a * 2 - mod) : (a * 2) };
                constexpr T next_res {(
                    (b & 1)                            ? 
                        (
                            (result >= mod - a)             ? 
                                (result + a - mod)          : 
                                (result + a)
                        )                               : 
                        result
                )};
                constexpr T next_b { (b >> 1) };
                return mulmod_ct_impl<T, next_a, next_b, mod, next_res>();
            }
        }
    } // close namespace internal

    template <unsigned_integral T, T A, T B, T Mod>
    consteval T mulmod_ct() {
        static_assert(Mod != 0, "El modulo no puede ser 0");
        if constexpr (Mod == 1) return 0;
        if constexpr (A == 0 || B == 0) return 0;
        if constexpr (A == 1) return B % Mod;
        if constexpr (B == 1) return A % Mod;
        return internal::mulmod_ct_impl<T, A % Mod, B, Mod, 0>();
    }

} // close namespace NumRepr

#endif // NUMREPR_INCLUDE_CORE_INTERNAL_APPEND_INT128_OPS_HPP_INCLUDED