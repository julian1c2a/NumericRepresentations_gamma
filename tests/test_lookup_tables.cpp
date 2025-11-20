#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark_all.hpp>
#include <catch2/generators/catch_generators_range.hpp>
#include "core/internal/lookup_tables/math_utils.hpp"
#include "core/internal/lookup_tables/primes_compiletime.hpp"
#include "core/internal/lookup_tables/primes.hpp"

// --- Verificaciones en tiempo de compilación ---
static_assert(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct<2>(), "2 debe ser primo");
static_assert(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct<3>(), "3 debe ser primo");
static_assert(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct<65521>(), "65521 debe ser primo");
static_assert(!NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct<0>(), "0 no debe ser primo");
static_assert(!NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct<1>(), "1 no debe ser primo");
static_assert(!NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct<4>(), "4 no debe ser primo");
static_assert(!NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct<65535>(), "65535 no debe ser primo");
static_assert(!NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct<65536>(), "65536 no debe ser primo");

// --- Verificaciones en tiempo de compilación ---
static_assert(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct<2>(), "2 debe ser primo");
static_assert(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct<3>(), "3 debe ser primo");
static_assert(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct<65521>(), "65521 debe ser primo");
static_assert(!NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct<0>(), "0 no debe ser primo");
static_assert(!NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct<1>(), "1 no debe ser primo");
static_assert(!NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct<4>(), "4 no debe ser primo");
static_assert(!NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct<65535>(), "65535 no debe ser primo");
static_assert(!NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct<65536>(), "65536 no debe ser primo");

TEST_CASE("LUT_of_primes: is_prime_lt_65537", "[LUT_of_primes]")
{
    using NumRepr::AuxFunc::LUT::is_prime_lt_65537;

    SECTION("Primos conocidos (runtime)")
    {
        REQUIRE(is_prime_lt_65537(2));
        REQUIRE(is_prime_lt_65537(3));
        REQUIRE(is_prime_lt_65537(65521));
    }

    SECTION("No primos conocidos (runtime)")
    {
        REQUIRE_FALSE(is_prime_lt_65537(0));
        REQUIRE_FALSE(is_prime_lt_65537(1));
        REQUIRE_FALSE(is_prime_lt_65537(4));
        REQUIRE_FALSE(is_prime_lt_65537(65535));
        REQUIRE_FALSE(is_prime_lt_65537(65536));
    }
}

