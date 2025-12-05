#include <catch2/catch_test_macros.hpp>
#include "core/internal/math/primes_ct.hpp"
#include <iostream>

// =============================================================================
// IDENTIFICACIÓN DE LA SUITE
// =============================================================================
// Truco: Este constructor estático corre antes que el main de Catch2
struct SuiteInfoPrinter {
    SuiteInfoPrinter() {
        std::cout << "\n=============================================================\n"
                  << " EJECUTANDO SUITE: test_primes_compiletime_catch2\n"
                  << "=============================================================" 
                  << std::endl;
    }
};
static SuiteInfoPrinter _info_printer;

TEST_CASE("Primalidad compile-time y runtime", "[primes][compiletime]")
{
    // Compile-time (static_assert) - si falla, no compila
    static_assert(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(2), "2 debe ser primo");
    static_assert(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(3), "3 debe ser primo");
    static_assert(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(65521), "65521 debe ser primo");
    static_assert(!NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(0), "0 no debe ser primo");
    static_assert(!NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(1), "1 no debe ser primo");
    static_assert(!NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(4), "4 no debe ser primo");
    static_assert(!NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(65535), "65535 no debe ser primo");
    static_assert(!NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(65536), "65536 no debe ser primo");

    SECTION("Runtime verification") {
        // Runtime - verificación que las constantes compile-time funcionan en runtime
        REQUIRE(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(2));
        REQUIRE(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(3));
        REQUIRE(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(65521));
        REQUIRE_FALSE(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(0));
        REQUIRE_FALSE(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(1));
        REQUIRE_FALSE(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(4));
        REQUIRE_FALSE(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(65535));
        REQUIRE_FALSE(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(65536));
    }
}