#include "core/internal/conversions.hpp"

#include <catch2/catch_test_macros.hpp>

#include <array>
#include <cstdint>
#include <limits>
#include <iostream>

// =============================================================================
// IDENTIFICACIÓN DE LA SUITE
// =============================================================================
// Truco: Este constructor estático corre antes que el main de Catch2
struct SuiteInfoPrinter {
    SuiteInfoPrinter() {
        std::cout << "\n=============================================================\n"
                  << " EJECUTANDO SUITE: test_15_conversions\n"
                  << "=============================================================" 
                  << std::endl;
    }
};
static SuiteInfoPrinter _info_printer;


using namespace NumRepr::AuxFunc;

// =============================================================================
// MOCK DIGIT PARA PRUEBAS AISLADAS
// =============================================================================
// Simulamos un dígito que cumple con la interfaz requerida por conversions.hpp
// (tener un operator() que devuelve el valor).
struct MockDig {
    uint64_t val;
    constexpr uint64_t operator()() const { return val; }
};

// =============================================================================
// PRUEBAS DE CONVERSIÓN SEGURA (Runtime)
// =============================================================================

TEST_CASE("Conversions: conversion_to_int_safe (Valid Cases)", "[core][conversions][safe]") {
    
    SECTION("Base 10: Conversión simple") {
        // Número 123 en base 10. 
        // conversions.hpp asume que el índice L-1 es el MSD (Most Significant Digit)
        // y el índice 0 es el LSD (Least Significant Digit).
        // Representación: [3, 2, 1] -> 1*10^2 + 2*10^1 + 3*10^0 = 123
        std::array<MockDig, 3> digits = { MockDig{3}, MockDig{2}, MockDig{1} };
        
        auto res = conversion_to_int_safe<10, 3>(digits);
        REQUIRE(res.has_value());
        CHECK(res.value() == 123);
    }

    SECTION("Base 2: Conversión binaria") {
        // 1011 (binario) = 11 (decimal)
        // Array: [1, 1, 0, 1] (LSD en 0) -> 1*2^0 + 1*2^1 + 0*2^2 + 1*2^3 = 1 + 2 + 0 + 8 = 11
        std::array<MockDig, 4> digits = { MockDig{1}, MockDig{1}, MockDig{0}, MockDig{1} };
        
        auto res = conversion_to_int_safe<2, 4>(digits);
        REQUIRE(res.has_value());
        CHECK(res.value() == 11);
    }

    SECTION("Base 16: Conversión Hexadecimal") {
        // 0xFF = 255
        std::array<MockDig, 2> digits = { MockDig{15}, MockDig{15} };
        
        auto res = conversion_to_int_safe<16, 2>(digits);
        REQUIRE(res.has_value());
        CHECK(res.value() == 255);
    }
    
    SECTION("Límite uint64_t") {
        // Vamos a probar con un valor cercano al máximo pero seguro
        // UINT64_MAX = 18446744073709551615
        // En base 10, probamos 100...0 (19 ceros) = 10^19, que cabe
        std::array<MockDig, 20> digits;
        digits.fill(MockDig{0});
        digits[19] = MockDig{1}; // 1 seguido de 19 ceros
        
        // 10^19 es menor que UINT64_MAX (aprox 1.84 * 10^19)
        auto res = conversion_to_int_safe<10, 20>(digits);
        REQUIRE(res.has_value());
        CHECK(res.value() == 10000000000000000000ULL);
    }
}

TEST_CASE("Conversions: conversion_to_int_safe (Error Cases)", "[core][conversions][error]") {

    SECTION("Invalid Base") {
        std::array<MockDig, 1> digits = { MockDig{0} };
        // Base 0
        auto res0 = conversion_to_int_safe<0, 1>(digits);
        CHECK_FALSE(res0.has_value());
        CHECK(res0.error() == ConversionError::InvalidBase);
        
        // Base 1
        auto res1 = conversion_to_int_safe<1, 1>(digits);
        CHECK_FALSE(res1.has_value());
        CHECK(res1.error() == ConversionError::InvalidBase);
    }

    SECTION("Invalid Digit (Digit >= Base)") {
        std::array<MockDig, 2> digits = { MockDig{0}, MockDig{10} }; // Dígito '10' en base 10 es ilegal
        
        auto res = conversion_to_int_safe<10, 2>(digits);
        CHECK_FALSE(res.has_value());
        CHECK(res.error() == ConversionError::InvalidDigit);
    }

    SECTION("Overflow") {
        // Intentar representar algo mayor que UINT64_MAX
        // Base 10. Ponemos puros 9s. 20 nueves desborda seguro (max es ~1.8e19)
        std::array<MockDig, 20> digits;
        digits.fill(MockDig{9});
        
        auto res = conversion_to_int_safe<10, 20>(digits);
        CHECK_FALSE(res.has_value());
        CHECK(res.error() == ConversionError::Overflow);
    }
}

// =============================================================================
// PRUEBAS DE CONVERSIÓN COMPILE-TIME (ConstEval)
// =============================================================================

TEST_CASE("Conversions: conversion_to_int_ct (ConstEval)", "[core][conversions][ct]") {
    // Definimos arrays constexpr
    constexpr std::array<MockDig, 3> arr10 = { MockDig{3}, MockDig{2}, MockDig{1} }; // 123
    
    // Verificación estática
    constexpr uint64_t val10 = conversion_to_int_ct<10, 3>(arr10);
    static_assert(val10 == 123, "Error en conversión CT base 10");
    
    constexpr std::array<MockDig, 4> arr2 = { MockDig{1}, MockDig{1}, MockDig{0}, MockDig{1} }; // 11
    constexpr uint64_t val2 = conversion_to_int_ct<2, 4>(arr2);
    static_assert(val2 == 11, "Error en conversión CT base 2");
    
    // Runtime check de lo calculado en CT
    CHECK(val10 == 123);
    CHECK(val2 == 11);
}