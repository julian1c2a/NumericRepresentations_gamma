#pragma once

// Wrapper header for Catch2 compatibility
// Works with both individual headers (GCC) and amalgamated header (MSVC/Clang)

#if defined(_MSC_VER) || defined(__clang__)
    // MSVC/Clang: Use amalgamated header (implementation in separate .cpp file)
    #include "catch_amalgamated.hpp"
#else
    // GCC: Use individual headers
    #include <catch2/catch_test_macros.hpp>
#endif