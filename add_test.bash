#!/bin/bash

# =============================================================================
# ADD_TEST.BASH - Script para añadir automáticamente un nuevo test
# =============================================================================
# Uso: ./add_test.bash <test_name> [compiler] [mode] [benchmark] [print]
# 
# Parámetros:
#   test_name  : Nombre del test (ej: test_16_rationals) [REQUERIDO]
#   compiler   : gcc, clang, msvc, all [default: gcc]
#   mode       : debug, release [default: release]
#   benchmark  : bench, nobench [default: nobench]
#   print      : print, noprint [default: noprint]
#
# Ejemplos:
#   ./add_test.bash test_16_rationals
#   ./add_test.bash test_17_fixed_point gcc debug bench print
# =============================================================================

# Colores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Función para mostrar ayuda
show_help() {
    echo -e "${CYAN}ADD_TEST.BASH - Automatización de creación de tests${NC}"
    echo ""
    echo "Uso: ./add_test.bash <test_name> [compiler] [mode] [benchmark] [print]"
    echo ""
    echo "Parámetros:"
    echo "  test_name  : Nombre del test (ej: test_16_rationals) [REQUERIDO]"
    echo "  compiler   : gcc, clang, msvc, all [default: gcc]"
    echo "  mode       : debug, release [default: release]"
    echo "  benchmark  : bench, nobench [default: nobench]" 
    echo "  print      : print, noprint [default: noprint]"
    echo ""
    echo "Ejemplos:"
    echo "  ./add_test.bash test_16_rationals"
    echo "  ./add_test.bash test_17_fixed_point gcc debug bench print"
    echo ""
    echo "El script:"
    echo "  1. Crea el archivo test_XX_name.cpp con estructura estándar"
    echo "  2. Lo añade automáticamente a tests/meson.build"
    echo "  3. Lo compila y ejecuta usando los scripts unificados"
}

