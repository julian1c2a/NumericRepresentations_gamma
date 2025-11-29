
#include <catch2/catch_test_macros.hpp>
#include <iostream>

#include "core/dig_t.hpp"

using namespace NumRepr;

// =============================================================================
// IDENTIFICACIÓN DE LA SUITE
// =============================================================================
// Truco: Este constructor estático corre antes que el main de Catch2
struct SuiteInfoPrinter {
    SuiteInfoPrinter() {
        std::cout << "\n=============================================================\n"
                  << " EJECUTANDO SUITE: test_08_dig_t_operadores\n"
                  << "=============================================================" 
                  << std::endl;
    }
};
static SuiteInfoPrinter _info_printer;


// =============================================================================
// 1. ARITMÉTICA BINARIA (+, -, *, /, %)
// =============================================================================

TEST_CASE("dig_t: Aritmética Binaria Modular", "[core][dig_t][operators][arithmetic]") {

    SECTION("Suma (+)") {
        dig_t<10> a(7);
        dig_t<10> b(4);
        // 7 + 4 = 11 = 1 (mod 10)
        CHECK((a + b).get() == 1);
        
        // Conmutatividad
        CHECK((b + a).get() == 1);
        
        // Mixto con enteros
        CHECK((a + 4).get() == 1);
        CHECK((a + 14).get() == 1); // 14 mod 10 = 4
    }

    SECTION("Resta (-)") {
        dig_t<10> a(3);
        dig_t<10> b(5);
        // 3 - 5 = -2 = 8 (mod 10)
        CHECK((a - b).get() == 8);
        
        // Resta que da 0
        CHECK((a - a).get() == 0);
        
        // Mixto con enteros
        CHECK((a - 5).get() == 8);
    }

    SECTION("Multiplicación (*)") {
        dig_t<10> a(3);
        dig_t<10> b(4);
        // 3 * 4 = 12 = 2 (mod 10)
        CHECK((a * b).get() == 2);
        
        // Multiplicación por 0
        CHECK((a * dig_t<10>(0)).get() == 0);
        
        // Multiplicación por 1
        CHECK((a * dig_t<10>(1)).get() == 3);

        // Mixto con enteros
        CHECK((a * 4).get() == 2);
    }

    SECTION("División Modular (/)") {
        // Nota: La división modular a/b requiere que b sea unidad (coprimo con Base)
        
        // Base 10: Unidades son 1, 3, 7, 9
        dig_t<10> a(8); // Queremos dividir 8 entre...
        
        // ...entre 3.  8 / 3 (mod 10) = 8 * inv(3) = 8 * 7 = 56 = 6
        // Verificación: 6 * 3 = 18 = 8
        CHECK((a / dig_t<10>(3)).get() == 6);
        
        // Mixto
        CHECK((a / 3).get() == 6);
    }

    SECTION("Módulo (%)") {
        dig_t<100> a(25);
        dig_t<100> b(7);
        // 25 % 7 = 4
        CHECK((a % b).get() == 4);
        
        // Mixto
        CHECK((a % 7).get() == 4);
    }
}

// =============================================================================
// 2. INCREMENTO Y DECREMENTO (++, --)
// =============================================================================

TEST_CASE("dig_t: Incremento y Decremento", "[core][dig_t][operators][incdec]") {
    SECTION("Pre-incremento y Post-incremento") {
        dig_t<5> d(3);
        
        // Pre: incrementa y devuelve ref
        CHECK((++d).get() == 4);
        
        // Wraparound 4 -> 0
        CHECK((++d).get() == 0);
        
        // Post: devuelve copia antigua, luego incrementa
        CHECK((d++).get() == 0);
        CHECK(d.get() == 1);
    }

    SECTION("Pre-decremento y Post-decremento") {
        dig_t<5> d(1);
        
        // Pre: decrementa y devuelve ref
        CHECK((--d).get() == 0);
        
        // Wraparound 0 -> 4
        CHECK((--d).get() == 4);
        
        // Post: devuelve copia antigua
        CHECK((d--).get() == 4);
        CHECK(d.get() == 3);
    }
}

// =============================================================================
// 3. COMPARACIONES (<=>, ==, !=, <, >)
// =============================================================================

TEST_CASE("dig_t: Comparaciones", "[core][dig_t][operators][comparison]") {
    dig_t<10> small(2);
    dig_t<10> big(8);
    dig_t<10> eq(2);

    SECTION("Igualdad y Desigualdad") {
        CHECK(small == eq);
        CHECK(small != big);
        CHECK(small == 2);      // Mixto
        CHECK(small == 12);     // 12 mod 10 = 2
        CHECK(small != 3);
    }

    SECTION("Orden Relacional") {
        CHECK(small < big);
        CHECK(big > small);
        CHECK(small <= eq);
        CHECK(big >= small);
        
        // Mixto
        CHECK(small < 8);
        CHECK(big > 2);
    }

    SECTION("Spaceship Operator (<=>)") {
        CHECK((small <=> big) < 0);
        CHECK((big <=> small) > 0);
        CHECK((small <=> eq) == 0);
        
        // Mixto
        CHECK((small <=> 8) < 0);
    }
}

// =============================================================================
// 4. LÓGICA ESPECIAL (MIN/MAX/POW)
// =============================================================================

TEST_CASE("dig_t: Operadores Lógicos Especiales", "[core][dig_t][operators][special]") {
    SECTION("Bitwise AND (&) como MIN") {
        dig_t<10> a(3);
        dig_t<10> b(7);
        CHECK((a & b).get() == 3);
        CHECK((b & a).get() == 3);
        
        a &= b;
        CHECK(a.get() == 3);
    }

    SECTION("Bitwise OR (|) como MAX") {
        dig_t<10> a(3);
        dig_t<10> b(7);
        CHECK((a | b).get() == 7);
        CHECK((b | a).get() == 7);
        
        a |= b;
        CHECK(a.get() == 7);
    }

    SECTION("Bitwise XOR (^) como POTENCIA") {
        dig_t<10> base(3);
        // 3^2 = 9
        CHECK((base ^ 2u).get() == 9);
        
        // 3^3 = 27 = 7 (mod 10)
        CHECK((base ^ 3u).get() == 7);
        
        // 0^0 = 1 (Convención)
        CHECK((dig_t<10>(0) ^ 0u).get() == 1);
        
        // Asignación
        base ^= 2u; // 3^2 = 9
        CHECK(base.get() == 9);
    }
}

// =============================================================================
// 5. OPERADORES UNARIOS (-, ~, !)
// =============================================================================

TEST_CASE("dig_t: Operadores Unarios", "[core][dig_t][operators][unary]") {
    SECTION("Negación aritmética (-)") {
        // -a = B - a
        dig_t<10> d(3);
        CHECK((-d).get() == 7);
        
        dig_t<10> zero(0);
        CHECK((-zero).get() == 0);
    }

    SECTION("Complemento a B-1 (~ y !)") {
        // ~a = (B-1) - a
        dig_t<10> d(3);
        // 9 - 3 = 6
        CHECK((~d).get() == 6);
        CHECK((!d).get() == 6); // ! hace lo mismo en esta implementación
        
        dig_t<10> zero(0);
        CHECK((~zero).get() == 9);
    }
}