# AI Agent Instructions for NumericRepresentations

## Project Overview

This is a **Modern C++23 template metaprogramming library** for arbitrary precision numeric representations. The core is `dig_t<B>` - a digit class template that implements modular arithmetic for any radix B ∈ [2, 4294967296].

**Architecture Philosophy**: Build complex number types (naturals, integers, rationals) from a foundational digit primitive with compile-time optimizations and type safety.

## Core Architecture

### Central Template Class: `dig_t<B>`
- **Location**: `include/core/dig_t.hpp` (3383 lines - the project's heart)
- **Purpose**: Single digit in base B with modular arithmetic
- **Key Constraint**: `B <= UINT32_MAX` (ensures (B-1)×(B-1) fits in uint64_t)
- **Member**: `uint_t m_d` - always normalized to [0, B-1]

```cpp
// Core usage pattern
dig_t<10> d(15);    // Automatically normalized: 15 % 10 = 5
auto sum = d + dig_t<10>(7);  // Modular: (5 + 7) % 10 = 2
```

### Type System Metaprogramming
- **Location**: `include/core/internal/basic_types.hpp`
- **Pattern**: Automatic type selection based on base size
  - B ∈ [2,256] → `uint8_t`
  - B ∈ [257,65536] → `uint16_t` 
  - B ∈ [65537,4294967296] → `uint32_t`

### Modular Structure
```
include/
├── core/                    # Foundation: dig_t and utilities
│   ├── dig_t.hpp           # Main template class (3383 lines)
│   ├── internal/           # Type traits, math utilities
└── {fixed_length,unlimited_length,variants}/ # Future number types
```

## Build System (Multi-Compiler)

### Supported Toolchains
- **MSVC**: Requires `/constexpr:depth8192 /constexpr:steps50000000 /bigobj`
- **GCC/Clang**: Requires `-fconstexpr-depth=8192 -fconstexpr-steps=50000000`
- **Standard**: C++23 (uses `std::expected`, `constexpr` bitwise ops)

### Build Commands
```bash
# CMake with presets (recommended)
cmake --preset gcc-release                  # or clang-release, msvc-release
cmake --build --preset gcc-release

# Meson alternative
meson setup builddir --buildtype=release
ninja -C builddir

# All tests at once
./build_tests.bash gcc-release              # Builds all test_XX targets
./check_tests.bash gcc-release              # Runs all tests
```

### VS Code Tasks
- **"C/C++: g++.exe compilar archivo activo"** - Quick compile current file
- **"Test LUT_of_primes.cpp"** - Specific test compilation pattern

## Testing Patterns

### Test Organization
- **Naming**: `test_XX_feature.cpp` (e.g., `test_04_dig_t.cpp`, `test_10_dig_t_io.cpp`)
- **Framework**: Catch2 with suite identification headers
- **Structure**: Each file tests one major component/feature

```cpp
// Standard test pattern in all files
#include <catch2/catch_test_macros.hpp>
#include "core/dig_t.hpp"

// Suite identification (runs before main)
struct SuiteInfoPrinter {
    SuiteInfoPrinter() {
        std::cout << "\n=============================================================\n"
                  << " EJECUTANDO SUITE: test_XX_feature\n" 
                  << "=============================================================" << std::endl;
    }
};
static SuiteInfoPrinter _info_printer;

TEST_CASE("Feature: Specific Test", "[core][component][feature]") {
    SECTION("Specific behavior") {
        // Test implementation
    }
}
```

## Code Generation Patterns

### Template Constraints
```cpp
template<std::uint64_t B>
    requires (B > 1)  // Always validate base
struct dig_t {
    static_assert(B-1 <= std::numeric_limits<std::uint32_t>::max(),
                  "Base too large for multiplication safety");
    // ...
};
```

### I/O Format System (Dual Support)
- **Output**: Always `d[value]Bbase` format (strict)
- **Input**: Accepts both `d[value]Bbase` and `dig#value#Bbase` (legacy)
- **Parsing**: Finite state machine in `parse_impl_ct()` for compile-time parsing

```cpp
// Both inputs produce same result
dig_t<10> d1("d[7]B10");      // Modern format
dig_t<10> d2("dig#7#B10");    // Legacy format  
constexpr dig_t<10> d3("d[7]B10");  // Compile-time parsing
```

### Error Handling
- **Philosophy**: `std::expected<T, parse_error_t>` instead of exceptions
- **Location**: Error codes in `enum class parse_error_t`
- **Pattern**: Detailed error reporting for invalid inputs

## Development Workflows

### Adding New Features
1. **Header-only**: Implementation in `include/core/` or appropriate subdirectory
2. **Test first**: Create `test_XX_newfeature.cpp` following existing patterns
3. **Update CMakeLists.txt**: Add test target following existing pattern
4. **Multi-compiler**: Test with all three toolchains using build scripts

### Debugging Compile-Time Issues
- **MSVC**: May hit constexpr limits - increase `/constexpr:steps` 
- **Templates**: Use `static_assert` with clear messages
- **Types**: Examine `basic_types.hpp` for type trait utilities

### Performance Considerations
- **Constexpr**: Maximize compile-time computation
- **Template specialization**: Used for base-specific optimizations
- **Modular arithmetic**: All operations are mod B by design

## Project Conventions

### Documentation
- **Doxygen**: Comprehensive API documentation in headers
- **Analysis docs**: `doc/dig_t_*.md` files explain implementation details
- **Testing coverage**: `TESTING_COVERAGE.md` tracks test status

### Naming Conventions
- **Types**: `snake_case_t` (e.g., `dig_t`, `parse_error_t`)
- **Members**: `m_` prefix for private members
- **Functions**: `snake_case` for functions, `PascalCase` for template metafunctions

### Code Style
- **Modern C++**: Use concepts, `std::expected`, three-way comparison
- **Type safety**: Explicit conversions, template constraints
- **Const correctness**: Methods const when possible, constexpr when feasible

When modifying this codebase:
1. Understand the modular arithmetic semantics (operations are mod B)
2. Respect the type system - don't bypass automatic type selection
3. Add comprehensive tests following existing patterns
4. Test across all three supported compilers
5. Document any new template constraints or error conditions