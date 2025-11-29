#include <catch2/catch_test_macros.hpp>

#include "core/internal/basic_types.hpp"
#include "core/internal/AuxFunc.hpp"
#include "core/internal/conversions.hpp"

#include <array>
#include <string_view>
#include <limits>
#include <iostream>

using namespace NumRepr;
using namespace NumRepr::AuxFunc;

// =============================================================================
// IDENTIFICACIÓN DE LA SUITE
// =============================================================================
// Truco: Este constructor estático corre antes que el main de Catch2
struct SuiteInfoPrinter {
    SuiteInfoPrinter() {
        std::cout << "\n=============================================================\n"
                  << " EJECUTANDO SUITE: test_03_core_internal\n"
                  << "=============================================================" 
                  << std::endl;
    }
};
static SuiteInfoPrinter _info_printer;


// =============================================================================
// 1. BASIC TYPES: String Parsing (atoull family)
// =============================================================================

TEST_CASE("atoull: Conversión básica (sin checks)", "[core][internal][basic_types]") {
    // Compile-time
    constexpr uint64_t val_ct = type_traits::atoull("12345");
    static_assert(val_ct == 12345, "atoull constexpr falló");

    // Runtime
    CHECK(type_traits::atoull("0") == 0);
    CHECK(type_traits::atoull("18446744073709551615") == UINT64_MAX);
    
    // String view version
    constexpr std::string_view sv = "9876";
    CHECK(type_traits::atoull(sv) == 9876);
}

TEST_CASE("atoull_checked: Conversión segura", "[core][internal][basic_types]") {
    using namespace NumRepr::type_traits;

    SECTION("Casos Exitosos") {
        auto res = atoull_checked("123456");
        REQUIRE(res.has_value());
        CHECK(res.value() == 123456);

        auto res_max = atoull_checked("18446744073709551615");
        REQUIRE(res_max.has_value());
        CHECK(res_max.value() == UINT64_MAX);
    }

    SECTION("Errores") {
        auto err_empty = atoull_checked("");
        REQUIRE_FALSE(err_empty.has_value());
        CHECK(err_empty.error() == atoull_err_t::empty_str);

        auto err_nodigit = atoull_checked("123a456");
        REQUIRE_FALSE(err_nodigit.has_value());
        CHECK(err_nodigit.error() == atoull_err_t::no_digit);

        // Overflow: UINT64_MAX + 1
        auto err_overflow = atoull_checked("18446744073709551616");
        REQUIRE_FALSE(err_overflow.has_value());
        CHECK(err_overflow.error() == atoull_err_t::overflow);
    }
}

// Variables estáticas para pasar como referencia a template NTTP
static constexpr char NUM_STR[] = "123456789";
static constexpr char ZERO_STR[] = "0";

TEST_CASE("atoull_ct: Literal de usuario compile-time", "[core][internal][basic_types]") {
    using namespace NumRepr::type_traits;
    
    // CORRECCIÓN: Pasar la variable estática, no el literal directo
    constexpr uint64_t val = atoull_ct<NUM_STR>();
    static_assert(val == 123456789, "atoull_ct valor incorrecto");

    // Casos de borde
    constexpr uint64_t val_0 = atoull_ct<ZERO_STR>();
    static_assert(val_0 == 0);
}

// =============================================================================
// 2. BASIC TYPES: Pack Utilities (pack2array)
// =============================================================================

TEST_CASE("pack2array: Conversión de variadic packs", "[core][internal][pack]") {
    using namespace NumRepr::ugly_pack_details;

    SECTION("Generación de std::array") {
        constexpr auto arr = pack2array<int, int, int>{}(10, 20, 30);
        // CORRECCIÓN: std::is_same_v necesita tipos exactos
        static_assert(std::is_same_v<std::remove_cv_t<decltype(arr)>, std::array<int, 3>>);
        CHECK(arr[0] == 10);
        CHECK(arr[1] == 20);
        CHECK(arr[2] == 30);
    }

    SECTION("Assign with order") {
        std::array<int, 3> dest{0, 0, 0};
        assign_with_order(dest, 1, 2, 3);
        CHECK(dest[0] == 1);
        CHECK(dest[1] == 2);
        CHECK(dest[2] == 3);
    }
}

