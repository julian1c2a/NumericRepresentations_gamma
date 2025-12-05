#include "core/dig_t.hpp"
#include <iostream>

int main() {
    try {
        // Test básico de dig_t
        NumRepr::dig_t<10> d1(5);
        NumRepr::dig_t<10> d2(3);
        auto result = d1 + d2;
        
        std::cout << "Test MSVC: " << d1.get() << " + " << d2.get() << " = " << result.get() << std::endl;
        
        if (result.get() == 8) {
            std::cout << "SUCCESS: MSVC compilation and execution working!" << std::endl;
            return 0;
        } else {
            std::cout << "FAILED: Unexpected result" << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}