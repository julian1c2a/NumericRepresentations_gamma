#include <iostream>
#include <core/dig_t.hpp>

int main() {
    try {
        std::cout << "Testing basic compilation..." << std::endl;
        
        // Test basic dig_t construction with explicit base
        NumRepr::dig_t<10> a{42};
        NumRepr::dig_t<10> b{13};
        
        // Test basic arithmetic
        auto sum = a + b;
        
        std::cout << "Basic test passed: " << a.to_string() << " + " << b.to_string() 
                  << " = " << sum.to_string() << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}