#include "core/dig_t.hpp"
#include <iostream>
#include <cassert>

// Simple test runner without Catch2
struct TestResult {
    int total = 0;
    int passed = 0;
    void check(bool condition, const char* description) {
        total++;
        if (condition) {
            passed++;
            std::cout << "[PASS] " << description << std::endl;
        } else {
            std::cout << "[FAIL] " << description << std::endl;
        }
    }
    void summary() {
        std::cout << "\n=============================================================\n";
        std::cout << " EJECUTANDO SUITE: test_04_dig_t_msvc\n";
        std::cout << "=============================================================\n";
        std::cout << "All tests " << (passed == total ? "passed" : "FAILED") 
                  << " (" << passed << "/" << total << " assertions)\n";
    }
};

int main() {
    try {
        TestResult tr;
        
        // Test básico de construcción
        NumRepr::dig_t<10> d1(5);
        tr.check(d1.get() == 5, "Construction with value 5");
        
        // Test de suma
        NumRepr::dig_t<10> d2(3);
        auto sum = d1 + d2;
        tr.check(sum.get() == 8, "Addition 5 + 3 = 8");
        
        // Test de módulo
        NumRepr::dig_t<10> d3(15);  // Should normalize to 5
        tr.check(d3.get() == 5, "Modular construction 15 mod 10 = 5");
        
        // Test de multiplicación
        NumRepr::dig_t<10> d4(4);
        auto mult = d2 * d4;  // 3 * 4 = 12 mod 10 = 2
        tr.check(mult.get() == 2, "Multiplication 3 * 4 mod 10 = 2");
        
        // Test de comparación
        tr.check(d1 > d2, "Comparison 5 > 3");
        tr.check(d2 < d1, "Comparison 3 < 5");
        
        tr.summary();
        return (tr.passed == tr.total) ? 0 : 1;
        
    } catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}