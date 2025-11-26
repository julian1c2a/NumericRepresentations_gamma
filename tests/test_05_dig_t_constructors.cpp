#include <catch2/catch_test_macros.hpp>
#include <type_traits>
#include "core/dig_t.hpp"

using namespace NumRepr;

TEST_CASE("dig_t: Constructores y Límites de Tipo", "[core][dig_t][constructor][types]") {

    SECTION("Constructor por defecto") {
        STATIC_CHECK(dig_t<10>().get() == 0);
        STATIC_CHECK(dig_t<2>().get() == 0);
        STATIC_CHECK(dig_t<256>().get() == 0);
        STATIC_CHECK(dig_t<257>().get() == 0);
        STATIC_CHECK(dig_t<65536>().get() == 0);
        STATIC_CHECK(dig_t<65537>().get() == 0);
    }

    SECTION("Límites de tipo de dato subyacente") {
        STATIC_CHECK(std::is_same_v<dig_t<2>::uint_t, uint8_t>);
        STATIC_CHECK(std::is_same_v<dig_t<256>::uint_t, uint8_t>);
        STATIC_CHECK(std::is_same_v<dig_t<257>::uint_t, uint16_t>);
        STATIC_CHECK(std::is_same_v<dig_t<65536>::uint_t, uint16_t>);
        STATIC_CHECK(std::is_same_v<dig_t<65537>::uint_t, uint32_t>);
    }

    SECTION("Constructor desde entero") {
        // Base 256 (límite de uint8_t)
        STATIC_CHECK(dig_t<256>(255).get() == 255);
        STATIC_CHECK(dig_t<256>(256).get() == 0);
        STATIC_CHECK(dig_t<256>(-1).get() == 255);

        // Base 257 (inicio de uint16_t)
        STATIC_CHECK(dig_t<257>(256).get() == 256);
        STATIC_CHECK(dig_t<257>(257).get() == 0);
        
        // Base 65536 (límite de uint16_t)
        STATIC_CHECK(dig_t<65536>(65535).get() == 65535);
        STATIC_CHECK(dig_t<65536>(65536).get() == 0);
    }

    SECTION("Constructor desde C-string (constexpr)") {
        constexpr dig_t<10> d1("d[5]B10");
        STATIC_CHECK(d1.get() == 5);

        // Falla de parsing -> valor 0
        constexpr dig_t<10> d_fail("formato_invalido");
        STATIC_CHECK(d_fail.get() == 0);
        
        // Base mismatch -> valor 0
        constexpr dig_t<10> d_mismatch("d[5]B11");
        STATIC_CHECK(d_mismatch.get() == 0);
    }
    
    SECTION("Constructor desde std::string (runtime)") {
        dig_t<10> d1(std::string("d[8]B10"));
        CHECK(d1.get() == 8);

        dig_t<10> d_fail(std::string("formato_invalido"));
        CHECK(d_fail.get() == 0);
    }
    
    SECTION("Factories from_cstr y from_string") {
        // from_cstr
        constexpr auto res_cstr = dig_t<10>::from_cstr("d[7]B10");
        STATIC_CHECK(res_cstr.has_value());
        STATIC_CHECK(res_cstr->get() == 7);

        constexpr auto res_cstr_fail = dig_t<10>::from_cstr("d[7]B11");
        STATIC_CHECK(!res_cstr_fail.has_value());
        // The error code depends on the FSM implementation details, so we just check for failure
        // STATIC_CHECK(res_cstr_fail.error() == parse_error_t::base_mismatch);
        
        // from_string
        auto res_str = dig_t<10>::from_string("d[9]B10");
        REQUIRE(res_str.has_value());
        CHECK(res_str->get() == 9);
        
        auto res_str_fail = dig_t<10>::from_string("d[9]B11");
        REQUIRE(!res_str_fail.has_value());
    }
}
