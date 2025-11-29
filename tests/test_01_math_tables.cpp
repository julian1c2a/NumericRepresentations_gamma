#include <catch2/catch_test_macros.hpp>

// Incluimos los headers a probar
#include "core/internal/math/Int_ExpLog.hpp"
#include "core/internal/math/tables/PrimeList.hpp"
#include "core/internal/math/tables/EratosthenesSieve.hpp"

#include <limits>
#include <type_traits>

// Usamos namespaces principales
using namespace NumRepr::AuxFunc;
using namespace NumRepr::AuxFunc::LUT;

// Alias para evitar ambigüedades entre funciones runtime y compile-time
// La función runtime está en Safety
using NumRepr::AuxFunc::Safety::max_exponent_for_base; 

// =============================================================================
// 1. PRUEBAS DE TABLA DE EXPONENTES MÁXIMOS
// =============================================================================

TEST_CASE("Tabla MaxExp4Base: Verificaciones en Compile-Time", "[tables][math][ct]") {
    // Accedemos explícitamente al namespace 'ct' para las versiones template consteval
    
    // Base 2: 2^63 cabe en uint64, 2^64 no. Max exp = 63.
    static_assert(NumRepr::AuxFunc::ct::max_exponent_for_base<2>() == 63, "Error en base 2 CT");
    
    // Base 3: 3^40 cabe, 3^41 no. Max exp = 40.
    static_assert(NumRepr::AuxFunc::ct::max_exponent_for_base<3>() == 40, "Error en base 3 CT");
    
    // Base 10: 10^19 cabe, 10^20 no.
    static_assert(NumRepr::AuxFunc::ct::max_exponent_for_base<10>() == 19, "Error en base 10 CT");
    
    // Límites superiores
    // Base 2^32 (4294967296): Al cuadrado es 2^64 (overflow). Max exp = 1.
    static_assert(NumRepr::AuxFunc::ct::max_exponent_for_base<4294967296ULL>() == 1, "Error en base 2^32 CT");
}

TEST_CASE("Tabla MaxExp4Base: Verificaciones en Runtime", "[tables][math][runtime]") {
    // Verificamos que la versión runtime (Safety::max_exponent_for_base) funcione
    
    CHECK(max_exponent_for_base(2) == 63);
    CHECK(max_exponent_for_base(3) == 40);
    CHECK(max_exponent_for_base(10) == 19);
    
    // Caso borde: Base muy grande
    CHECK(max_exponent_for_base(std::numeric_limits<uint64_t>::max()) == 1);
    
    // Caso borde: Base 0 y 1 (definidos como infinito ~0ull para seguridad inversa)
    CHECK(max_exponent_for_base(0) == ~0ull);
    CHECK(max_exponent_for_base(1) == ~0ull);
}

// =============================================================================
// 2. PRUEBAS DE EXPONENCIACIÓN Y SEGURIDAD
// =============================================================================

TEST_CASE("IntExpIntLog: Potencias Seguras (int_pow)", "[math][pow]") {
    
    SECTION("Cálculos correctos dentro del rango (Runtime)") {
        CHECK(int_pow(2, 10) == 1024);
        CHECK(int_pow(10, 5) == 100000);
        CHECK(int_pow(3, 3) == 27);
        CHECK(int_pow(5, 0) == 1);
        CHECK(int_pow(0, 5) == 0); // 0^n = 0
        CHECK(int_pow(0, 0) == 1); // 0^0 = 1 por convención común
    }

    SECTION("Detección de Overflow usando la tabla (Runtime)") {
        // 2^64 debe fallar (retornar 0)
        // max_exponent_for_base(2) es 63. Pedimos 64.
        CHECK(int_pow(2, 64) == 0);
        
        // 10^20 debe fallar (retornar 0)
        CHECK(int_pow(10, 20) == 0);
    }
    
    SECTION("Compile-time int_pow") {
        // Usamos NumRepr::AuxFunc::ct::int_pow
        constexpr uint64_t val = NumRepr::AuxFunc::ct::int_pow<2, 10>();
        static_assert(val == 1024, "ct::int_pow<2,10> falló");
        
        // La versión CT fallaría al compilar si hay overflow gracias a 'requires'
        // constexpr uint64_t overflow = NumRepr::AuxFunc::ct::int_pow<2, 64>(); 
    }
}

TEST_CASE("IntExpIntLog: Logaritmos Enteros (int_log2)", "[math][log]") {
    CHECK(int_log2(1) == 0);
    CHECK(int_log2(2) == 1);
    CHECK(int_log2(3) == 1);
    CHECK(int_log2(4) == 2);
    CHECK(int_log2(1024) == 10);
    CHECK(int_log2(1025) == 10);
    
    // Check de versión safe
    auto res = int_log2_sf(0);
    CHECK_FALSE(res.has_value()); // Debe dar error de dominio
    
    CHECK(bit_width(0) == 0);
    CHECK(bit_width(1) == 1); // 1 bit (el 1)
    CHECK(bit_width(4) == 3); // 100 (3 bits)
}

// =============================================================================
// 3. PRUEBAS DE TABLAS DE PRIMOS (LISTA vs CRIBA)
// =============================================================================

TEST_CASE("Tablas de Primos: Consistencia", "[tables][primes]") {
    
    SECTION("Verificar PrimeList básica") {
        // Verificar algunos primos conocidos
        CHECK(primes_lt_65537[0] == 2);
        CHECK(primes_lt_65537[1] == 3);
        CHECK(primes_lt_65537[2] == 5);
        
        // Verificar ordenación
        for (size_t i = 0; i < 100; ++i) {
            CHECK(primes_lt_65537[i] < primes_lt_65537[i+1]);
        }
    }

    SECTION("Criba de Eratóstenes (Compile-Time vs Runtime)") {
        // Generamos la criba en tiempo de ejecución para comparar
        auto runtime_sieve = make_prime_bitset();
        
        // is_prime_lt_65537_lut es constexpr bitset importado de EratosthenesSieve.hpp
        
        // 1. Verificar que la criba Runtime coincide con la lista de primos
        for (auto p : primes_lt_65537) {
            if (p < 65536) {
                CHECK(runtime_sieve[p] == true);
            }
        }
        
        // 2. Verificar algunos no-primos
        CHECK(runtime_sieve[0] == false);
        CHECK(runtime_sieve[1] == false);
        CHECK(runtime_sieve[4] == false);
        CHECK(runtime_sieve[9] == false);
        
        // 3. Verificar la tabla LUT (constexpr) contra la Runtime
        // Esto asegura que el string de bits generado offline es correcto
        for (size_t i = 0; i < 1000; ++i) {
            if (runtime_sieve[i] != is_prime_lt_65537_lut[i]) {
                FAIL("Discrepancia en índice " << i << ": Runtime=" << runtime_sieve[i] << " vs LUT=" << is_prime_lt_65537_lut[i]);
            }
        }
        
        // 4. Verificar la generación Compile-Time in-situ
        constexpr auto ct_sieve = make_prime_bitset_ct<100>(); // Criba pequeña CT
        static_assert(ct_sieve[2] == true, "2 es primo (CT)");
        static_assert(ct_sieve[3] == true, "3 es primo (CT)");
        static_assert(ct_sieve[4] == false, "4 no es primo (CT)");
    }
}