// =============================================================================
// 3. AUX FUNC: Math Helpers
// =============================================================================

TEST_CASE("AuxFunc: Logaritmos y Bits", "[core][internal][aux_func]") {
    SECTION("int_log2") {
        CHECK(int_log2(1) == 0);
        CHECK(int_log2(2) == 1);
        CHECK(int_log2(1024) == 10);
        CHECK(int_log2(0) == 0); // Comportamiento definido en implementación
    }

    SECTION("int_log2_sf (Safe)") {
        CHECK(int_log2_sf(1024).value() == 10);
        CHECK_FALSE(int_log2_sf(0).has_value());
    }

    SECTION("bit_width") {
        CHECK(bit_width(0) == 0);
        CHECK(bit_width(1) == 1);
        CHECK(bit_width(2) == 2); // 10
        CHECK(bit_width(255) == 8);
    }
}

TEST_CASE("AuxFunc: Raíces Cuadradas", "[core][internal][aux_func]") {
    SECTION("floorsqrt") {
        CHECK(floorsqrt(0) == 0);
        CHECK(floorsqrt(1) == 1);
        CHECK(floorsqrt(4) == 2);
        CHECK(floorsqrt(8) == 2);
        CHECK(floorsqrt(9) == 3);
        CHECK(floorsqrt(UINT64_MAX) == 4294967295ULL);
    }

    SECTION("ceilsqrt") {
        CHECK(ceilsqrt(0) == 0);
        CHECK(ceilsqrt(1) == 1);
        CHECK(ceilsqrt(2) == 2);
        CHECK(ceilsqrt(4) == 2);
        CHECK(ceilsqrt(5) == 3);
    }

    SECTION("is_perfect_square") {
        CHECK(is_perfect_square(0));
        CHECK(is_perfect_square(1));
        CHECK(is_perfect_square(4));
        CHECK_FALSE(is_perfect_square(2));
        CHECK_FALSE(is_perfect_square(3));
        CHECK_FALSE(is_perfect_square(5));
    }
}

// =============================================================================
// 4. CONVERSIONS: Digit Arrays to Int
// =============================================================================

// Mock simple de un "Dígito" para probar conversion_to_int_safe
struct MockDigit {
    uint8_t v;
    constexpr uint8_t operator()() const { return v; }
};

TEST_CASE("Conversions: Array to Integer", "[core][internal][conversions]") {
    
    std::array<MockDigit, 3> arr10 = {MockDigit{1}, MockDigit{2}, MockDigit{3}}; 
    // La implementación itera desde L-2 hasta 0, sumando L-1 al principio.
    // L-1 (índice 2) es el más significativo.
    // 3 * 100 + 2 * 10 + 1 = 321.

    SECTION("Conversión Segura (Base 10)") {
        auto res = conversion_to_int_safe<10, 3>(arr10);
        REQUIRE(res.has_value());
        CHECK(res.value() == 321); 
    }

    SECTION("Conversión Segura (Base 2)") {
        std::array<MockDigit, 4> arr2 = {MockDigit{1}, MockDigit{0}, MockDigit{1}, MockDigit{1}};
        // Indices: 0, 1, 2, 3
        // L=4. MSB es index 3 (valor 1).
        // 1*8 + 1*4 + 0*2 + 1*1 = 13
        auto res = conversion_to_int_safe<2, 4>(arr2);
        REQUIRE(res.has_value());
        CHECK(res.value() == 13);
    }

    SECTION("Errores de Conversión") {
        // Dígito inválido (>= Base)
        std::array<MockDigit, 2> invalid = {MockDigit{1}, MockDigit{10}}; // 10 no válido en base 10
        auto err = conversion_to_int_safe<10, 2>(invalid);
        REQUIRE_FALSE(err.has_value());
        CHECK(err.error() == ConversionError::InvalidDigit);
    }
}