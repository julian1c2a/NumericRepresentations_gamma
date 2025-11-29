#include "core/internal/math/IntRoot.hpp"

#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <limits>
#include <iostream>

// Detectar soporte para __uint128_t
#if defined(__SIZEOF_INT128__)
    #define HAS_INT128
#endif

// =============================================================================
// IDENTIFICACIÓN DE LA SUITE
struct SuiteInfoPrinter {
    SuiteInfoPrinter() {
        std::cout << "\n=============================================================\n"
                  << " EJECUTANDO SUITE: test_13_IntRoot\n"
                  << "=============================================================" 
                  << std::endl;
    }
};
static SuiteInfoPrinter _info_printer;

using namespace NumRepr::AuxFunc;

TEST_CASE("IntRoot: floorsqrt (Runtime & Constexpr)", "[math][root][floor]") {
    
    SECTION("Casos base y números pequeños") {
        REQUIRE(floorsqrt(0) == 0);
        REQUIRE(floorsqrt(1) == 1);
        REQUIRE(floorsqrt(2) == 1);
        REQUIRE(floorsqrt(3) == 1);
        REQUIRE(floorsqrt(4) == 2);
        REQUIRE(floorsqrt(5) == 2);
        REQUIRE(floorsqrt(8) == 2);
        REQUIRE(floorsqrt(9) == 3);
    }

    SECTION("Tipos con signo") {
        REQUIRE(floorsqrt(-1) == 0);      // Error domain -> 0
        REQUIRE(floorsqrt(-100) == 0);
        REQUIRE(floorsqrt((int)4) == 2);
        REQUIRE(floorsqrt((int)10) == 3);
    }

    SECTION("Cuadrados perfectos (uint64_t)") {
        REQUIRE(floorsqrt(100ULL) == 10ULL);
        REQUIRE(floorsqrt(12345ULL * 12345ULL) == 12345ULL);
        REQUIRE(floorsqrt(4294967296ULL) == 65536ULL); // 2^32
    }

    SECTION("No cuadrados grandes (uint64_t)") {
        uint64_t base = 1000000ULL;
        uint64_t sq = base * base;
        REQUIRE(floorsqrt(sq) == base);
        REQUIRE(floorsqrt(sq + 1) == base);
        REQUIRE(floorsqrt(sq - 1) == base - 1);
    }

    SECTION("Límites de uint64_t") {
        // sqrt(2^64 - 1) es 4294967295 (2^32 - 1)
        REQUIRE(floorsqrt(std::numeric_limits<uint64_t>::max()) == 4294967295ULL);
    }

#ifdef HAS_INT128
    SECTION("Soporte para __uint128_t") {
        __uint128_t big = (__uint128_t)1 << 80; // 2^80
        // sqrt(2^80) = 2^40
        __uint128_t expected = (__uint128_t)1 << 40;
        REQUIRE(floorsqrt(big) == expected);
        
        // Un número no potencia de 2 exacto
        __uint128_t val = expected * expected + 500;
        REQUIRE(floorsqrt(val) == expected);
    }
#endif

    SECTION("Compile-time checks (floorsqrt)") {
        STATIC_REQUIRE(floorsqrt(0) == 0);
        STATIC_REQUIRE(floorsqrt(9) == 3);
        STATIC_REQUIRE(floorsqrt(10) == 3);
    }
}

TEST_CASE("IntRoot: floorsqrt_ct (Template)", "[math][root][floor][compile-time]") {
    STATIC_REQUIRE(floorsqrt_ct<0>() == 0);
    STATIC_REQUIRE(floorsqrt_ct<1>() == 1);
    STATIC_REQUIRE(floorsqrt_ct<2>() == 1);
    STATIC_REQUIRE(floorsqrt_ct<4>() == 2);
    STATIC_REQUIRE(floorsqrt_ct<8>() == 2);
    STATIC_REQUIRE(floorsqrt_ct<9>() == 3);
    
    // 2^60
    constexpr uint64_t big_pow2 = 1ULL << 60;
    STATIC_REQUIRE(floorsqrt_ct<big_pow2>() == (1ULL << 30));
    
    // Número primo grande
    STATIC_REQUIRE(floorsqrt_ct<104729>() == 323); // 323^2 = 104329
}

