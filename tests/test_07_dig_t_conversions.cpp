// test_07_dig_t_conversions.cpp
#define CATCH_CONFIG_MAIN
#include "external/catch_amalgamated.hpp"
#include "core/dig_t.hpp"

TEST_CASE("dig_t explicit conversions", "[dig_t][conversions]") {

    SECTION("explicit operator uint_t()") {
        NumRepr::dig_t<10> d10(7);
        auto val10 = static_cast<typename NumRepr::dig_t<10>::uint_t>(d10);
        REQUIRE(val10 == 7);
        STATIC_REQUIRE(std::is_same_v<decltype(val10), uint8_t>);

        NumRepr::dig_t<256> d256(128);
        auto val256 = static_cast<typename NumRepr::dig_t<256>::uint_t>(d256);
        REQUIRE(val256 == 128);
        STATIC_REQUIRE(std::is_same_v<decltype(val256), uint8_t>);

        NumRepr::dig_t<65536> d65536(32768);
        auto val65536 = static_cast<typename NumRepr::dig_t<65536>::uint_t>(d65536);
        REQUIRE(val65536 == 32768);
        STATIC_REQUIRE(std::is_same_v<decltype(val65536), uint16_t>);
    }

    SECTION("explicit operator nextsz_uint_t()") {
        NumRepr::dig_t<10> d10(7);
        auto val10 = static_cast<typename NumRepr::dig_t<10>::nextsz_uint_t>(d10);
        REQUIRE(val10 == 7);
        STATIC_REQUIRE(std::is_same_v<decltype(val10), uint16_t>);

        NumRepr::dig_t<256> d256(128);
        auto val256 = static_cast<typename NumRepr::dig_t<256>::nextsz_uint_t>(d256);
        REQUIRE(val256 == 128);
        STATIC_REQUIRE(std::is_same_v<decltype(val256), uint16_t>);

        NumRepr::dig_t<65536> d65536(32768);
        auto val65536 = static_cast<typename NumRepr::dig_t<65536>::nextsz_uint_t>(d65536);
        REQUIRE(val65536 == 32768);
        STATIC_REQUIRE(std::is_same_v<decltype(val65536), uint32_t>);
    }

    SECTION("explicit operator nextsz_int_t()") {
        NumRepr::dig_t<10> d10(7);
        auto val10 = static_cast<typename NumRepr::dig_t<10>::nextsz_int_t>(d10);
        REQUIRE(val10 == 7);
        STATIC_REQUIRE(std::is_same_v<decltype(val10), int16_t>);

        NumRepr::dig_t<256> d256(128);
        auto val256 = static_cast<typename NumRepr::dig_t<256>::nextsz_int_t>(d256);
        REQUIRE(val256 == 128);
        STATIC_REQUIRE(std::is_same_v<decltype(val256), int16_t>);

        NumRepr::dig_t<65536> d65536(32768);
        auto val65536 = static_cast<typename NumRepr::dig_t<65536>::nextsz_int_t>(d65536);
        REQUIRE(val65536 == 32768);
        STATIC_REQUIRE(std::is_same_v<decltype(val65536), int32_t>);
    }
}