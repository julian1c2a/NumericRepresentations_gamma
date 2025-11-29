#include <catch2/catch_test_macros.hpp>
#include "core/internal/math/primes.hpp"
#include "core/internal/math/primes_ct.hpp"
#include <cstdint>

using namespace NumRepr::AuxFunc::LUT;

TEST_CASE("Primes: Runtime isPrime check", "[math][primes][runtime]") {
    
    SECTION("Casos base y números pequeños") {
        REQUIRE_FALSE(isPrime(0));
        REQUIRE_FALSE(isPrime(1));
        REQUIRE(isPrime(2));
        REQUIRE(isPrime(3));
        REQUIRE_FALSE(isPrime(4));
        REQUIRE(isPrime(5));
        REQUIRE_FALSE(isPrime(6));
        REQUIRE(isPrime(7));
        REQUIRE_FALSE(isPrime(8));
        REQUIRE_FALSE(isPrime(9));
        REQUIRE_FALSE(isPrime(10));
        REQUIRE(isPrime(11));
    }

    SECTION("Primos conocidos < 65537 (Lookup Table)") {
        REQUIRE(isPrime(13));
        REQUIRE(isPrime(17));
        REQUIRE(isPrime(19));
        REQUIRE(isPrime(31));
        REQUIRE(isPrime(65521)); // El mayor primo < 65536
        REQUIRE(isPrime(65519));
    }

    SECTION("Compuestos conocidos < 65537") {
        REQUIRE_FALSE(isPrime(100));
        REQUIRE_FALSE(isPrime(12345));
        REQUIRE_FALSE(isPrime(65535)); // 5 * 13107
        REQUIRE_FALSE(isPrime(65536)); // 2^16
    }

    SECTION("Primos grandes (Miller-Rabin 64-bit)") {
        REQUIRE(isPrime(104729));        // Primo nº 10000
        REQUIRE(isPrime(1299709));       // Primo nº 100000
        REQUIRE(isPrime(179424673));     // Primo grande
        REQUIRE(isPrime(32416190071ULL)); // Primo grande > 32 bits
        // Máximo primo uint64: 18446744073709551557
        REQUIRE(isPrime(18446744073709551557ULL));
    }

    SECTION("Compuestos grandes y pseudoprimos") {
        REQUIRE_FALSE(isPrime(104728));
        REQUIRE_FALSE(isPrime(1299710));
        REQUIRE_FALSE(isPrime(18446744073709551556ULL)); // Par gigante
        
        // Números de Carmichael (test de fuego para primality tests simples)
        // 561 = 3 * 11 * 17
        REQUIRE_FALSE(isPrime(561)); 
        // 1105 = 5 * 13 * 17
        REQUIRE_FALSE(isPrime(1105));
        // 1729 = 7 * 13 * 19
        REQUIRE_FALSE(isPrime(1729));
    }
}

TEST_CASE("Primes: Compile-time isPrime_ct check", "[math][primes][constexpr]") {
    
    // Verificaciones estáticas (deben compilar)
    SECTION("Casos base estáticos") {
        STATIC_REQUIRE(!isPrime_ct<0>());
        STATIC_REQUIRE(!isPrime_ct<1>());
        STATIC_REQUIRE(isPrime_ct<2>());
        STATIC_REQUIRE(isPrime_ct<3>());
        STATIC_REQUIRE(!isPrime_ct<4>());
        STATIC_REQUIRE(isPrime_ct<5>());
    }

    SECTION("Tabla LUT estática (< 65537)") {
        STATIC_REQUIRE(isPrime_ct<13>());
        STATIC_REQUIRE(isPrime_ct<101>());
        STATIC_REQUIRE(!isPrime_ct<100>());
        STATIC_REQUIRE(isPrime_ct<65521>());
    }

    SECTION("Miller-Rabin estático (>= 65537)") {
        // Primo
        constexpr uint64_t p1 = 104729; 
        STATIC_REQUIRE(isPrime_ct<p1>());

        // Compuesto
        constexpr uint64_t c1 = 104730;
        STATIC_REQUIRE(!isPrime_ct<c1>());
        
        // Primo grande
        constexpr uint64_t big_prime = 4294967291ULL; // Mayor primo de 32 bits
        STATIC_REQUIRE(isPrime_ct<big_prime>());
    }
    
    SECTION("Números de Carmichael Estáticos") {
        STATIC_REQUIRE(!isPrime_ct<561>());
        STATIC_REQUIRE(!isPrime_ct<294409>()); // 37 * 73 * 109
    }
}

TEST_CASE("Primes: Constexpr evaluation runtime check", "[math][primes][constexpr_runtime]") {
    // Verificar que la función constexpr se puede llamar en runtime y da lo mismo
    uint64_t n = 104729;
    // No podemos llamar a isPrime_ct<n>() si n no es constante,
    // pero podemos verificar constantes asignadas a variables.
    constexpr bool res = isPrime_ct<104729>();
    REQUIRE(res == true);
    REQUIRE(isPrime(n) == true);
}