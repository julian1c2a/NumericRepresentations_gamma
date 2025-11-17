cmake_minimum_required(VERSION 3.25)
project(NumericRepresentations)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---
# NUEVO: Forzar la salida de los ejecutables (.exe) al directorio
# CMAKE_BINARY_DIR. El CMAKE_BINARY_DIR se define en tu CMakePresets.json
# para que coincida con la estructura que pediste.
# ---
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

enable_testing()

# Add Catch2
include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.11.0
)
FetchContent_MakeAvailable(Catch2)

# ---
# MODIFICADO: Eliminada la librería 'numeric_lib' y el test 'test_dig_t'.
# Solo queda el test para lookup_tables.
# ---

# ORDEN CORRECTO:
# 1. Primero, CREA el objetivo (el ejecutable)
add_executable(test_lookup_tables tests/test_lookup_tables.cpp)

# 2. Después, MODIFICA el objetivo (añadiendo directorios de inclusión)
target_include_directories(test_lookup_tables PRIVATE include)

# 3. Finalmente, MODIFICA el objetivo (añadiendo librerías enlazadas)
target_link_libraries(test_lookup_tables PRIVATE Catch2::Catch2WithMain)

add_test(NAME test_lookup_tables COMMAND test_lookup_tables)