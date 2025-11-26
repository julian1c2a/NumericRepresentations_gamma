// test_06_dig_t_assignations.cpp
#define CATCH_CONFIG_MAIN
#include "external/catch_amalgamated.hpp"
#include "core/dig_t.hpp"

TEST_CASE("dig_t assignations", "[dig_t][assignations]") {
    SECTION("Assignment from dig_t") {
        NumRepr::dig_t<10> a(5);
        NumRepr::dig_t<10> b;
        b = a;
        REQUIRE(b.get() == 5);
    }

    SECTION("Assignment from various integral types") {
        NumRepr::dig_t<100> d;

        SECTION("short") {
            short val = 123;
            d = val;
            REQUIRE(d.get() == 23);
            val = -123;
            d = val;
            REQUIRE(d.get() == 77); // -123 mod 100 = 77
        }

        SECTION("int") {
            int val = 123;
            d = val;
            REQUIRE(d.get() == 23);
            val = -123;
            d = val;
            REQUIRE(d.get() == 77);
        }

        SECTION("long") {
            long val = 123L;
            d = val;
            REQUIRE(d.get() == 23);
            val = -123L;
            d = val;
            REQUIRE(d.get() == 77);
        }

        SECTION("long long") {
            long long val = 123LL;
            d = val;
            REQUIRE(d.get() == 23);
            val = -123LL;
            d = val;
            REQUIRE(d.get() == 77);
        }

        SECTION("unsigned short") {
            unsigned short val = 123;
            d = val;
            REQUIRE(d.get() == 23);
        }

        SECTION("unsigned int") {
            unsigned int val = 123u;
            d = val;
            REQUIRE(d.get() == 23);
        }

        SECTION("unsigned long") {
            unsigned long val = 123ul;
            d = val;
            REQUIRE(d.get() == 23);
        }

        SECTION("unsigned long long") {
            unsigned long long val = 123ull;
            d = val;
            REQUIRE(d.get() == 23);
        }

        SECTION("int8_t") {
            int8_t val = 123;
            d = val;
            REQUIRE(d.get() == 23);
             val = -123;
            d = val;
            REQUIRE(d.get() == 77);
        }

        SECTION("uint8_t") {
            uint8_t val = 123;
            d = val;
            REQUIRE(d.get() == 23);
        }

        SECTION("int16_t") {
            int16_t val = 123;
            d = val;
            REQUIRE(d.get() == 23);
            val = -123;
            d = val;
            REQUIRE(d.get() == 77);
        }

        SECTION("uint16_t") {
            uint16_t val = 123;
            d = val;
            REQUIRE(d.get() == 23);
        }

        SECTION("int32_t") {
            int32_t val = 123;
            d = val;
            REQUIRE(d.get() == 23);
            val = -123;
            d = val;
            REQUIRE(d.get() == 77);
        }

        SECTION("uint32_t") {
            uint32_t val = 123;
            d = val;
            REQUIRE(d.get() == 23);
        }

        SECTION("int64_t") {
            int64_t val = 123;
            d = val;
            REQUIRE(d.get() == 23);
            val = -123;
            d = val;
            REQUIRE(d.get() == 77);
        }

        SECTION("uint64_t") {
            uint64_t val = 123;
            d = val;
            REQUIRE(d.get() == 23);
        }
    }

    SECTION("Copy and Move assignments") {
        SECTION("Copy assignment by reference") {
            NumRepr::dig_t<10> a(5);
            NumRepr::dig_t<10> b(3);
            b = a;
            REQUIRE(b.get() == 5);
            REQUIRE(a.get() == 5); // Original object is not modified
        }

        SECTION("Move assignment from lvalue") {
            NumRepr::dig_t<10> a(5);
            NumRepr::dig_t<10> b(3);
            b = std::move(a);
            REQUIRE(b.get() == 5);
            // After move, 'a' is in a valid but unspecified state.
            // For a trivial type like dig_t, it will still be 5.
            REQUIRE(a.get() == 5);
        }

        SECTION("Move assignment from rvalue") {
            NumRepr::dig_t<10> b(3);
            b = NumRepr::dig_t<10>(5);
            REQUIRE(b.get() == 5);
        }
    }

    SECTION("Compound assignment operators") {
        NumRepr::dig_t<10> a(3);
        a += NumRepr::dig_t<10>(4);
        REQUIRE(a.get() == 7);
        a += 5;
        REQUIRE(a.get() == 2);

        NumRepr::dig_t<10> b(8);
        b -= NumRepr::dig_t<10>(3);
        REQUIRE(b.get() == 5);
        b -= 7;
        REQUIRE(b.get() == 8);

        NumRepr::dig_t<10> c(2);
        c *= NumRepr::dig_t<10>(3);
        REQUIRE(c.get() == 6);
        c *= 4;
        REQUIRE(c.get() == 4);

        // In base 10, the inverse of 3 is 7 (3*7=21=1 mod 10)
        NumRepr::dig_t<10> d(8);
        d /= NumRepr::dig_t<10>(3); // 8 * 7 = 56 = 6 mod 10
        REQUIRE(d.get() == 6);

        NumRepr::dig_t<10> e(9);
        e %= NumRepr::dig_t<10>(4);
        REQUIRE(e.get() == 1);
    }

    SECTION("Bitwise assignment operators") {
        NumRepr::dig_t<10> a(5);
        a &= NumRepr::dig_t<10>(3);
        REQUIRE(a.get() == 3);

        NumRepr::dig_t<10> b(5);
        b |= NumRepr::dig_t<10>(7);
        REQUIRE(b.get() == 7);

        NumRepr::dig_t<10> c(2);
        c ^= 3u; // 2^3 = 8
        REQUIRE(c.get() == 8);
    }
}