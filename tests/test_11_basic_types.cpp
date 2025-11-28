/**
 * @file tests/test_basic_types.cpp
 * @brief Pruebas unitarias exhaustivas para basic_types.hpp
 * @details Cubre semántica de movimiento, conversiones de tipos, parsing de cadenas
 * y utilidades de metaprogramación.
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <string>
#include <type_traits>

// Incluimos el archivo bajo prueba
#include "core/internal/basic_types.hpp"

using namespace NumRepr;

// =============================================================================
// HELPER: CLASE PARA RASTREO DE MOVIMIENTOS (LifetimeTracker)
// =============================================================================
/**
 * @brief Clase auxiliar para verificar Perfect Forwarding
 * @details Cuenta cuántas veces se ha copiado o movido una instancia.
 * Esencial para validar que pack2array no hace copias innecesarias.
 */
struct LifetimeTracker {
    int value;
    static inline int copy_count = 0;
    static inline int move_count = 0;

    explicit LifetimeTracker(int v) : value(v) {}

    // Constructor de copia
    LifetimeTracker(const LifetimeTracker& other) : value(other.value) {
        copy_count++;
    }

    // Constructor de movimiento
    LifetimeTracker(LifetimeTracker&& other) noexcept : value(other.value) {
        move_count++;
        other.value = -1; // Estado inválido post-movimiento
    }

    // Asignación por copia
    LifetimeTracker& operator=(const LifetimeTracker& other) {
        if (this != &other) {
            value = other.value;
            copy_count++;
        }
        return *this;
    }

    // Asignación por movimiento
    LifetimeTracker& operator=(LifetimeTracker&& other) noexcept {
        if (this != &other) {
            value = other.value;
            move_count++;
            other.value = -1;
        }
        return *this;
    }

    static void reset_counters() {
        copy_count = 0;
        move_count = 0;
    }
};

// =============================================================================
// TESTS: ENUMS Y UTILIDADES BÁSICAS
// =============================================================================

TEST_CASE("sign_funct_e utilities work correctly", "[basic_types][enum]") {
    SECTION("Conversión a entero") {
        REQUIRE(to_int(sign_funct_e::vplus) == 1);
        REQUIRE(to_int(sign_funct_e::vminus) == -1);
        REQUIRE(to_int(sign_funct_e::vzero) == 0);
    }

    SECTION("Predicados de estado") {
        REQUIRE(is_positive(sign_funct_e::vplus));
        REQUIRE_FALSE(is_positive(sign_funct_e::vminus));
        
        REQUIRE(is_negative(sign_funct_e::vminus));
        REQUIRE_FALSE(is_negative(sign_funct_e::vplus));
        
        REQUIRE(is_zero(sign_funct_e::vzero));
    }

    SECTION("Inversión de signo") {
        REQUIRE(opposite_sign(sign_funct_e::vplus) == sign_funct_e::vminus);
        REQUIRE(opposite_sign(sign_funct_e::vminus) == sign_funct_e::vplus);
        REQUIRE(opposite_sign(sign_funct_e::vzero) == sign_funct_e::vzero);
    }
}

// =============================================================================
// TESTS: PARSING DE CADENAS (atoull family)
// =============================================================================

