#include <catch2/catch_test_macros.hpp>
#include <sstream>
#include <type_traits>

#include "core/dig_t.hpp"
#include "core/dig_t_display_helpers.hpp"

using namespace NumRepr;

// =============================================================================
// 1. CONSTRUCCIÓN Y NORMALIZACIÓN
// =============================================================================

TEST_CASE("dig_t: Construcción Básica", "[core][dig_t][constructor]") {
    
    SECTION("Base 10: Normalización Estándar") {
        // 0 -> 0
        dig_t<10> d0(0);
        CHECK(d0.get() == 0);

        // 5 -> 5
        dig_t<10> d5(5);
        CHECK(d5.get() == 5);

        // 15 mod 10 -> 5
        dig_t<10> d15(15);
        CHECK(d15.get() == 5);

        // -1 mod 10 -> 9
        dig_t<10> dm1(-1);
        CHECK(dm1.get() == 9);
    }

    SECTION("Base 2: Binario") {
        dig_t<2> d0(0);
        dig_t<2> d1(1);
        dig_t<2> d2(2);  // 0
        dig_t<2> d3(3);  // 1

        CHECK(d0.get() == 0);
        CHECK(d1.get() == 1);
        CHECK(d2.get() == 0);
        CHECK(d3.get() == 1);
    }

    SECTION("Base Hexadecimal (16)") {
        dig_t<16> dA(10);
        dig_t<16> dF(15);
        dig_t<16> d10(16); // 0

        CHECK(dA.get() == 0xA);
        CHECK(dF.get() == 0xF);
        CHECK(d10.get() == 0);
    }
}

// =============================================================================
// 2. ARITMÉTICA MODULAR
// =============================================================================

TEST_CASE("dig_t: Operadores Aritméticos", "[core][dig_t][math]") {

    SECTION("Suma (+, +=)") {
        dig_t<10> a(7);
        dig_t<10> b(4);
        
        // 7 + 4 = 11 = 1 (mod 10)
        CHECK((a + b).get() == 1);
        
        a += b;
        CHECK(a.get() == 1);

        // Conmutatividad
        CHECK((dig_t<10>(3) + dig_t<10>(5)).get() == 8);
    }

    SECTION("Resta (-, -=)") {
        dig_t<10> a(3);
        dig_t<10> b(5);
        
        // 3 - 5 = -2 = 8 (mod 10)
        CHECK((a - b).get() == 8);
        
        a -= b;
        CHECK(a.get() == 8);
    }

    SECTION("Multiplicación (*, *=)") {
        dig_t<10> a(3);
        dig_t<10> b(4);
        
        // 3 * 4 = 12 = 2 (mod 10)
        CHECK((a * b).get() == 2);
        
        a *= b;
        CHECK(a.get() == 2);
    }

    SECTION("Incremento y Decremento (++, --)") {
        dig_t<5> d(0);
        
        // 0 -- -> 4
        --d;
        CHECK(d.get() == 4);
        
        // 4 ++ -> 0
        ++d;
        CHECK(d.get() == 0);
        
        // Post-incremento
        dig_t<5> old = d++;
        CHECK(old.get() == 0);
        CHECK(d.get() == 1);
    }
    
    SECTION("Potencias (^, ^=)") {
        dig_t<7> base(2);
        // 2^3 = 8 = 1 (mod 7)
        CHECK((base ^ 3u).get() == 1);
        
        dig_t<10> base10(3);
        // 3^2 = 9 (mod 10)
        base10 ^= 2u;
        CHECK(base10.get() == 9);
    }
}

// =============================================================================
// 3. PROPIEDADES ALGEBRAICAS (ANILLOS)
// =============================================================================

TEST_CASE("dig_t: Álgebra y Primalidad", "[core][dig_t][algebra]") {
    
    SECTION("Base Prima (B=7) -> Es un Cuerpo") {
        CHECK(dig_t<7>::is_prime());
        
        dig_t<7> x(3);
        CHECK(x.is_unit()); // Todo elemento != 0 es unidad
        CHECK_FALSE(x.is_0_divisor());
        
        // Inverso: 3 * inv(3) = 1 (mod 7) -> 3 * 5 = 15 = 1
        CHECK(x.mult_inv().get() == 5);
        CHECK((x * x.mult_inv()).is_1());
    }

    SECTION("Base Compuesta (B=10)") {
        CHECK_FALSE(dig_t<10>::is_prime());
        
        dig_t<10> x(3); // Coprimo con 10 -> Unidad
        CHECK(x.is_unit());
        CHECK(x.mult_inv().get() == 7); // 3*7=21=1
        
        dig_t<10> y(2); // No coprimo -> Divisor de cero
        CHECK_FALSE(y.is_unit());
        CHECK(y.is_0_divisor()); // 2*5 = 10 = 0
        CHECK(y.mult_inv().is_0()); // Sin inverso (retorna 0)
    }
}

// =============================================================================
// 4. COMPARACIONES (SPACESHIP)
// =============================================================================

TEST_CASE("dig_t: Comparaciones y Orden", "[core][dig_t][cmp]") {
    dig_t<10> small(2);
    dig_t<10> big(8);
    dig_t<10> eq(2);

    CHECK(small == eq);
    CHECK(small != big);
    CHECK(small < big);
    CHECK(big > small);
    CHECK(small <= eq);
    CHECK(big >= small);

    // Comparación mixta con enteros
    CHECK(small == 2);
    CHECK(small == 12); // 12 mod 10 == 2
    CHECK(small != 3);
}

// =============================================================================
// 5. STRING PARSING Y VISUALIZACIÓN
// =============================================================================

TEST_CASE("dig_t: I/O y Strings", "[core][dig_t][io]") {
    
    SECTION("to_string") {
        dig_t<10> d(5);
        // Formato: "d[5]B10"
        CHECK(d.to_string() == "d[5]B10");
    }

    SECTION("to_cstr (Compile-Time)") {
        constexpr dig_t<16> d(15); // F
        constexpr auto arr = d.to_cstr();
        // Debe ser "d[15]B16" (o similar según implementación interna de dígitos)
        // Verificamos prefijo y base
        CHECK(arr[0] == 'd');
        CHECK(arr[1] == '[');
        // ... validación básica
    }

    SECTION("Parsing desde String (from_string)") {
        auto res = dig_t<10>::from_string("d[7]B10");
        REQUIRE(res.has_value());
        CHECK(res->get() == 7);

        // Formato alternativo
        auto res2 = dig_t<10>::from_string("dig#7#B10");
        REQUIRE(res2.has_value());
        CHECK(res2->get() == 7);
    }

    SECTION("Display Helpers") {
        // Verificar que display() extraiga el valor numérico,
        // evitando el problema de char/uint8_t en cout
        dig_t<10> d(65); // 'A' en ASCII si fuera char
        
        std::stringstream ss;
        ss << display(d); // Debería imprimir "5" (65%10), no 'A' ni 5 (raw)
        CHECK(ss.str() == "5");
    }
}

// =============================================================================
// 6. OPERADORES UNARIOS (Complementos)
// =============================================================================

TEST_CASE("dig_t: Operadores Unarios", "[core][dig_t][unary]") {
    SECTION("Negación (-)") {
        dig_t<10> d(3);
        // -3 = 7 (mod 10)
        CHECK((-d).get() == 7);
        CHECK((-dig_t<10>(0)).get() == 0); // -0 = 0
    }

    SECTION("Complemento a B-1 (~)") {
        dig_t<10> d(3);
        // ~3 = (10-1) - 3 = 6
        CHECK((~d).get() == 6);
        CHECK(d.C_Bm1().get() == 6);
    }
}