#include <catch2/catch_test_macros.hpp>
#include <sstream>
#include <string>
#include <array>
#include "core/dig_t.hpp"
#include "core/dig_t_display_helpers.hpp"

using namespace NumRepr;

// =============================================================================
// 1. SERIALIZACIÓN A STRING (to_string, to_cstr)
// =============================================================================

TEST_CASE("dig_t: Serialización a String", "[core][dig_t][io][tostring]") {

    SECTION("Runtime std::string (to_string)") {
        dig_t<10> d(5);
        // Formato estándar: "d[valor]Bbase"
        CHECK(d.to_string() == "d[5]B10");

        dig_t<16> hex_d(15);
        CHECK(hex_d.to_string() == "d[15]B16");
        
        dig_t<256> byte_d(255);
        CHECK(byte_d.to_string() == "d[255]B256");
    }

    SECTION("Compile-time C-string (to_cstr)") {
        // Verificamos que se puede evaluar en constexpr
        constexpr dig_t<10> d(7);
        constexpr auto arr = d.to_cstr();
        
        // Verificación estática del contenido
        STATIC_CHECK(arr[0] == 'd');
        STATIC_CHECK(arr[1] == '[');
        STATIC_CHECK(arr[2] == '7');
        STATIC_CHECK(arr[3] == ']');
        STATIC_CHECK(arr[4] == 'B');
        STATIC_CHECK(arr[5] == '1');
        STATIC_CHECK(arr[6] == '0');
        STATIC_CHECK(arr[7] == '\0');
        
        // Verificación runtime de la conversión a string
        CHECK(std::string(arr.data()) == "d[7]B10");
    }
}

// =============================================================================
// 2. PARSING DESDE STRING (from_string, parsing errors)
// =============================================================================

TEST_CASE("dig_t: Parsing desde String", "[core][dig_t][io][parsing]") {

    SECTION("Formatos válidos") {
        // Formato corto: d[...]
        auto res1 = dig_t<10>::from_string("d[5]B10");
        REQUIRE(res1.has_value());
        CHECK(res1->get() == 5);

        // Formato alternativo: dig#...#
        auto res2 = dig_t<16>::from_string("dig#15#B16");
        REQUIRE(res2.has_value());
        CHECK(res2->get() == 15); // F
        
        // Espacios extra o basura (el parser actual es estricto o flexible según FSM)
        // Asumimos estricto en prefijo pero probamos valores grandes que se normalizan
        auto res3 = dig_t<10>::from_string("d[15]B10"); // 15 mod 10 = 5
        REQUIRE(res3.has_value());
        CHECK(res3->get() == 5);
    }

    SECTION("Errores de Parsing") {
        // Base incorrecta
        auto err_base = dig_t<10>::from_string("d[5]B11");
        CHECK(!err_base.has_value());
        CHECK(err_base.error() == parse_error_t::base_mismatch);

        // Formato malformado
        auto err_fmt = dig_t<10>::from_string("x[5]B10");
        CHECK(!err_fmt.has_value());
        CHECK(err_fmt.error() == parse_error_t::invalid_prefix);

        // Falta base
        auto err_nobase = dig_t<10>::from_string("d[5]");
        CHECK(!err_nobase.has_value());
        // El error específico depende de dónde corte la FSM, suele ser missing_delimiter o missing_B
    }
}

// =============================================================================
// 3. PARSING COMPILE-TIME (from_array_ct, make_digit literals)
// =============================================================================

TEST_CASE("dig_t: Parsing Compile-Time", "[core][dig_t][io][ct]") {

    SECTION("from_array_ct con std::array") {
        constexpr std::array<char, 8> input = {'d','[','3',']','B','1','0','\0'};
        constexpr auto res = dig_t<10>::from_array_ct<input>();
        
        STATIC_CHECK(res.has_value());
        STATIC_CHECK(res->get() == 3);
    }

    SECTION("make_digit (Literal String Template)") {
        // Deducción automática de base y valor en tiempo de compilación
        constexpr auto d = make_digit<fixed_string("dig[25]B13")>();
        
        // Tipo deducido: dig_t<13>
        STATIC_CHECK(std::is_same_v<decltype(d), const dig_t<13>>);
        
        // Valor normalizado: 25 % 13 = 12
        STATIC_CHECK(d.get() == 12);
        
        // Otro caso
        constexpr auto d2 = make_digit<fixed_string("d[10]B2")>();
        STATIC_CHECK(std::is_same_v<decltype(d2), const dig_t<2>>);
        STATIC_CHECK(d2.get() == 0); // 10 es par -> 0
    }
}

// =============================================================================
// 4. STREAM OPERATORS (<<, >>)
// =============================================================================

TEST_CASE("dig_t: Stream Operators", "[core][dig_t][io][stream]") {

    SECTION("Output Operator (<<)") {
        dig_t<10> d(7);
        std::stringstream ss;
        ss << d;
        CHECK(ss.str() == "d[7]B10");
    }

    SECTION("Input Operator (>>)") {
        dig_t<10> d(0);
        std::stringstream ss("d[9]B10");
        ss >> d;
        
        CHECK(d.get() == 9);
        CHECK(!ss.fail());
        
        // Fallo de lectura
        std::stringstream ss_fail("basura");
        ss_fail >> d;
        CHECK(ss_fail.fail()); // Debe activar bit de fallo
    }
}

// =============================================================================
// 5. DISPLAY HELPERS (Evitar uint8_t char printing)
// =============================================================================

TEST_CASE("dig_t: Display Helpers", "[core][dig_t][io][display]") {
    
    SECTION("Prevención de impresión como char") {
        // dig_t<10> usa uint8_t internamente.
        // uint8_t(65) es 'A' en ASCII.
        // Queremos ver "5" (65%10), no un carácter extraño ni 'A'.
        
        dig_t<10> d(65); // Valor interno 5
        
        std::stringstream ss;
        // Usamos el helper display()
        ss << display(d); 
        
        // Verificamos que sea "5" numérico
        CHECK(ss.str() == "5");
        
        // Verificamos type_name
        CHECK(std::string(type_name<10>()) == "uint8_t");
    }
    
    SECTION("Bases grandes (uint16_t)") {
        dig_t<300> d(65); // Valor 65
        std::stringstream ss;
        ss << display(d);
        CHECK(ss.str() == "65");
        
        CHECK(std::string(type_name<300>()) == "uint16_t");
    }
}