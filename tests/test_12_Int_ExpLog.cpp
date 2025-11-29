/**
 * @file tests/test_int_explog.cpp
 * @brief Pruebas para NumRepr::AuxFunc (Int_ExpLog.hpp)
 */

#include <catch2/catch_test_macros.hpp>
#include <iostream>

// =============================================================================
// IDENTIFICACIÓN DE LA SUITE
// =============================================================================
// Truco: Este constructor estático corre antes que el main de Catch2
struct SuiteInfoPrinter {
    SuiteInfoPrinter() {
        std::cout << "\n=============================================================\n"
                  << " EJECUTANDO SUITE: test_12_Int_ExpLog\n"
                  << "=============================================================" 
                  << std::endl;
    }
};
static SuiteInfoPrinter _info_printer;

#include "core/internal/math/Int_ExpLog.hpp"

using namespace NumRepr::AuxFunc;

// =============================================================================
// TESTS: COMPILE-TIME (namespace ct)
// =============================================================================

TEST_CASE("Compile-time math functions", "[math][ct]") {
    // 1. Log2
    STATIC_REQUIRE(ct::int_log2<1>() == 0);
    STATIC_REQUIRE(ct::int_log2<2>() == 1);
    STATIC_REQUIRE(ct::int_log2<3>() == 1);
    STATIC_REQUIRE(ct::int_log2<8>() == 3);
    STATIC_REQUIRE(ct::int_log2<1024>() == 10);

    // 2. Bit Width
    STATIC_REQUIRE(ct::bit_width<0>() == 1);
    STATIC_REQUIRE(ct::bit_width<1>() == 1);
    STATIC_REQUIRE(ct::bit_width<255>() == 8);
    STATIC_REQUIRE(ct::bit_width<256>() == 9);

    // 3. Pow2
    STATIC_REQUIRE(ct::int_pow2<0>() == 1);
    STATIC_REQUIRE(ct::int_pow2<10>() == 1024);
    STATIC_REQUIRE(ct::int_pow2<63>() == (1ULL << 63));

    // 4. Generic Pow (Validación con tabla)
    // 10^19 cabe en uint64, 10^20 no. La tabla debe permitir 19.
    STATIC_REQUIRE(ct::int_pow<10, 5>() == 100000ULL);
    // ct::int_pow<10, 20>(); // Esto debería fallar al compilar si se descomenta

    // 5. Generic Log
    STATIC_REQUIRE(ct::int_log<10, 100>() == 2);
    STATIC_REQUIRE(ct::int_log<10, 99>() == 1);
    STATIC_REQUIRE(ct::int_log<2, 8>() == 3);
}

// =============================================================================
// TESTS: RUNTIME
// =============================================================================

TEST_CASE("Runtime log2 functions", "[math][runtime][log]") {
    REQUIRE(int_log2(1) == 0);
    REQUIRE(int_log2(2) == 1);
    REQUIRE(int_log2(1023) == 9);
    REQUIRE(int_log2(1024) == 10);
    
    // Test safe version
    auto res = int_log2_sf(0);
    REQUIRE_FALSE(res.has_value());
    REQUIRE(res.error() == math_error_ec::baddomain);
    
    REQUIRE(*int_log2_sf(8) == 3);
}

TEST_CASE("Runtime integer power (int_pow) with safety", "[math][runtime][pow]") {
    
    SECTION("Basic powers") {
        REQUIRE(int_pow(2, 3) == 8);
        REQUIRE(int_pow(10, 2) == 100);
        REQUIRE(int_pow(5, 0) == 1);
        REQUIRE(int_pow(1, 999) == 1);
    }

    SECTION("Overflow detection via Table/Builtin") {
        // Caso límite: 2^63 cabe, 2^64 no (en uint64)
        REQUIRE(int_pow(2, 63) == (1ULL << 63));
        
        // 2^64 overflow -> debe devolver 0
        REQUIRE(int_pow(2, 64) == 0);

        // Base 10: Max exponente seguro es 19 (10^19 < 2^64)
        REQUIRE(int_pow(10, 19) > 0);
        REQUIRE(int_pow(10, 20) == 0);
    }
}

TEST_CASE("Digit counting", "[math][runtime][digits]") {
    SECTION("Base 10") {
        REQUIRE(count_digits_base10(0) == 1);
        REQUIRE(count_digits_base10(5) == 1);
        REQUIRE(count_digits_base10(9) == 1);
        REQUIRE(count_digits_base10(10) == 2);
        REQUIRE(count_digits_base10(99) == 2);
        REQUIRE(count_digits_base10(100) == 3);
    }

    SECTION("Binary Base") {
        REQUIRE(count_digits_base(0, 2) == 1);
        REQUIRE(count_digits_base(1, 2) == 1); // "1"
        REQUIRE(count_digits_base(2, 2) == 2); // "10"
        REQUIRE(count_digits_base(3, 2) == 2); // "11"
        REQUIRE(count_digits_base(4, 2) == 3); // "100"
    }
}