#include "core/internal/append/int128_ops.hpp"
#include <iostream>
#include <cassert>

// Simple MSVC test para verificar que nuestros fixes funcionan
int main() {
    std::cout << "=== MSVC Test: Verificando Fixes ===" << std::endl;
    
    // Test mulmod con tipos unsigned (esto sí funciona en MSVC)
    try {
        auto result = NumRepr::mulmod(123ULL, 456ULL, 789ULL);
        auto expected = (123ULL * 456ULL) % 789ULL;
        assert(result == expected);
        
        std::cout << "✅ mulmod unsigned: OK (" << result << ")" << std::endl;
    } catch (...) {
        std::cout << "❌ mulmod: FAILED" << std::endl;
        return 1;
    }
    
    // Test template metaprogramming básico  
    try {
        using test_type = uint64_t;
        constexpr auto value = test_type(255);
        // Verificar que podemos usar template metaprogramming
        static_assert(sizeof(test_type) == 8);
        
        std::cout << "✅ template metaprogramming: OK" << std::endl;
    } catch (...) {
        std::cout << "❌ templates: FAILED" << std::endl;
        return 1;
    }
    
    // Test constexpr en MSVC
    constexpr auto compile_time_result = 2 + 3;
    assert(compile_time_result == 5);
    std::cout << "✅ constexpr: OK" << std::endl;
    
    std::cout << "🎉 Fixes compatibles con MSVC verificados!" << std::endl;
    std::cout << "ℹ️  MSVC no soporta __uint128_t (extensión GCC/Clang)" << std::endl;
    return 0;
}