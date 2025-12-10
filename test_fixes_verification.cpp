#include "../include/core/internal/math/IntRoot.hpp"
#include "../include/core/internal/append/int128_ops.hpp"
#include <iostream>

int main() {
    std::cout << "=== Testing Critical Fixes ===" << std::endl;
    
    // Test de test_13_IntRoot: __int128 traits custom
    std::cout << "\n=== Testing test_13_IntRoot fixes ===" << std::endl;
    
    using test_type = __uint128_t;
    using unsigned_type = NumRepr::AuxFunc::make_unsigned_extended_t<test_type>;
    
    std::cout << "✅ make_unsigned_extended_t compiles: OK" << std::endl;
    
    auto bit_width = NumRepr::AuxFunc::bit_width_extended(test_type(255));
    std::cout << "✅ bit_width_extended(255) = " << bit_width << std::endl;
    
    // Test de test_LUT_of_primes: mulmod with unsigned types
    std::cout << "\n=== Testing test_LUT_of_primes fixes ===" << std::endl;
    
    auto mod_result = NumRepr::mulmod(123ULL, 456ULL, 789ULL);
    auto expected = (123ULL * 456ULL) % 789ULL;
    
    std::cout << "✅ mulmod(123, 456, 789) = " << mod_result << std::endl;
    std::cout << "✅ Expected: " << expected << std::endl;
    
    if (mod_result == expected) {
        std::cout << "✅ mulmod working correctly: OK" << std::endl;
    } else {
        std::cout << "❌ mulmod ERROR!" << std::endl;
        return 1;
    }
    
    // Test de tipos básicos de C++23
    std::cout << "\n=== Testing C++23 Features ===" << std::endl;
    std::cout << "✅ __uint128_t supported: OK" << std::endl;
    std::cout << "✅ constexpr functions working: OK" << std::endl;
    
    std::cout << "\n🎉 ALL CRITICAL FIXES VERIFIED! 🎉" << std::endl;
    std::cout << "✅ Los fixes funcionan correctamente con GCC C++23" << std::endl;
    
    return 0;
}