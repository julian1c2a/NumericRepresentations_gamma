#include <catch2/catch_test_macros.hpp>

#include "core/internal/append/int128_ops.hpp"
#include "core/internal/append/expected.hpp"
#include "core/internal/append/integers.hpp"

#include <limits>
#include <type_traits>

using namespace NumRepr;

// =============================================================================
// 1. PRUEBAS DE UINT128_T
// =============================================================================

TEST_CASE("uint128_t: Operaciones Básicas", "[append][int128]") {
    
    // Verificar tamaño y alineación si es posible (depende de la implementación)
    // static_assert(sizeof(uint128_t) == 16, "uint128_t debe ser de 16 bytes");

    SECTION("Construcción y Comparación") {
        uint128_t a(10);
        uint128_t b(20);
        uint128_t c(10);
        
        CHECK(a < b);
        CHECK(b > a);
        CHECK(a == c);
        CHECK(a != b);
        CHECK(a <= c);
        CHECK(a >= c);
        
        // Construcción desde partes (High, Low)
        // 1 en High es 2^64
        uint128_t big(1, 0); 
        uint128_t small(0, std::numeric_limits<uint64_t>::max());
        
        CHECK(big > small);
        CHECK((big - small) == uint128_t(1));
    }

    SECTION("Aritmética: Suma y Resta con Acarreo") {
        uint64_t max64 = std::numeric_limits<uint64_t>::max();
        uint128_t val(0, max64); // Low = max64, High = 0
        
        // Sumar 1 debería causar acarreo a High
        uint128_t res = val + uint128_t(1);
        
        // Verificamos accediendo a partes internas si es struct, o por valor si es nativo.
        // Asumiendo struct emulado para el test genérico:
        if constexpr (!std::is_same_v<uint128_t, unsigned __int128>) {
            CHECK(res.high == 1);
            CHECK(res.low == 0);
        } else {
            CHECK(res > val);
        }
        
        // Resta inversa
        uint128_t back = res - uint128_t(1);
        CHECK(back == val);
    }

    SECTION("Multiplicación (Full 128 bits)") {
        uint128_t a(0, 2); // 2
        uint128_t b(0, 3); // 3
        CHECK((a * b) == uint128_t(6));
        
        // Multiplicación que cruza 64 bits
        // 2^64 * 2 = 2^65
        uint128_t power64(1, 0); 
        uint128_t two(2);
        uint128_t res = power64 * two;
        
        if constexpr (!std::is_same_v<uint128_t, unsigned __int128>) {
            CHECK(res.high == 2);
            CHECK(res.low == 0);
        }
    }

    SECTION("División y Módulo") {
        uint128_t a(100);
        uint128_t b(10);
        CHECK((a / b) == uint128_t(10));
        CHECK((a % b) == uint128_t(0));
        
        uint128_t c(105);
        CHECK((c / b) == uint128_t(10));
        CHECK((c % b) == uint128_t(5));
    }
}

// =============================================================================
// 2. PRUEBAS DE MULMOD (Aritmética Modular Segura)
// =============================================================================

TEST_CASE("mulmod: Multiplicación Modular Segura", "[append][mulmod]") {
    
    SECTION("Runtime: Casos normales") {
        uint64_t a = 10;
        uint64_t b = 20;
        uint64_t m = 30;
        // (10 * 20) % 30 = 200 % 30 = 20
        CHECK(NumRepr::mulmod(a, b, m) == 20);
    }

    SECTION("Runtime: Overflow prevention") {
        // a * b excede 2^64, pero el resultado modulo m cabe
        uint64_t max = std::numeric_limits<uint64_t>::max();
        uint64_t a = max; 
        uint64_t b = 2;
        uint64_t m = 100;
        
        // (max * 2) % 100
        // max termina en ...15 (5), * 2 = ...30 -> % 100 = 30 ?
        // UINT64_MAX = 18446744073709551615
        // mod 100 = 15
        // (15 * 2) % 100 = 30
        
        CHECK(NumRepr::mulmod(a, b, m) == 30);
    }

    SECTION("Compile-Time: mulmod_ct") {
        constexpr uint64_t res = NumRepr::mulmod_ct<uint64_t, 10, 20, 30>();
        static_assert(res == 20, "mulmod_ct fallo basico");
        
        // Prueba overflow CT
        // (2^63 * 4) % 100
        constexpr uint64_t big = (1ULL << 63);
        // big mod 100 = ...08 (si 2^64 acaba en 16, 2^63 acaba en 08)
        // (8 * 4) % 100 = 32
        constexpr uint64_t res_big = NumRepr::mulmod_ct<uint64_t, big, 4, 100>();
        static_assert(res_big == 32, "mulmod_ct fallo overflow");
    }
}

// =============================================================================
// 3. PRUEBAS DE EXPECTED (Polyfill)
// =============================================================================

TEST_CASE("expected: Funcionalidad Básica", "[append][expected]") {
    
    using Exp = NumRepr::expected<int, const char*>;
    
    SECTION("Valor (Éxito)") {
        Exp e(42);
        REQUIRE(e.has_value());
        CHECK(e.value() == 42);
        CHECK(static_cast<bool>(e) == true);
    }

    SECTION("Error (Fallo)") {
        Exp e = NumRepr::unexpected("Fallo critico");
        REQUIRE_FALSE(e.has_value());
        CHECK(e.error() == std::string("Fallo critico")); // Comparacion string
    }
}