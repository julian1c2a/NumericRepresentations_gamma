cmake_minimum_required(VERSION 3.25)

# Política para manejo correcto de runtimes en MSVC moderno
cmake_policy(SET CMP0091 NEW) 

project(NumericRepresentations)

# CAMBIO IMPORTANTE: Subimos a C++23 para soporte de std::expected y constexpr bitset
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- FIX PARA CLANG EN WINDOWS/MSYS2 ---
if(WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_LINK_EXECUTABLE 
    "<CMAKE_CXX_COMPILER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
endif()

# --- SALIDAS ---
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

enable_testing()

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.11.0
)
FetchContent_MakeAvailable(Catch2)

# ==========================================
# TARGETS
# ==========================================

# Tests antiguos
add_executable(test_primes_compiletime_catch2 tests/test_primes_compiletime_catch2.cpp)
target_include_directories(test_primes_compiletime_catch2 PRIVATE include tests/external)
target_link_libraries(test_primes_compiletime_catch2 PRIVATE Catch2::Catch2WithMain)

add_executable(test_lookup_tables tests/test_lookup_tables.cpp)
target_include_directories(test_lookup_tables PRIVATE include)
target_link_libraries(test_lookup_tables PRIVATE Catch2::Catch2WithMain)

# Nuevo test
add_executable(test_01_math_tables tests/test_01_math_tables.cpp)
target_include_directories(test_01_math_tables PRIVATE include)
target_link_libraries(test_01_math_tables PRIVATE Catch2::Catch2WithMain)

# ==========================================
# REGISTRO CTEST
# ==========================================
add_test(NAME test_primes_compiletime_catch2 COMMAND test_primes_compiletime_catch2)
add_test(NAME test_lookup_tables COMMAND test_lookup_tables)
add_test(NAME test_01_math_tables COMMAND test_01_math_tables)