TEST_CASE("Lookup Tables", "[lookup_tables]")
{

    SECTION("max_exponent_for_base_ct")
    {
        // Test with specific bases
        REQUIRE(NumRepr::AuxFunc::LUT::max_exponent_for_base_ct<2>() == 63);
        REQUIRE(NumRepr::AuxFunc::LUT::max_exponent_for_base_ct<10>() == 19);
        REQUIRE(NumRepr::AuxFunc::LUT::max_exponent_for_base_ct<16>() == 15);

        // Test with bases in ranges
        REQUIRE(NumRepr::AuxFunc::LUT::max_exponent_for_base_ct<13>() == 17);
        REQUIRE(NumRepr::AuxFunc::LUT::max_exponent_for_base_ct<100>() == 9);

        // Test with boundary conditions
        REQUIRE(NumRepr::AuxFunc::LUT::max_exponent_for_base_ct<0>() == std::numeric_limits<std::uint64_t>::max());
        REQUIRE(NumRepr::AuxFunc::LUT::max_exponent_for_base_ct<1>() == std::numeric_limits<std::uint64_t>::max());
        REQUIRE_FALSE(NumRepr::AuxFunc::LUT::max_exponent_for_base_ct<2>() == std::numeric_limits<std::uint64_t>::max());
        REQUIRE(NumRepr::AuxFunc::LUT::max_exponent_for_base_ct<4294967295>() == 2);
        REQUIRE_FALSE(NumRepr::AuxFunc::LUT::max_exponent_for_base_ct<4294967295>() == 3);
        REQUIRE(NumRepr::AuxFunc::LUT::max_exponent_for_base_ct<4294967296>() == 1);
        REQUIRE_FALSE(NumRepr::AuxFunc::LUT::max_exponent_for_base_ct<4294967296>() == 2);
        REQUIRE(NumRepr::AuxFunc::LUT::max_exponent_for_base_ct<std::numeric_limits<std::uint64_t>::max()>() == 1);
        REQUIRE_FALSE(NumRepr::AuxFunc::LUT::max_exponent_for_base_ct<std::numeric_limits<std::uint64_t>::max()>() == 2);
    }

    SECTION("max_exponent_for_base")
    {
        // Test with specific bases
        REQUIRE(NumRepr::AuxFunc::LUT::max_exponent_for_base(2) == 63);
        REQUIRE(NumRepr::AuxFunc::LUT::max_exponent_for_base(10) == 19);
        REQUIRE(NumRepr::AuxFunc::LUT::max_exponent_for_base(16) == 15);

        // Test with bases in ranges
        REQUIRE(NumRepr::AuxFunc::LUT::max_exponent_for_base(13) == 17);
        REQUIRE(NumRepr::AuxFunc::LUT::max_exponent_for_base(100) == 9);

        // Test with boundary conditions
        REQUIRE(NumRepr::AuxFunc::LUT::max_exponent_for_base(0) == std::numeric_limits<std::uint64_t>::max());
        REQUIRE(NumRepr::AuxFunc::LUT::max_exponent_for_base(1) == std::numeric_limits<std::uint64_t>::max());
        REQUIRE_FALSE(NumRepr::AuxFunc::LUT::max_exponent_for_base(2) == std::numeric_limits<std::uint64_t>::max());
        REQUIRE(NumRepr::AuxFunc::LUT::max_exponent_for_base(4294967295) == 2);
        REQUIRE_FALSE(NumRepr::AuxFunc::LUT::max_exponent_for_base(4294967295) == 3);
        REQUIRE(NumRepr::AuxFunc::LUT::max_exponent_for_base(4294967296) == 1);
        REQUIRE_FALSE(NumRepr::AuxFunc::LUT::max_exponent_for_base(4294967296) == 2);
        REQUIRE(NumRepr::AuxFunc::LUT::max_exponent_for_base(std::numeric_limits<std::uint64_t>::max()) == 1);
        REQUIRE_FALSE(NumRepr::AuxFunc::LUT::max_exponent_for_base(std::numeric_limits<std::uint64_t>::max()) == 2);
    }

    SECTION("get_nth_prime")
    {
        // Test with valid positions
        REQUIRE(NumRepr::AuxFunc::LUT::get_nth_prime(1) == 2);
        REQUIRE(NumRepr::AuxFunc::LUT::get_nth_prime(10) == 29);
        REQUIRE(NumRepr::AuxFunc::LUT::get_nth_prime(1024) == 8161);

        // Test with invalid positions
        REQUIRE(NumRepr::AuxFunc::LUT::get_nth_prime(0) == -1);
        REQUIRE(NumRepr::AuxFunc::LUT::get_nth_prime(1025) == std::numeric_limits<std::int64_t>::min());
    }

    SECTION("is_prime_rt")
    {
        // Test with primes in the list
        REQUIRE(NumRepr::AuxFunc::LUT::is_prime_rt(2));
        REQUIRE(NumRepr::AuxFunc::LUT::is_prime_rt(7));
        REQUIRE(NumRepr::AuxFunc::LUT::is_prime_rt(8161));

        // Test with composites
        REQUIRE_FALSE(NumRepr::AuxFunc::LUT::is_prime_rt(4));
        REQUIRE_FALSE(NumRepr::AuxFunc::LUT::is_prime_rt(10));
        REQUIRE_FALSE(NumRepr::AuxFunc::LUT::is_prime_rt(100));

        // Test with a prime not in the list
        REQUIRE_FALSE(NumRepr::AuxFunc::LUT::is_prime_rt(8171)); // Next prime after 8161
    }

    SECTION("is_prime_ct")
    {
        // Test with primes in the list
        REQUIRE(NumRepr::AuxFunc::LUT::is_prime_ct<2>());
        REQUIRE(NumRepr::AuxFunc::LUT::is_prime_ct<7>());
        REQUIRE(NumRepr::AuxFunc::LUT::is_prime_ct<8161>());

        // Test with composites
        REQUIRE_FALSE(NumRepr::AuxFunc::LUT::is_prime_ct<4>());
        REQUIRE_FALSE(NumRepr::AuxFunc::LUT::is_prime_ct<10>());
        REQUIRE_FALSE(NumRepr::AuxFunc::LUT::is_prime_ct<100>());

        // Test with a prime not in the list
        REQUIRE_FALSE(NumRepr::AuxFunc::LUT::is_prime_ct<8171>()); // Next prime after 8161
    }
}

