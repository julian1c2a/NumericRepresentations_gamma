#include "catch_amalgamated.hpp"
#include "core/dig_t.hpp"

TEST_CASE("dig_t to_string and from_string", "[dig_t]") {
    SECTION("to_string and from_string") {
        NumRepr::dig_t<10> d(5);
        std::string s = d.to_string();
        REQUIRE(s == "d[5]B10");

        auto d2 = NumRepr::dig_t<10>::from_string(s);
        REQUIRE(d2.has_value());
        REQUIRE(d2.value() == d);
    }

    SECTION("to_cstr and from_cstr") {
        NumRepr::dig_t<16> d(10);
        auto s = d.to_cstr();
        
        auto d2 = NumRepr::dig_t<16>::from_cstr(s.data());
        REQUIRE(d2.has_value());
        REQUIRE(d2.value() == d);
    }

    SECTION("from_string with invalid format") {
        auto d = NumRepr::dig_t<10>::from_string("invalid");
        REQUIRE_FALSE(d.has_value());
    }
}
