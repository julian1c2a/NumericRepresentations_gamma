#pragma once

// Wrapper header for Catch2 compatibility
// Works with both individual headers (GCC) and amalgamated header (MSVC)

#if defined(_MSC_VER)
    // MSVC: Use amalgamated header (implementation in separate .cpp file)
    #include "catch_amalgamated.hpp"
#else
    // GCC/Clang: Use individual headers
    #include <catch2/catch_test_macros.hpp>
#endif