TEST_CASE("Benchmarks para LUT", "[lut][benchmark]")
{

    // Esto mide el tiempo de ejecución de la función get_nth_prime(1024)
    BENCHMARK("get_nth_prime (elemento 1024)")
    {
        // Esta función se llamará muchas veces para obtener una medida estable
        return NumRepr::AuxFunc::LUT::get_nth_prime(1024);
    };

    BENCHMARK("get_nth_prime (elemento 1)")
    {
        return NumRepr::AuxFunc::LUT::get_nth_prime(1);
    };

    BENCHMARK("get_nth_prime (elemento 2)")
    {
        return NumRepr::AuxFunc::LUT::get_nth_prime(2);
    };

    BENCHMARK("get_nth_prime (elemento 3)")
    {
        return NumRepr::AuxFunc::LUT::get_nth_prime(3);
    };

    BENCHMARK("get_nth_prime (elemento 256)")
    {
        return NumRepr::AuxFunc::LUT::get_nth_prime(256);
    };

    BENCHMARK("get_nth_prime (elemento 37)")
    {
        return NumRepr::AuxFunc::LUT::get_nth_prime(37);
    };

    BENCHMARK("get_nth_prime (elemento 42)")
    {
        return NumRepr::AuxFunc::LUT::get_nth_prime(42);
    };

    BENCHMARK("max_exponent_for_base (runtime) 1024")
    {
        return NumRepr::AuxFunc::LUT::max_exponent_for_base(1024);
    };

    BENCHMARK("max_exponent_for_base (runtime) 1")
    {
        return NumRepr::AuxFunc::LUT::max_exponent_for_base(1);
    };

    BENCHMARK("max_exponent_for_base (runtime) 256")
    {
        return NumRepr::AuxFunc::LUT::max_exponent_for_base(256);
    };

    BENCHMARK("max_exponent_for_base (runtime) 37")
    {
        return NumRepr::AuxFunc::LUT::max_exponent_for_base(37);
    };

    BENCHMARK("max_exponent_for_base (runtime) 42")
    {
        return NumRepr::AuxFunc::LUT::max_exponent_for_base(42);
    };
}

// TEST_CASE("Benchmarks para LUT get_nth_prime", "[lut][benchmark][get_nth_prime]") {

// int k_prime = GENERATE(range(1, 9));
// std::string benchmark_name_prime = "get_nth_prime (elemento " + std::to_string(k_prime) + ")";
// volatile int runtime_k_prime = k_prime;
// BENCHMARK(std::move(benchmark_name_prime)) {
// return NumRepr::AuxFunc::LUT::get_nth_prime(runtime_k_prime);
// };

// }

// TEST_CASE("Benchmarks para LUT max_exponent_for_base", "[lut][benchmark][max_exponent_for_base]") {

// int k_exponent = GENERATE(range(1, 9));
// BENCHMARK("max_exponent_for_base (runtime) (elemento " + std::to_string(k_exponent) + ")") {
// return NumRepr::AuxFunc::LUT::max_exponent_for_base(k_exponent);
// };
// }
