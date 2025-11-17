#include <catch2/catch_test_macros.hpp>
// Asegúrate de que la ruta de inclusión sea correcta.
// CMake (target_include_directories) añade 'include' al path.
#include "core/internal/lookup_tables/lookup_tables.hpp"

TEST_CASE("Lookup Tables", "[lookup_tables]") {

    SECTION("max_exponent_for_base_ct") {
        // Test with specific bases
        REQUIRE(NumRepr::auxiliary_functions::LUT::max_exponent_for_base_ct<2>() == 63);
        REQUIRE(NumRepr::auxiliary_functions::LUT::max_exponent_for_base_ct<10>() == 19);
        REQUIRE(NumRepr::auxiliary_functions::LUT::max_exponent_for_base_ct<16>() == 15);

        // Test with bases in ranges
        REQUIRE(NumRepr::auxiliary_functions::LUT::max_exponent_for_base_ct<13>() == 17);
        REQUIRE(NumRepr::auxiliary_functions::LUT::max_exponent_for_base_ct<100>() == 9);

        // Test with boundary conditions
        REQUIRE(NumRepr::auxiliary_functions::LUT::max_exponent_for_base_ct<1>() == std::numeric_limits<std::uint64_t>::max());
        REQUIRE(NumRepr::auxiliary_functions::LUT::max_exponent_for_base_ct<4294967295>() == 2);
        REQUIRE(NumRepr::auxiliary_functions::LUT::max_exponent_for_base_ct<4294967296>() == 1);
    }

    SECTION("max_exponent_for_base") {
        // Test with specific bases
        REQUIRE(NumRepr::auxiliary_functions::LUT::max_exponent_for_base(2) == 63);
        REQUIRE(NumRepr::auxiliary_functions::LUT::max_exponent_for_base(10) == 19);
        REQUIRE(NumRepr::auxiliary_functions::LUT::max_exponent_for_base(16) == 15);

        // Test with bases in ranges
        REQUIRE(NumRepr::auxiliary_functions::LUT::max_exponent_for_base(13) == 17);
        REQUIRE(NumRepr::auxiliary_functions::LUT::max_exponent_for_base(100) == 9);

        // Test with boundary conditions
        REQUIRE(NumRepr::auxiliary_functions::LUT::max_exponent_for_base(1) == std::numeric_limits<std::uint64_t>::max());
        REQUIRE(NumRepr::auxiliary_functions::LUT::max_exponent_for_base(4294967295) == 2);
        REQUIRE(NumRepr::auxiliary_functions::LUT::max_exponent_for_base(4294967296) == 1);
    }

    SECTION("get_nth_prime") {
        // Test with valid positions
        REQUIRE(NumRepr::auxiliary_functions::LUT::get_nth_prime(1) == 2);
        REQUIRE(NumRepr::auxiliary_functions::LUT::get_nth_prime(10) == 29);
        REQUIRE(NumRepr::auxiliary_functions::LUT::get_nth_prime(1024) == 8161);

        // Test with invalid positions
        REQUIRE(NumRepr::auxiliary_functions::LUT::get_nth_prime(0) == 0);
        REQUIRE(NumRepr::auxiliary_functions::LUT::get_nth_prime(1025) == 0);
    }

    SECTION("is_prime_ct") {
        // Test with primes in the list
        REQUIRE(NumRepr::auxiliary_functions::LUT::is_prime_ct(2));
        REQUIRE(NumRepr::auxiliary_functions::LUT::is_prime_ct(7));
        REQUIRE(NumRepr::auxiliary_functions::LUT::is_prime_ct(8161));

        // Test with composites
        REQUIRE_FALSE(NumRepr::auxiliary_functions::LUT::is_prime_ct(4));
        REQUIRE_FALSE(NumRepr::auxiliary_functions::LUT::is_prime_ct(10));
        REQUIRE_FALSE(NumRepr::auxiliary_functions::LUT::is_prime_ct(100));

        // Test with a prime not in the list
        REQUIRE_FALSE(NumRepr::auxiliary_functions::LUT::is_prime_ct(8171)); // Next prime after 8161
    }
}
