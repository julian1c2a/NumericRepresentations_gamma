#define CATCH_CONFIG_MAIN
#include "external/catch.hpp"
#include "../include/core/internal/lookup_tables/LUT_of_primes.hpp"

using namespace NumRepr::AuxFunc::LUT;
#include <vector>
#include <cstdint>
#include <random>
#include <algorithm>

TEST_CASE("isPrime correctness for small numbers", "[primes]") {
    std::vector<uint16_t> known_primes = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 97, 101, 127, 131, 199, 211, 997, 65521};
    for (auto p : known_primes) {
        REQUIRE(isPrime(static_cast<uint16_t>(p)));
    }
    std::vector<uint16_t> known_composites = {0, 1, 4, 6, 8, 9, 10, 12, 14, 15, 16, 18, 20, 21, 22, 24, 25, 26, 27, 28, 30, 32, 100, 200, 1000, 65535};
    for (auto c : known_composites) {
        REQUIRE_FALSE(isPrime(static_cast<uint16_t>(c)));
    }
}

TEST_CASE("isPrime correctness for large primes and composites", "[primes]") {
    std::vector<uint64_t> large_primes = {6700417ULL, 2147483647ULL, 2305843009213693951ULL, 4294967311ULL, 9999999967ULL};
    for (auto p : large_primes) {
        REQUIRE(isPrime(p));
    }
    std::vector<uint64_t> large_composites = {6700418ULL, 2147483646ULL, 2305843009213693950ULL, 4294967312ULL, 9999999968ULL};
    for (auto c : large_composites) {
        REQUIRE_FALSE(isPrime(c));
    }
}

TEST_CASE("isPrime edge cases", "[primes]") {
    REQUIRE_FALSE(isPrime(0));
    REQUIRE_FALSE(isPrime(1));
    REQUIRE(isPrime(2));
    REQUIRE(isPrime(3));
    REQUIRE_FALSE(isPrime(4));
    REQUIRE(isPrime(5));
    REQUIRE_FALSE(isPrime(UINT64_MAX));
}

TEST_CASE("isPrime random stress test", "[primes][random]") {
    std::mt19937_64 rng(42);
    std::uniform_int_distribution<uint64_t> dist(2, 1000000);
    for (int i = 0; i < 1000; ++i) {
        uint64_t n = dist(rng);
        bool expected = true;
        if (n < 2) expected = false;
        for (uint64_t d = 2; d * d <= n; ++d) {
            if (n % d == 0) { expected = false; break; }
        }
        REQUIRE(isPrime(n) == expected);
    }
}

TEST_CASE("mulmod and binpower properties", "[modular]") {
    REQUIRE(mulmod(2, 3, 5) == (2 * 3) % 5);
    REQUIRE(mulmod(123456789, 987654321, 1000000007) == (123456789ULL * 987654321ULL) % 1000000007ULL);
    REQUIRE(NumRepr::AuxFunc::LUT::binpower_ct<2, 10, 1000>() == 24);
    REQUIRE(NumRepr::AuxFunc::LUT::binpower_ct<3, 7, 13>() == 3);
}

TEST_CASE("mulmod_ct and binpower_ct compile-time evaluation", "[constexpr]") {
    constexpr uint64_t m1 = NumRepr::AuxFunc::LUT::mulmod_ct<123, 456, 789, 0>();
    constexpr uint64_t m2 = NumRepr::AuxFunc::LUT::mulmod_ct<987654321, 123456789, 1000000007, 0>();
    STATIC_REQUIRE(m1 == (123ULL * 456ULL) % 789ULL);
    STATIC_REQUIRE(m2 == (987654321ULL * 123456789ULL) % 1000000007ULL);
}