TEST_CASE("IntRoot: ceilsqrt (Runtime & Constexpr)", "[math][root][ceil]") {
    SECTION("Casos base") {
        REQUIRE(ceilsqrt(0) == 0);
        REQUIRE(ceilsqrt(1) == 1);
        REQUIRE(ceilsqrt(2) == 2); // sqrt(2) = 1.414 -> 2
        REQUIRE(ceilsqrt(3) == 2); // sqrt(3) = 1.732 -> 2
        REQUIRE(ceilsqrt(4) == 2);
        REQUIRE(ceilsqrt(5) == 3);
    }

    SECTION("Tipos con signo") {
        REQUIRE(ceilsqrt(-5) == 0);
        REQUIRE(ceilsqrt((int)5) == 3);
    }

    SECTION("Transiciones") {
        REQUIRE(ceilsqrt(8) == 3);
        REQUIRE(ceilsqrt(9) == 3);
        REQUIRE(ceilsqrt(10) == 4);
    }

    SECTION("Compile-time checks (ceilsqrt)") {
        STATIC_REQUIRE(ceilsqrt(0) == 0);
        STATIC_REQUIRE(ceilsqrt(2) == 2);
        STATIC_REQUIRE(ceilsqrt(4) == 2);
    }
}

TEST_CASE("IntRoot: ceilsqrt_ct (Template)", "[math][root][ceil][compile-time]") {
    STATIC_REQUIRE(ceilsqrt_ct<0>() == 0);
    STATIC_REQUIRE(ceilsqrt_ct<1>() == 1);
    STATIC_REQUIRE(ceilsqrt_ct<2>() == 2);
    STATIC_REQUIRE(ceilsqrt_ct<3>() == 2);
    STATIC_REQUIRE(ceilsqrt_ct<4>() == 2);
    STATIC_REQUIRE(ceilsqrt_ct<5>() == 3);
}

TEST_CASE("IntRoot: is_perfect_square", "[math][root][check]") {
    SECTION("Runtime checks") {
        REQUIRE(is_perfect_square(0));
        REQUIRE(is_perfect_square(1));
        REQUIRE(is_perfect_square(4));
        REQUIRE(is_perfect_square(9));
        REQUIRE(is_perfect_square(16));
        REQUIRE(is_perfect_square(25));
        
        REQUIRE_FALSE(is_perfect_square(2));
        REQUIRE_FALSE(is_perfect_square(3));
        REQUIRE_FALSE(is_perfect_square(5));
        REQUIRE_FALSE(is_perfect_square(8));
        
        REQUIRE_FALSE(is_perfect_square(-4)); // Negativos no son cuadrados perfectos en reales
    }

    SECTION("Compile-time checks") {
        STATIC_REQUIRE(is_perfect_square_ct<0>());
        STATIC_REQUIRE(is_perfect_square_ct<1>());
        STATIC_REQUIRE(is_perfect_square_ct<4>());
        STATIC_REQUIRE(!is_perfect_square_ct<2>());
        
        STATIC_REQUIRE(is_perfect_square(49));
        STATIC_REQUIRE(!is_perfect_square(50));
    }
    
    SECTION("Límites") {
        uint64_t max_sq = 4294967295ULL; // raíz de UINT64_MAX aprox
        // 4294967295^2 = 18446744065119617025
        uint64_t val = max_sq * max_sq;
        REQUIRE(is_perfect_square(val));
        REQUIRE_FALSE(is_perfect_square(val - 1));
        REQUIRE_FALSE(is_perfect_square(val + 1));
    }
}