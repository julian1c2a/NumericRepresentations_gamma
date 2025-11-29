#!/bin/bash

# Leemos el preset del primer argumento, o usamos 'gcc-release' por defecto
PRESET=${1:-gcc-release}

# Deducimos la ruta basándonos en el nombre del preset (formato: compilador-modo)
# Ejemplo: gcc-release -> COMPILER=gcc, MODE=release
COMPILER=$(echo "$PRESET" | cut -d'-' -f1)
MODE=$(echo "$PRESET" | cut -d'-' -f2)

# Construimos la ruta base según tu estructura de CMakePresets.json
BASE_PATH="./build/build_targets/$COMPILER/$MODE"

echo "=========================================="
echo " EJECUTANDO TESTS DIRECTOS DESDE: $BASE_PATH"
echo "=========================================="

if [ ! -d "$BASE_PATH" ]; then
    echo "Error: El directorio '$BASE_PATH' no existe."
    echo "Asegúrate de haber construido el proyecto con el preset '$PRESET' primero."
    exit 1
fi

# Función helper para ejecutar
run_test() {
    TEST_EXE="$BASE_PATH/$1.exe"
    if [ -f "$TEST_EXE" ]; then
        echo ">>> Ejecutando $1..."
        "$TEST_EXE"
    else
        echo "Advertencia: No se encontró $TEST_EXE"
    fi
}

run_test test_01_math_tables
run_test test_02_append
run_test test_03_core_internal
run_test test_04_dig_t
run_test test_05_dig_t_constructors
run_test test_06_dig_t_assignations
run_test test_07_dig_t_conversions
run_test test_08_dig_t_operadores
run_test test_09_dig_t_algebra
run_test test_10_dig_t_io
run_test test_11_basic_types
run_test test_12_Int_ExpLog
run_test test_13_IntRoot
run_test test_14_primes