# Verificar argumentos
if [ $# -eq 0 ] || [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
    show_help
    exit 0
fi

# Parsing de argumentos con valores por defecto
TEST_NAME="$1"
COMPILER="${2:-gcc}"
MODE="${3:-release}" 
BENCHMARK="${4:-nobench}"
PRINT_FLAG="${5:-noprint}"

# Validación del nombre del test
if [[ ! "$TEST_NAME" =~ ^test_[0-9]+_.+ ]]; then
    echo -e "${RED}❌ ERROR: El nombre debe seguir el patrón 'test_XX_nombre' (ej: test_16_rationals)${NC}"
    exit 1
fi

# Validar parámetros
if [[ ! "$COMPILER" =~ ^(gcc|clang|msvc|all)$ ]]; then
    echo -e "${RED}❌ ERROR: Compilador debe ser: gcc, clang, msvc, all${NC}"
    exit 1
fi

if [[ ! "$MODE" =~ ^(debug|release)$ ]]; then
    echo -e "${RED}❌ ERROR: Modo debe ser: debug, release${NC}"
    exit 1
fi

if [[ ! "$BENCHMARK" =~ ^(bench|nobench)$ ]]; then
    echo -e "${RED}❌ ERROR: Benchmark debe ser: bench, nobench${NC}"
    exit 1
fi

if [[ ! "$PRINT_FLAG" =~ ^(print|noprint)$ ]]; then
    echo -e "${RED}❌ ERROR: Print debe ser: print, noprint${NC}"
    exit 1
fi

# Archivos y paths
TEST_FILE="tests/${TEST_NAME}.cpp"
MESON_FILE="tests/meson.build"

# Extraer información del nombre
TEST_NUMBER=$(echo "$TEST_NAME" | grep -o 'test_[0-9]\+' | grep -o '[0-9]\+')
TEST_FEATURE=$(echo "$TEST_NAME" | sed 's/test_[0-9]\+_//')

echo -e "${CYAN}🔧 AÑADIENDO NUEVO TEST: ${TEST_NAME}${NC}"
echo -e "${BLUE}   Test:      ${TEST_NAME}${NC}"
echo -e "${BLUE}   Feature:   ${TEST_FEATURE}${NC}" 
echo -e "${BLUE}   Compiler:  ${COMPILER}${NC}"
echo -e "${BLUE}   Mode:      ${MODE}${NC}"
echo -e "${BLUE}   Benchmark: ${BENCHMARK}${NC}"
echo ""

# =============================================================================
# PASO 1: CREAR ARCHIVO DE TEST
# =============================================================================

if [ -f "$TEST_FILE" ]; then
    echo -e "${YELLOW}⚠️  El archivo ${TEST_FILE} ya existe. ¿Sobrescribir? [y/N]${NC}"
    read -r response
    if [[ ! "$response" =~ ^[Yy]$ ]]; then
        echo -e "${RED}❌ Operación cancelada${NC}"
        exit 1
    fi
fi

echo -e "${GREEN}📝 Creando archivo de test: ${TEST_FILE}${NC}"

# Crear directorio tests si no existe
mkdir -p tests

# Generar contenido del test
cat > "$TEST_FILE" << EOF
#include <catch2/catch_test_macros.hpp>
#include "core/dig_t.hpp"

// =============================================================================
// TEST SUITE: ${TEST_NAME}
// =============================================================================
// Feature: ${TEST_FEATURE}
// Created: $(date '+%d/%m/%Y')
// Purpose: Test implementation for ${TEST_FEATURE} functionality
// =============================================================================

// Suite identification header
struct SuiteInfoPrinter {
    SuiteInfoPrinter() {
        std::cout << "\\n=============================================================\\n"
                  << " EJECUTANDO SUITE: ${TEST_NAME}\\n" 
                  << " FEATURE: ${TEST_FEATURE}\\n"
                  << "=============================================================" << std::endl;
    }
};
static SuiteInfoPrinter _info_printer;

// =============================================================================
// TESTS PRINCIPALES
// =============================================================================

TEST_CASE("${TEST_FEATURE}: Basic functionality", "[core][${TEST_FEATURE}]") {
    SECTION("Basic test case") {
        // TODO: Implement your test logic here
        
        // Ejemplo básico con dig_t
        dig_t<10> d(5);
        REQUIRE(d.get() == 5);
        
        // TODO: Añadir tests específicos para ${TEST_FEATURE}
        INFO("Test ${TEST_NAME} needs implementation");
    }
    
    SECTION("Edge cases") {
        // TODO: Implement edge case tests
        INFO("Edge case tests for ${TEST_FEATURE} need implementation");
    }
}

TEST_CASE("${TEST_FEATURE}: Advanced functionality", "[core][${TEST_FEATURE}][advanced]") {
    SECTION("Complex operations") {
        // TODO: Implement complex operation tests
        INFO("Advanced tests for ${TEST_FEATURE} need implementation");
    }
}

// =============================================================================
// BENCHMARKS (OPCIONAL)
// =============================================================================

#ifdef CATCH_CONFIG_ENABLE_BENCHMARKING
TEST_CASE("${TEST_FEATURE}: Performance benchmarks", "[benchmark][${TEST_FEATURE}]") {
    SECTION("Performance test") {
        BENCHMARK("${TEST_FEATURE} basic operation") {
            // TODO: Implement benchmark
            dig_t<10> d(5);
            return d.get();
        };
    }
}
#endif

EOF

echo -e "${GREEN}✅ Archivo de test creado: ${TEST_FILE}${NC}"

# =============================================================================
# PASO 2: AÑADIR AL MESON.BUILD
# =============================================================================

echo -e "${GREEN}📝 Añadiendo test a ${MESON_FILE}${NC}"

# Buscar el patrón donde insertar el nuevo test
if ! grep -q "test_targets = \[" "$MESON_FILE"; then
    echo -e "${RED}❌ ERROR: No se encontró el array test_targets en ${MESON_FILE}${NC}"
    exit 1
fi

# Crear entrada para meson.build
MESON_ENTRY="  {
    'name': '${TEST_NAME}',
    'source': '${TEST_NAME}.cpp',
    'priority': 'high'
  },"

