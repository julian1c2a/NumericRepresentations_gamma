#!/bin/bash

# Leemos el preset del primer argumento, o usamos 'gcc-release' por defecto
PRESET=${1:-gcc-release}

# Si el último argumento es 'print', redirigimos toda la salida al log correspondiente
if [ "$2" == "print" ]; then
    LOG_FILE="build_log_${PRESET//-/_}.txt"
    exec > "$LOG_FILE" 2>&1
fi

echo "=========================================="
echo " CONFIGURANDO Y CONSTRUYENDO: $PRESET"
echo "=========================================="

# Configuración (genera el build system)
cmake --preset "$PRESET" || { echo "Error en configuración"; exit 1; }

# Construcción de todos los targets de test
cmake --build --preset "$PRESET" --target test_01_math_tables || exit 1
cmake --build --preset "$PRESET" --target test_02_append || exit 1
cmake --build --preset "$PRESET" --target test_03_core_internal || exit 1
cmake --build --preset "$PRESET" --target test_04_dig_t || exit 1
cmake --build --preset "$PRESET" --target test_05_dig_t_constructors || exit 1
cmake --build --preset "$PRESET" --target test_06_dig_t_assignations || exit 1
cmake --build --preset "$PRESET" --target test_07_dig_t_conversions || exit 1
cmake --build --preset "$PRESET" --target test_08_dig_t_operadores || exit 1
cmake --build --preset "$PRESET" --target test_09_dig_t_algebra || exit 1
cmake --build --preset "$PRESET" --target test_10_dig_t_io || exit 1
cmake --build --preset "$PRESET" --target test_11_basic_types || exit 1
cmake --build --preset "$PRESET" --target test_12_Int_ExpLog || exit 1
cmake --build --preset "$PRESET" --target test_13_IntRoot || exit 1
cmake --build --preset "$PRESET" --target test_14_primes || exit 1
cmake --build --preset "$PRESET" --target test_15_conversions || exit 1

echo "--- Construcción completada para $PRESET ---"