TEST_CASE("atoull conversions work correctly", "[basic_types][parsing]") {
    
    SECTION("atoull (fast/unsafe version)") {
        REQUIRE(type_traits::atoull("12345") == 12345ULL);
        REQUIRE(type_traits::atoull("0") == 0ULL);
    }

    SECTION("atoull_checked (safe version)") {
        // Caso éxito
        auto res = type_traits::atoull_checked("12345");
        REQUIRE(res.has_value());
        REQUIRE(*res == 12345ULL);

        // Caso string vacío
        auto empty = type_traits::atoull_checked("");
        REQUIRE_FALSE(empty.has_value());
        REQUIRE(empty.error() == type_traits::atoull_err_t::empty_str);

        // Caso no dígito
        auto invalid = type_traits::atoull_checked("123a45");
        REQUIRE_FALSE(invalid.has_value());
        REQUIRE(invalid.error() == type_traits::atoull_err_t::no_digit);

        // Caso Overflow (ULLONG_MAX + 1 approx)
        // ULLONG_MAX es 18446744073709551615 (20 dígitos)
        auto overflow = type_traits::atoull_checked("999999999999999999999"); 
        REQUIRE_FALSE(overflow.has_value());
        REQUIRE(overflow.error() == type_traits::atoull_err_t::overflow);
    }

    SECTION("atoull_ct (compile-time literal)") {
        // Estas líneas fallarían al compilar si la lógica consteval está mal
        constexpr auto val = type_traits::atoull_ct<"123456">();
        static_assert(val == 123456ULL);
        
        constexpr auto max_val = type_traits::atoull_ct<"18446744073709551615">();
        static_assert(max_val == std::numeric_limits<unsigned long long>::max());
    }
}

// =============================================================================
// TESTS: PACK2ARRAY & PERFECT FORWARDING (CRÍTICO)
// =============================================================================

TEST_CASE("pack2array implements perfect forwarding correctly", "[basic_types][meta][performance]") {
    LifetimeTracker::reset_counters();

    SECTION("Conversion from L-values (Copies expected)") {
        LifetimeTracker a(10), b(20);
        using Converter = ugly_pack_details::pack2array<LifetimeTracker, LifetimeTracker>;
        
        // Ejecución
        auto arr = Converter{}(a, b);

        REQUIRE(arr[0].value == 10);
        REQUIRE(arr[1].value == 20);
        
        // Verificación: Debería haber copias (para llenar el array) pero 0 movimientos
        CHECK(LifetimeTracker::copy_count >= 2);
        CHECK(LifetimeTracker::move_count == 0);
    }

    SECTION("Conversion from R-values (Moves expected)") {
        LifetimeTracker::reset_counters();
        using Converter = ugly_pack_details::pack2array<LifetimeTracker, LifetimeTracker>;

        // Ejecución con temporales (R-values)
        // Aquí es donde la implementación antigua fallaba (hacía copias)
        auto arr = Converter{}(LifetimeTracker(10), LifetimeTracker(20));

        REQUIRE(arr[0].value == 10);
        REQUIRE(arr[1].value == 20);

        // Verificación CRÍTICA:
        // Si hay perfect forwarding, los temporales se mueven al array.
        // Si NO hubiera, se copiarían.
        CHECK(LifetimeTracker::move_count >= 2);
        CHECK(LifetimeTracker::copy_count == 0); 
    }
}

TEST_CASE("assign_with_order helpers work", "[basic_types][meta]") {
    std::array<int, 3> dest = {0, 0, 0};
    
    SECTION("Standard assignment") {
        assign_with_order(dest, 10, 20, 30);
        
        REQUIRE(dest[0] == 10);
        REQUIRE(dest[1] == 20);
        REQUIRE(dest[2] == 30);
    }
}

// =============================================================================
// TESTS: TYPE TRAITS & SIZE DETECTION
// =============================================================================

TEST_CASE("type_traits correctly detect sizes", "[basic_types][traits]") {
    SECTION("nextsz_UInt_for_UInt_t") {
        // uint8 -> uint16
        STATIC_REQUIRE(std::is_same_v<type_traits::nextsz_UInt_for_UInt_t<std::uint8_t>, std::uint16_t>);
        // uint32 -> uint64
        STATIC_REQUIRE(std::is_same_v<type_traits::nextsz_UInt_for_UInt_t<std::uint32_t>, std::uint64_t>);
    }

    SECTION("Size comparison utilities") {
        STATIC_REQUIRE(type_traits::lt_sz_v<std::uint8_t, std::uint16_t>);
        STATIC_REQUIRE(type_traits::eq_sz_v<std::uint32_t, int>); // Asumiendo int de 32 bits
        STATIC_REQUIRE(type_traits::gt_sz_v<std::uint64_t, std::uint8_t>);
    }
}