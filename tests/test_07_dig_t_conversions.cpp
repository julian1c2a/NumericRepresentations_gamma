#include <catch2/catch_test_macros.hpp>
#include <type_traits>
#include <cstdint>
#include <iostream>

// =============================================================================
// IDENTIFICACIÓN DE LA SUITE
// =============================================================================
// Truco: Este constructor estático corre antes que el main de Catch2
struct SuiteInfoPrinter {
    SuiteInfoPrinter() {
        std::cout << "\n=============================================================\n"
                  << " EJECUTANDO SUITE: test_07_dig_t_conversions\n"
                  << "=============================================================" 
                  << std::endl;
    }
};
static SuiteInfoPrinter _info_printer;

#include "core/dig_t.hpp"

using namespace NumRepr;

// =============================================================================
// HELPERS Y CONCEPTS PARA VERIFICACIÓN DE TIPOS (C++20/23)
// =============================================================================

// Concepto para verificar que T se puede convertir explícitamente a U,
// pero NO implícitamente.
template <typename T, typename U>
concept ExplicitlyConvertibleTo = 
    std::is_constructible_v<U, T> &&   // Se puede construir U(t) o static_cast<U>(t)
    !std::is_convertible_v<T, U>;      // NO se puede hacer U u = t;

// Concepto para verificar que una operación es noexcept
template <typename T>
concept GetIsNoexcept = requires(const T& t) {
    { t.get() } noexcept;
};

// =============================================================================
// 1. TEST DE LA FUNCIÓN GET()
// =============================================================================

TEST_CASE("dig_t: Método get()", "[core][dig_t][conversion][get]") {

    SECTION("Consistencia de Tipos y noexcept") {
        // Caso 1: Base pequeña (uint8_t)
        using DigSmall = dig_t<10>;
        STATIC_CHECK(std::is_same_v<decltype(std::declval<DigSmall>().get()), uint8_t>);
        STATIC_CHECK(GetIsNoexcept<DigSmall>);

        // Caso 2: Base mediana (uint16_t)
        using DigMedium = dig_t<300>;
        STATIC_CHECK(std::is_same_v<decltype(std::declval<DigMedium>().get()), uint16_t>);
        STATIC_CHECK(GetIsNoexcept<DigMedium>);

        // Caso 3: Base grande (uint32_t)
        using DigLarge = dig_t<70000>;
        STATIC_CHECK(std::is_same_v<decltype(std::declval<DigLarge>().get()), uint32_t>);
        STATIC_CHECK(GetIsNoexcept<DigLarge>);
    }

    SECTION("Integridad del valor retornado") {
        dig_t<10> d(7);
        CHECK(d.get() == 7);
        
        // Verificamos que get devuelve el valor crudo normalizado
        dig_t<256> d2(300); // 300 % 256 = 44
        CHECK(d2.get() == 44);
    }
}

// =============================================================================
// 2. TEST DE CONVERSIONES EXPLÍCITAS (OPERATOR T())
// =============================================================================

