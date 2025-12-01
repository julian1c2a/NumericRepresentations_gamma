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

# --- CONFIGURACIÓN DE FLAGS ESPECÍFICOS ---

if(MSVC)
  # Flags propuestos + /wd4146
  # Añadimos /Zc:preprocessor para asegurar conformidad total con macros
  add_compile_options(
    /permissive- 
    /bigobj 
    /constexpr:depth8192 
    /constexpr:steps50000000 
    /wd4146 
    /Zc:preprocessor
  )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # Aumentamos límites para Clang y FORZAMOS libc++
  add_compile_options(
    -fconstexpr-depth=8192
    -fconstexpr-steps=50000000
    -stdlib=libc++
  )
  add_link_options(-stdlib=libc++)
endif()

# --- SALIDAS ---
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

enable_testing()

# ==========================================
# DEPENDENCIAS (CORREGIDO PARA EVITAR MINGW EN MSVC)
# ==========================================
if(MSVC)
    # IMPORTANTE: Con MSVC, prohibimos buscar en las rutas del sistema (MinGW)
    # para evitar contaminación de cabeceras stdio.h incompatibles.
    # Solo buscará en CMAKE_PREFIX_PATH (definido en el Preset).
    find_package(Catch2 3 REQUIRED PATHS "${CMAKE_PREFIX_PATH}" NO_DEFAULT_PATH)
else()
    find_package(Catch2 3 REQUIRED)
endif()

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

# Test test_01_math_tables
add_executable(test_01_math_tables tests/test_01_math_tables.cpp)
target_include_directories(test_01_math_tables PRIVATE include)
target_link_libraries(test_01_math_tables PRIVATE Catch2::Catch2WithMain)

# Test test_02_append
add_executable(test_02_append tests/test_02_append.cpp)
target_include_directories(test_02_append PRIVATE include)
target_link_libraries(test_02_append PRIVATE Catch2::Catch2WithMain)

# Test test_03_core_internal
add_executable(test_03_core_internal tests/test_03_core_internal.cpp)
target_include_directories(test_03_core_internal PRIVATE include)
target_link_libraries(test_03_core_internal PRIVATE Catch2::Catch2WithMain)

# Test test_04_dig_t
add_executable(test_04_dig_t tests/test_04_dig_t.cpp)
target_include_directories(test_04_dig_t PRIVATE include)
target_link_libraries(test_04_dig_t PRIVATE Catch2::Catch2WithMain)

# Test test_05_dig_t_constructors
add_executable(test_05_dig_t_constructors tests/test_05_dig_t_constructors.cpp)
target_include_directories(test_05_dig_t_constructors PRIVATE include)
target_link_libraries(test_05_dig_t_constructors PRIVATE Catch2::Catch2WithMain)

# Test test_06_dig_t_assignations
add_executable(test_06_dig_t_assignations tests/test_06_dig_t_assignations.cpp)
target_include_directories(test_06_dig_t_assignations PRIVATE include)
target_link_libraries(test_06_dig_t_assignations PRIVATE Catch2::Catch2WithMain)

# Test test_07_dig_t_conversions
add_executable(test_07_dig_t_conversions tests/test_07_dig_t_conversions.cpp)
target_include_directories(test_07_dig_t_conversions PRIVATE include)
target_link_libraries(test_07_dig_t_conversions PRIVATE Catch2::Catch2WithMain)

# Test test_08_dig_t_operadores
add_executable(test_08_dig_t_operadores tests/test_08_dig_t_operadores.cpp)
target_include_directories(test_08_dig_t_operadores PRIVATE include)
target_link_libraries(test_08_dig_t_operadores PRIVATE Catch2::Catch2WithMain)

# Test test_09_dig_t_algebra
add_executable(test_09_dig_t_algebra tests/test_09_dig_t_algebra.cpp)
target_include_directories(test_09_dig_t_algebra PRIVATE include)
target_link_libraries(test_09_dig_t_algebra PRIVATE Catch2::Catch2WithMain)

# Test test_10_dig_t_io
add_executable(test_10_dig_t_io tests/test_10_dig_t_io.cpp)
target_include_directories(test_10_dig_t_io PRIVATE include)
target_link_libraries(test_10_dig_t_io PRIVATE Catch2::Catch2WithMain)

# Test test_11_basic_types
add_executable(test_11_basic_types tests/test_11_basic_types.cpp)
target_include_directories(test_11_basic_types PRIVATE include)
target_link_libraries(test_11_basic_types PRIVATE Catch2::Catch2WithMain)

# Test test_12_Int_ExpLog
add_executable(test_12_Int_ExpLog tests/test_12_Int_ExpLog.cpp)
target_include_directories(test_12_Int_ExpLog PRIVATE include)
target_link_libraries(test_12_Int_ExpLog PRIVATE Catch2::Catch2WithMain)

# Test test_13_IntRoot
add_executable(test_13_IntRoot tests/test_13_IntRoot.cpp)
target_include_directories(test_13_IntRoot PRIVATE include)
target_link_libraries(test_13_IntRoot PRIVATE Catch2::Catch2WithMain)

# Test test_14_primes
add_executable(test_14_primes tests/test_14_primes.cpp)
target_include_directories(test_14_primes PRIVATE include)
target_link_libraries(test_14_primes PRIVATE Catch2::Catch2WithMain)

# Test test_15_conversions
add_executable(test_15_conversions tests/test_15_conversions.cpp)
target_include_directories(test_15_conversions PRIVATE include)
target_link_libraries(test_15_conversions PRIVATE Catch2::Catch2WithMain)

# ==========================================
# REGISTRO CTEST
# ==========================================
add_test(NAME test_primes_compiletime_catch2 COMMAND test_primes_compiletime_catch2)
add_test(NAME test_lookup_tables COMMAND test_lookup_tables)
add_test(NAME test_01_math_tables COMMAND test_01_math_tables)
add_test(NAME test_02_append COMMAND test_02_append)
add_test(NAME test_03_core_internal COMMAND test_03_core_internal)
add_test(NAME test_04_dig_t COMMAND test_04_dig_t)
add_test(NAME test_05_dig_t_constructors COMMAND test_05_dig_t_constructors)
add_test(NAME test_06_dig_t_assignations COMMAND test_06_dig_t_assignations)
add_test(NAME test_07_dig_t_conversions COMMAND test_07_dig_t_conversions)
add_test(NAME test_08_dig_t_operadores COMMAND test_08_dig_t_operadores)
add_test(NAME test_09_dig_t_algebra COMMAND test_09_dig_t_algebra)
add_test(NAME test_10_dig_t_io COMMAND test_10_dig_t_io)
add_test(NAME test_11_basic_types COMMAND test_11_basic_types)
add_test(NAME test_12_Int_ExpLog COMMAND test_12_Int_ExpLog)
add_test(NAME test_13_IntRoot COMMAND test_13_IntRoot)
add_test(NAME test_14_primes COMMAND test_14_primes)
add_test(NAME test_15_conversions COMMAND test_15_conversions)