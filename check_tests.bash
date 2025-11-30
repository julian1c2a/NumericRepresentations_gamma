#!/bin/bash

# Leemos el preset del primer argumento, o usamos 'gcc-release' por defecto
PRESET=${1:-gcc-release}

# Si el último argumento es 'print', redirigimos toda la salida al log correspondiente
if [ "$2" == "print" ]; then
    LOG_FILE="check_log_${PRESET//-/_}.txt"
    exec > "$LOG_FILE" 2>&1
fi

echo "=========================================="
echo " EJECUTANDO CTEST CON PRESET: $PRESET"
echo "=========================================="

# Ejecutamos ctest filtrando por nombre (-R) para asegurar orden o selección
# Nota: Podrías ejecutar simplemente 'ctest --preset "$PRESET"' para correrlos todos,
# pero mantenemos tu lista explícita.

ctest --preset "$PRESET" -R test_01_math_tables
ctest --preset "$PRESET" -R test_02_append
ctest --preset "$PRESET" -R test_03_core_internal
ctest --preset "$PRESET" -R test_04_dig_t
ctest --preset "$PRESET" -R test_05_dig_t_constructors
ctest --preset "$PRESET" -R test_06_dig_t_assignations
ctest --preset "$PRESET" -R test_07_dig_t_conversions
ctest --preset "$PRESET" -R test_08_dig_t_operadores
ctest --preset "$PRESET" -R test_09_dig_t_algebra
ctest --preset "$PRESET" -R test_10_dig_t_io
ctest --preset "$PRESET" -R test_11_basic_types
ctest --preset "$PRESET" -R test_12_Int_ExpLog
ctest --preset "$PRESET" -R test_13_IntRoot
ctest --preset "$PRESET" -R test_14_primes
ctest --preset "$PRESET" -R test_15_conversions