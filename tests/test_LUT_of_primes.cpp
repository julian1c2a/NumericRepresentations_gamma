#include "catch2_wrapper.hpp"
#include "core/internal/math/primes.hpp"
#include "core/internal/append/int128_ops.hpp"  // Para mulmod
#include <vector>
#include <cstdint>
#include <random>
#include <algorithm>
#include <iostream>

using namespace NumRepr::AuxFunc::LUT;
using NumRepr::mulmod;  // Agregar mulmod al scope

// =============================================================================
// IDENTIFICACIÓN DE LA SUITE
// =============================================================================
// Truco: Este constructor estático corre antes que el main de Catch2
struct SuiteInfoPrinter {
    SuiteInfoPrinter() {
        std::cout << "\n=============================================================\n"
                  << " EJECUTANDO SUITE: test_LUT_of_primes\n"
                  << "=============================================================" 
                  << std::endl;
    }
};
static SuiteInfoPrinter _info_printer;

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
    REQUIRE(mulmod(2U, 3U, 5U) == (2U * 3U) % 5U);
    REQUIRE(mulmod(123456789ULL, 987654321ULL, 1000000007ULL) == (123456789ULL * 987654321ULL) % 1000000007ULL);
    
    // Test binpower (existe en NumRepr::AuxFunc::LUT)
    REQUIRE(binpower(2U, 10U, 1000U) == 24U);  // 2^10 % 1000 = 1024 % 1000 = 24
    REQUIRE(binpower(3U, 7U, 13U) == 3U);      // 3^7 % 13 = 2187 % 13 = 3
}

TEST_CASE("Modular arithmetic functions", "[modular]") {
    // Usar mulmod en runtime sin template parameters
    uint64_t m1 = mulmod(123ULL, 456ULL, 789ULL);
    uint64_t m2 = mulmod(987654321ULL, 123456789ULL, 1000000007ULL);
    REQUIRE(m1 == (123ULL * 456ULL) % 789ULL);
    REQUIRE(m2 == (987654321ULL * 123456789ULL) % 1000000007ULL);
}
