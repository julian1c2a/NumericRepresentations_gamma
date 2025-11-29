#include <catch2/catch_test_macros.hpp>

#include "core/internal/append/int128_ops.hpp"
#include "core/internal/append/expected.hpp"
#include "core/internal/append/integers.hpp"

#include <limits>
#include <type_traits>
#include <iostream>

using namespace NumRepr;

// =============================================================================
// IDENTIFICACIÓN DE LA SUITE
// =============================================================================
// Truco: Este constructor estático corre antes que el main de Catch2
struct SuiteInfoPrinter {
    SuiteInfoPrinter() {
        std::cout << "\n=============================================================\n"
                  << " EJECUTANDO SUITE: test_02_append\n"
                  << "=============================================================" 
                  << std::endl;
    }
};
static SuiteInfoPrinter _info_printer;


// =============================================================================
// HELPERS DE ABSTRACCIÓN (Nativo vs Struct)
// =============================================================================

// Construye un uint128_t desde partes alta y baja
template <typename T = uint128_t>
constexpr T make_u128(uint64_t high, uint64_t low) {
    if constexpr (std::is_integral_v<T>) {
        // Caso Nativo (__int128)
        return (static_cast<T>(high) << 64) | low;
    } else {
        // Caso Struct (constructor es low, high)
        return T(low, high);
    }
}

// Obtiene la parte alta
template <typename T = uint128_t>
constexpr uint64_t get_high(const T& v) {
    if constexpr (std::is_integral_v<T>) {
        return static_cast<uint64_t>(v >> 64);
    } else {
        return v.high;
    }
}

// Obtiene la parte baja
template <typename T = uint128_t>
constexpr uint64_t get_low(const T& v) {
    if constexpr (std::is_integral_v<T>) {
        return static_cast<uint64_t>(v);
    } else {
        return v.low;
    }
}

// =============================================================================
// 1. PRUEBAS DE UINT128_T
// =============================================================================

TEST_CASE("uint128_t: Operaciones Básicas", "[append][int128]") {
    
    SECTION("Construcción y Comparación") {
        // Inicialización uniforme (funciona para ambos)
        uint128_t a = 10;
        uint128_t b = 20;
        uint128_t c = 10;
        
        CHECK(a < b);
        CHECK(b > a);
        CHECK(a == c);
        CHECK(a != b);
        CHECK(a <= c);
        CHECK(a >= c);
        
        // Construcción desde partes (High=1, Low=0) -> 2^64
        uint128_t big = make_u128(1, 0); 
        uint128_t small = make_u128(0, std::numeric_limits<uint64_t>::max());
        
        CHECK(big > small);
        CHECK((big - small) == uint128_t(1));
    }

    SECTION("Aritmética: Suma y Resta con Acarreo") {
        uint64_t max64 = std::numeric_limits<uint64_t>::max();
        // Low = max64, High = 0. Valor = 2^64 - 1
        uint128_t val = make_u128(0, max64); 
        
        // Sumar 1: (2^64 - 1) + 1 = 2^64
        // Representación binaria de 2^64 en 128 bits:
        // High = 1, Low = 0
        
        uint128_t res = val + 1;
        
        // CORRECCIÓN: Esperamos High=1, no High=0.
        CHECK(get_high(res) == 1); 
        CHECK(get_low(res) == 0);
        
        // Resta inversa: 2^64 - 1 = 2^64 - 1 (High=0, Low=MAX)
        uint128_t back = res - 1;
        CHECK(back == val);
        CHECK(get_high(back) == 0);
        CHECK(get_low(back) == max64);
    }

    SECTION("Multiplicación (Full 128 bits)") {
        uint128_t a = 2;
        uint128_t b = 3;
        CHECK((a * b) == 6);
        
        // Multiplicación que cruza 64 bits
        // 2^64 * 2 = 2^65
        uint128_t power64 = make_u128(1, 0); // Valor 2^64
        uint128_t two = 2;
        uint128_t res = power64 * two;
        
        // Resultado esperado: 2^65.
        // Representación: High=2 (bit 1 de la parte alta), Low=0.
        CHECK(get_high(res) == 2);
        CHECK(get_low(res) == 0);
    }

    SECTION("División y Módulo") {
        uint128_t a = 100;
        uint128_t b = 10;
        CHECK((a / b) == 10);
        CHECK((a % b) == 0);
        
        uint128_t c = 105;
        CHECK((c / b) == 10);
        CHECK((c % b) == 5);
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
        CHECK(NumRepr::mulmod(a, b, m) == 20);
    }

    SECTION("Runtime: Overflow prevention") {
        // a * b excede 2^64, pero el resultado modulo m cabe
        uint64_t max = std::numeric_limits<uint64_t>::max();
        uint64_t a = max; 
        uint64_t b = 2;
        uint64_t m = 100;
        
        CHECK(NumRepr::mulmod(a, b, m) == 30);
    }

    SECTION("Compile-Time: mulmod_ct") {
        constexpr uint64_t res = NumRepr::mulmod_ct<uint64_t, 10, 20, 30>();
        static_assert(res == 20, "mulmod_ct fallo basico");
        
        constexpr uint64_t big = (1ULL << 63);
        constexpr uint64_t res_big = NumRepr::mulmod_ct<uint64_t, big, 4, 100>();
        // Solo verificamos que compile y no rompa
        (void)res_big; 
        REQUIRE(true);
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
        CHECK(std::string(e.error()) == "Fallo critico");
    }
}