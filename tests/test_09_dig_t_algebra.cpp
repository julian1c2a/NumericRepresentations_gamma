#include <catch2/catch_test_macros.hpp>
#include <iostream>

// =============================================================================
// IDENTIFICACIÓN DE LA SUITE
// =============================================================================
// Truco: Este constructor estático corre antes que el main de Catch2
struct SuiteInfoPrinter {
    SuiteInfoPrinter() {
        std::cout << "\n=============================================================\n"
                  << " EJECUTANDO SUITE: test_09_dig_t_algebra\n"
                  << "=============================================================" 
                  << std::endl;
    }
};
static SuiteInfoPrinter _info_printer;

#include "core/dig_t.hpp"

using namespace NumRepr;

// =============================================================================
// 1. PRIMALIDAD DE LA BASE (Compile-Time)
// =============================================================================

TEST_CASE("dig_t: Primalidad de la Base", "[core][dig_t][algebra][prime]") {
    
    SECTION("Bases Primas") {
        STATIC_CHECK(dig_t<2>::isPrime());
        STATIC_CHECK(dig_t<3>::isPrime());
        STATIC_CHECK(dig_t<5>::isPrime());
        STATIC_CHECK(dig_t<7>::isPrime());
        STATIC_CHECK(dig_t<13>::isPrime());
        STATIC_CHECK(dig_t<257>::isPrime()); // Fermat prime F3
    }

    SECTION("Bases Compuestas") {
        STATIC_CHECK_FALSE(dig_t<4>::isPrime());
        STATIC_CHECK_FALSE(dig_t<6>::isPrime());
        STATIC_CHECK_FALSE(dig_t<10>::isPrime());
        STATIC_CHECK_FALSE(dig_t<100>::isPrime());
        STATIC_CHECK_FALSE(dig_t<256>::isPrime()); // 2^8
    }
}

// =============================================================================
// 2. UNIDADES Y DIVISORES DE CERO (Anillos)
// =============================================================================

TEST_CASE("dig_t: Unidades y Divisores de Cero", "[core][dig_t][algebra][ring]") {

    SECTION("Base Prima (B=7) -> Cuerpo Finito GF(7)") {
        // En un cuerpo, todo elemento != 0 es unidad.
        // Solo 0 es divisor de cero (trivial).
        
        using GF7 = dig_t<7>;
        
        for(uint8_t i = 1; i < 7; ++i) {
            GF7 x(i);
            CHECK(x.is_unit());
            CHECK_FALSE(x.is_0_divisor());
        }
        
        GF7 zero(0);
        CHECK_FALSE(zero.is_unit());
        CHECK(zero.is_0_divisor());
    }

    SECTION("Base Compuesta (B=10)") {
        // Unidades: Coprimos con 10 -> {1, 3, 7, 9}
        // Divisores de Cero: No coprimos o 0 -> {0, 2, 4, 5, 6, 8}
        
        using Z10 = dig_t<10>;

        // Unidades
        CHECK(Z10(1).is_unit());
        CHECK(Z10(3).is_unit());
        CHECK(Z10(7).is_unit());
        CHECK(Z10(9).is_unit());
        
        CHECK_FALSE(Z10(1).is_0_divisor());
        CHECK_FALSE(Z10(3).is_0_divisor());
        
        // Divisores de Cero
        CHECK(Z10(2).is_0_divisor()); // 2*5 = 10 = 0
        CHECK(Z10(5).is_0_divisor()); // 5*2 = 10 = 0
        CHECK(Z10(6).is_0_divisor()); // 6*5 = 30 = 0
        CHECK(Z10(0).is_0_divisor());
        
        CHECK_FALSE(Z10(2).is_unit());
        CHECK_FALSE(Z10(5).is_unit());
    }

    SECTION("Base Potencia de 2 (B=8)") {
        // En Z/2^n Z, solo los impares son unidades.
        using Z8 = dig_t<8>;
        
        CHECK(Z8(1).is_unit());
        CHECK(Z8(3).is_unit());
        CHECK(Z8(5).is_unit());
        CHECK(Z8(7).is_unit());
        
        CHECK(Z8(2).is_0_divisor()); // 2*4 = 8 = 0
        CHECK(Z8(4).is_0_divisor()); // 4*2 = 8 = 0
        CHECK(Z8(6).is_0_divisor()); // 6*4 = 24 = 0
    }
}

