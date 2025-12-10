#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark_all.hpp>
#include <catch2/generators/catch_generators_range.hpp>
#include "core/internal/math/math_utils.hpp"
#include "core/internal/math/primes_ct.hpp"
#include "core/internal/math/primes.hpp"

// --- Verificaciones en tiempo de compilación ---
static_assert(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(2), "2 debe ser primo");
static_assert(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(3), "3 debe ser primo");
static_assert(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(65521), "65521 debe ser primo");
static_assert(!NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(0), "0 no debe ser primo");
static_assert(!NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(1), "1 no debe ser primo");
static_assert(!NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(4), "4 no debe ser primo");
static_assert(!NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(65535), "65535 no debe ser primo");
static_assert(!NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(65536), "65536 no debe ser primo");

// --- Verificaciones en tiempo de compilación ---
static_assert(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(2), "2 debe ser primo");
static_assert(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(3), "3 debe ser primo");
static_assert(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(65521), "65521 debe ser primo");
static_assert(!NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(0), "0 no debe ser primo");
static_assert(!NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(1), "1 no debe ser primo");
static_assert(!NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(4), "4 no debe ser primo");
static_assert(!NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(65535), "65535 no debe ser primo");
static_assert(!NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(65536), "65536 no debe ser primo");

TEST_CASE("LUT_of_primes: is_prime_lt_65537", "[LUT_of_primes]")
{
    using NumRepr::AuxFunc::LUT::is_prime_in_uint16;

    SECTION("Primos conocidos (runtime)")
    {
        REQUIRE(is_prime_in_uint16(2));
        REQUIRE(is_prime_in_uint16(3));
        REQUIRE(is_prime_in_uint16(65521));
    }

    SECTION("No primos conocidos (runtime)")
    {
        REQUIRE_FALSE(is_prime_in_uint16(0));
        REQUIRE_FALSE(is_prime_in_uint16(1));
        REQUIRE_FALSE(is_prime_in_uint16(4));
        REQUIRE_FALSE(is_prime_in_uint16(65535));
        // Note: 65536 overflows uint16_t, so we can't test it with is_prime_in_uint16
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

    SECTION("primes_lt_65537 array access")
    {
        // Test with valid array indices (note: 1-based vs 0-based indexing)
        REQUIRE(NumRepr::AuxFunc::LUT::primes_lt_65537[0] == 2);    // 1st prime
        REQUIRE(NumRepr::AuxFunc::LUT::primes_lt_65537[9] == 29);   // 10th prime
        REQUIRE(NumRepr::AuxFunc::LUT::primes_lt_65537[1023] == 8161); // 1024th prime
        
        // Test actual array size (contains all primes less than 65537)
        REQUIRE(NumRepr::AuxFunc::LUT::primes_lt_65537.size() == 6542);
    }

    SECTION("isPrime (runtime)")
    {
        // Test with primes
        REQUIRE(NumRepr::AuxFunc::LUT::isPrime(2));
        REQUIRE(NumRepr::AuxFunc::LUT::isPrime(7));
        REQUIRE(NumRepr::AuxFunc::LUT::isPrime(8161));

        // Test with composites
        REQUIRE_FALSE(NumRepr::AuxFunc::LUT::isPrime(4));
        REQUIRE_FALSE(NumRepr::AuxFunc::LUT::isPrime(10));
        REQUIRE_FALSE(NumRepr::AuxFunc::LUT::isPrime(100));

        // Test with a prime not in the list (large primes test with Miller-Rabin)
        REQUIRE(NumRepr::AuxFunc::LUT::isPrime(8171)); // Next prime after 8161
    }

    SECTION("is_prime_lt_65537_ct (compile-time)")
    {
        // Test with primes using constexpr function calls
        REQUIRE(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(2));
        REQUIRE(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(7));
        REQUIRE(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(8161));

        // Test with composites 
        REQUIRE_FALSE(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(4));
        REQUIRE_FALSE(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(10));
        REQUIRE_FALSE(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(100));

        // Test with a composite not in small list
        REQUIRE_FALSE(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct(8170)); // Composite near 8161
    }
}

TEST_CASE("Benchmarks para LUT", "[lut][benchmark]")
{

    // Esto mide el tiempo de acceso directo al array de primos
    BENCHMARK("primes_lt_65537 access (elemento 1023)")
    {
        // Array access es 0-indexed, así que 1023 es el 1024vo primo
        return NumRepr::AuxFunc::LUT::primes_lt_65537[1023];
    };

    BENCHMARK("primes_lt_65537 access (elemento 0)")
    {
        return NumRepr::AuxFunc::LUT::primes_lt_65537[0];  // 1st prime
    };

    BENCHMARK("primes_lt_65537 access (elemento 1)")
    {
        return NumRepr::AuxFunc::LUT::primes_lt_65537[1];  // 2nd prime
    };

    BENCHMARK("primes_lt_65537 access (elemento 2)")
    {
        return NumRepr::AuxFunc::LUT::primes_lt_65537[2];  // 3rd prime
    };

    BENCHMARK("primes_lt_65537 access (elemento 255)")
    {
        return NumRepr::AuxFunc::LUT::primes_lt_65537[255]; // 256th prime
    };

    BENCHMARK("primes_lt_65537 access (elemento 36)")
    {
        return NumRepr::AuxFunc::LUT::primes_lt_65537[36];  // 37th prime
    };

    BENCHMARK("primes_lt_65537 access (elemento 41)")
    {
        return NumRepr::AuxFunc::LUT::primes_lt_65537[41];  // 42nd prime
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