# Buscar la línea antes del cierre del array y añadir el test
if grep -q "# EJEMPLO: Test nuevo" "$MESON_FILE"; then
    # Insertar antes del ejemplo comentado
    sed -i "/# EJEMPLO: Test nuevo/i\\
$MESON_ENTRY" "$MESON_FILE"
else
    # Insertar antes del cierre del array
    sed -i "/^]/i\\
$MESON_ENTRY" "$MESON_FILE"
fi

echo -e "${GREEN}✅ Test añadido a ${MESON_FILE}${NC}"

# =============================================================================
# PASO 3: COMPILAR Y EJECUTAR
# =============================================================================

echo -e "${CYAN}🔨 COMPILANDO Y EJECUTANDO TEST${NC}"

# Compilar el test
echo -e "${GREEN}📦 Compilando: ./build_tests.bash ${TEST_NAME} ${COMPILER} ${MODE}${NC}"
if [ "$PRINT_FLAG" = "print" ]; then
    ./build_tests.bash "$TEST_NAME" "$COMPILER" "$MODE" print
else
    ./build_tests.bash "$TEST_NAME" "$COMPILER" "$MODE"
fi

BUILD_EXIT_CODE=$?
if [ $BUILD_EXIT_CODE -ne 0 ]; then
    echo -e "${RED}❌ ERROR: Falló la compilación del test${NC}"
    exit 1
fi

echo ""
echo -e "${GREEN}🧪 Ejecutando: ./check_tests.bash ${TEST_NAME} ${COMPILER} ${BENCHMARK}${NC}"
if [ "$PRINT_FLAG" = "print" ]; then
    ./check_tests.bash "$TEST_NAME" "$COMPILER" "$BENCHMARK" print
else
    ./check_tests.bash "$TEST_NAME" "$COMPILER" "$BENCHMARK"
fi

TEST_EXIT_CODE=$?

echo ""
echo -e "${CYAN}==============================================================${NC}"

if [ $TEST_EXIT_CODE -eq 0 ]; then
    echo -e "${GREEN}✅ TEST AÑADIDO EXITOSAMENTE: ${TEST_NAME}${NC}"
    echo -e "${GREEN}📁 Archivo creado: ${TEST_FILE}${NC}"
    echo -e "${GREEN}📝 Añadido a: ${MESON_FILE}${NC}"
    echo -e "${GREEN}✅ Compilación: EXITOSA${NC}"
    echo -e "${GREEN}✅ Ejecución: EXITOSA${NC}"
else
    echo -e "${YELLOW}⚠️  TEST AÑADIDO CON WARNINGS: ${TEST_NAME}${NC}"
    echo -e "${GREEN}📁 Archivo creado: ${TEST_FILE}${NC}"
    echo -e "${GREEN}📝 Añadido a: ${MESON_FILE}${NC}"
    echo -e "${GREEN}✅ Compilación: EXITOSA${NC}"
    echo -e "${YELLOW}⚠️  Ejecución: CON WARNINGS (código: ${TEST_EXIT_CODE})${NC}"
fi

echo ""
echo -e "${CYAN}Próximos pasos:${NC}"
echo -e "${BLUE}1. Editar ${TEST_FILE} para implementar la lógica específica${NC}"
echo -e "${BLUE}2. Añadir includes específicos si son necesarios${NC}"
echo -e "${BLUE}3. Ejecutar: ./check_tests.bash ${TEST_NAME} ${COMPILER} ${BENCHMARK}${NC}"
echo -e "${CYAN}==============================================================${NC}"

exit 0