// =============================================================================
// 3. INVERSO MULTIPLICATIVO
// =============================================================================

TEST_CASE("dig_t: Inverso Modular", "[core][dig_t][algebra][inverse]") {
    
    SECTION("Existencia y Corrección") {
        using Z10 = dig_t<10>;
        
        // 3 * x = 1 (mod 10) -> x=7 (21=1)
        CHECK(Z10(3).mult_inv().get() == 7);
        CHECK((Z10(3) * Z10(3).mult_inv()).is_1());

        // 7 * x = 1 (mod 10) -> x=3
        CHECK(Z10(7).mult_inv().get() == 3);
        CHECK((Z10(7) * Z10(7).mult_inv()).is_1());
        
        // 9 * x = 1 (mod 10) -> x=9 (81=1)
        CHECK(Z10(9).mult_inv().get() == 9);
    }

    SECTION("No Existencia (devuelve 0)") {
        using Z10 = dig_t<10>;
        // 2 no tiene inverso mod 10
        CHECK(Z10(2).mult_inv().is_0());
        
        // 5 no tiene inverso mod 10
        CHECK(Z10(5).mult_inv().is_0());
        
        // 0 nunca tiene inverso
        CHECK(Z10(0).mult_inv().is_0());
    }
    
    SECTION("Base 257 (Primo)") {
        // Todos (salvo 0) tienen inverso
        using Z257 = dig_t<257>;
        Z257 a(50);
        Z257 inv = a.mult_inv();
        
        CHECK_FALSE(inv.is_0());
        CHECK((a * inv).is_1());
    }
}

// =============================================================================
// 4. SUMA CON ACARREO (Static Logic)
// =============================================================================

TEST_CASE("dig_t: Suma con Acarreo (sum_carry)", "[core][dig_t][algebra][carry]") {
    
    SECTION("Base Par (B=10)") {
        using Z10 = dig_t<10>;
        
        // Sin acarreo: 3 + 4 = 7 < 10
        CHECK(Z10::sum_carry(Z10(3), Z10(4)).is_0());
        
        // Con acarreo exacto: 5 + 5 = 10
        CHECK(Z10::sum_carry(Z10(5), Z10(5)).is_1());
        
        // Con acarreo sobrado: 7 + 8 = 15
        CHECK(Z10::sum_carry(Z10(7), Z10(8)).is_1());
        
        // Borde: 9 + 0 = 9 (Sin acarreo)
        CHECK(Z10::sum_carry(Z10(9), Z10(0)).is_0());
        
        // Borde: 9 + 1 = 10 (Con acarreo)
        CHECK(Z10::sum_carry(Z10(9), Z10(1)).is_1());
    }

    SECTION("Base Impar (B=7)") {
        using Z7 = dig_t<7>;
        
        // Sin acarreo: 3 + 2 = 5 < 7
        CHECK(Z7::sum_carry(Z7(3), Z7(2)).is_0());
        
        // Con acarreo: 4 + 4 = 8 >= 7
        CHECK(Z7::sum_carry(Z7(4), Z7(4)).is_1());
        
        // Caso Borde: 6 + 0 = 6 < 7
        CHECK(Z7::sum_carry(Z7(6), Z7(0)).is_0());
        
        // Caso Borde: 6 + 1 = 7 >= 7
        CHECK(Z7::sum_carry(Z7(6), Z7(1)).is_1());
    }
    
    SECTION("Acarreo con tipos grandes (Prevención de Overflow)") {
        // Usar una base grande para probar lógica interna de tipos nextsz
        using ZLarge = dig_t<65000>; // uint16_t subyacente
        
        ZLarge max(64999);
        ZLarge one(1);
        
        // 64999 + 1 = 65000 = Base -> Acarreo 1
        CHECK(ZLarge::sum_carry(max, one).is_1());
        
        // 30000 + 30000 = 60000 < 65000 -> Acarreo 0
        CHECK(ZLarge::sum_carry(ZLarge(30000), ZLarge(30000)).is_0());
    }
}