TEST_CASE("dig_t: Conversiones Explícitas a Enteros", "[core][dig_t][conversion][explicit]") {

    // Helper lambda para verificar valores
    auto check_casts = [](auto digit, auto expected_val) {
        // Conversión a uint8_t
        CHECK(static_cast<uint8_t>(digit) == static_cast<uint8_t>(expected_val));
        
        // Conversión a uint16_t
        CHECK(static_cast<uint16_t>(digit) == static_cast<uint16_t>(expected_val));
        
        // Conversión a uint32_t
        CHECK(static_cast<uint32_t>(digit) == static_cast<uint32_t>(expected_val));
        
        // Conversión a uint64_t
        CHECK(static_cast<uint64_t>(digit) == static_cast<uint64_t>(expected_val));
        
        // Conversión a size_t
        CHECK(static_cast<size_t>(digit) == static_cast<size_t>(expected_val));

        // Conversión a tipos con signo (int64_t está explícitamente definido en la clase)
        CHECK(static_cast<int64_t>(digit) == static_cast<int64_t>(expected_val));
    };

    SECTION("Base 10 (uint8_t subyacente)") {
        using T = dig_t<10>;
        T d(5);

        // 1. Verificamos que la conversión sea EXPLÍCITA
        //    Esto asegura que `int x = d;` falle al compilar, lo cual es deseado.
        STATIC_CHECK(ExplicitlyConvertibleTo<T, uint8_t>);
        STATIC_CHECK(ExplicitlyConvertibleTo<T, int>);
        STATIC_CHECK(ExplicitlyConvertibleTo<T, size_t>);

        // 2. Verificamos los valores
        check_casts(d, 5);
    }

    SECTION("Base 65537 (uint32_t subyacente)") {
        using T = dig_t<65537>;
        T d(1000);

        // Verificamos "Explicitness"
        // Nota: dig_t define operadores específicos (uint_t, nextsz_uint_t, size_t, etc.)
        // La conversión a otros tipos (como int) ocurre via static_cast invocando esos operadores.
        
        // Verificamos conversión correcta de valores
        check_casts(d, 1000);
    }

    SECTION("Conversión a bool (Contextual)") {
        // dig_t NO tiene operator bool() explícito en el código provisto,
        // pero verificamos comportamiento si se añade o si se usa en if(d.get())
        
        dig_t<10> d_zero(0);
        dig_t<10> d_non_zero(5);

        // La forma correcta actual de verificar validez lógica es is_not_0() o get()
        CHECK(!d_zero.is_not_0());
        CHECK(d_non_zero.is_not_0());
        
        // Si intentamos static_cast<bool>(d), usará una de las conversiones a entero
        CHECK(static_cast<bool>(d_non_zero) == true);
        CHECK(static_cast<bool>(d_zero) == false);
    }

    SECTION("Verificación de noexcept en conversiones") {
        dig_t<10> d(1);
        
        // Las conversiones deben ser noexcept según la firma solicitada:
        // explicit operator TInt_t() const noexcept
        
        static_assert(noexcept(static_cast<uint8_t>(d)));
        static_assert(noexcept(static_cast<uint64_t>(d)));
        static_assert(noexcept(static_cast<size_t>(d)));
        static_assert(noexcept(static_cast<int64_t>(d)));
    }
}

// =============================================================================
// 3. COMPROBACIÓN DE TEMPLATES Y REQUIRES (Metaprogramación)
// =============================================================================

TEST_CASE("dig_t: Constraints de Conversión", "[core][dig_t][meta]") {
    
    // Verificamos que la clase compila correctamente con las condiciones if constexpr
    // para evitar definiciones duplicadas de operadores de conversión.
    
    SECTION("Base pequeña no duplica operadores") {
        // Para dig_t<10>:
        // uint_t es uint8_t.
        // nextsz_uint_t es uint16_t.
        // size_t es (usualmente) uint64_t.
        // La clase define:
        // 1. operator uint8_t()
        // 2. operator uint16_t() (porque != uint8_t)
        // 3. operator size_t() (porque != uint8_t y != uint16_t)
        // 4. operator uint32_t() ...
        
        dig_t<10> d(1);
        // Simplemente instanciar los cast para asegurar que no hay ambigüedad
        (void)static_cast<uint8_t>(d);
        (void)static_cast<uint16_t>(d);
        (void)static_cast<size_t>(d);
        SUCCEED("Compilación de casts en base pequeña exitosa sin ambigüedades");
    }

    SECTION("Base size_t (casos límite)") {
        // Si la base fuerza que uint_t sea size_t (en arquitecturas raras o bases gigantes)
        // O simplemente verificamos que no colisione con operator size_t definido manualmente.
        
        // Usamos una base que requiera uint32_t
        dig_t<100000> d(12345);
        
        // uint_t = uint32_t
        // operator uint32_t() definido como primario
        // operator size_t() definido en el bloque if constexpr
        
        CHECK(static_cast<uint32_t>(d) == 12345);
        CHECK(static_cast<size_t>(d) == 12345);
    }
}