#!/bin/bash

# Script adaptado para Meson - ejecución directa de tests
# Uso: check_direct_tests.bash [compiler] [mode] [print]
# Ejemplos:
#   check_direct_tests.bash
#   check_direct_tests.bash gcc release
#   check_direct_tests.bash msvc release print
#   check_direct_tests.bash clang debug
#   check_direct_tests.bash release         # Para compatibilidad (asume gcc)

# Parsear argumentos manteniendo compatibilidad con formato anterior
if [[ $# -eq 0 ]]; then
    # Sin argumentos: usa defaults
    COMPILER="gcc"
    MODE="release"
elif [[ $# -eq 1 ]]; then
    # Un argumento: puede ser modo o compiler-mode o compiler
    if [[ "$1" == "debug" || "$1" == "release" ]]; then
        # Es un modo solo
        COMPILER="gcc"
        MODE="$1"
    elif [[ "$1" =~ ^(gcc|clang|msvc|all)$ ]]; then
        # Es un compilador solo
        COMPILER="$1"
        MODE="release"
    else
        # Formato legacy: gcc-release
        COMPILER=$(echo "$1" | cut -d'-' -f1)
        MODE=$(echo "$1" | cut -d'-' -f2)
    fi
elif [[ $# -eq 2 ]]; then
    if [[ "$2" == "print" ]]; then
        # formato: modo print
        COMPILER="gcc"
        MODE="$1"
    else
        # formato: compiler modo
        COMPILER="$1"
        MODE="$2"
    fi
else
    # 3 argumentos: compiler modo print
    COMPILER="$1"
    MODE="$2"
fi

# Si el último argumento es 'print', redirigir salida
if [[ "${!#}" == "print" ]]; then
    LOG_FILE="check_direct_log_${COMPILER}_${MODE}.txt"
    exec > "$LOG_FILE" 2>&1
fi

run_direct_tests_with_compiler() {
    local comp="$1"
    local mode="$2"
    
    echo ">>> Ejecutando tests directos con $comp $mode <<<"
    
    case "$comp" in
        "msvc")
            echo "Los tests de MSVC están compilados por separado con batch scripts"
            echo "Para MSVC, usar los scripts .bat específicos:"
            echo "  test_02_append_msvc.bat"
            echo "  test_LUT_of_primes_msvc.bat"
            echo "  etc."
            return 0
            ;;
        "clang")
            # Tests de Clang están en build CMake
            BASE_PATH="./build-clang/build-$mode"
            if [ ! -d "$BASE_PATH" ]; then
                echo "Error: Directorio Clang '$BASE_PATH' no existe."
                echo "Compila primero con: cmake --preset clang-$mode && cmake --build --preset clang-$mode"
                return 1
            fi
            ;;
        "gcc")
            # Tests de GCC están en Meson
            case "$mode" in
                "debug")
                    BASE_PATH="./builddir-debug/tests"
                    ;;
                "release")
                    BASE_PATH="./builddir/tests"
                    ;;
                *)
                    echo "Error: Modo no válido '$mode'. Usar 'debug' o 'release'"
                    return 1
                    ;;
            esac
            
            if [ ! -d "$BASE_PATH" ]; then
                echo "Error: El directorio '$BASE_PATH' no existe."
                echo "Asegúrate de haber construido el proyecto primero con:"
                echo "  bash ./build_tests.bash all $mode"
                return 1
            fi
            ;;
    esac
    
    # Ejecutar todos los tests para este compilador
    echo "Ejecutando desde: $BASE_PATH"
    
    # Lista de tests en orden - usar función ya existente
    run_all_direct_tests
    
    return 0
}

# Manejar compilador='all'
if [ "$COMPILER" == "all" ]; then
    echo "Ejecutando tests directos con TODOS los compiladores ($MODE):"
    echo "============================================="
    
    failed_compilers=()
    success_count=0
    
    for comp in "gcc" "clang" "msvc"; do
        if run_direct_tests_with_compiler "$comp" "$MODE"; then
            ((success_count++))
            echo "✅ $comp: OK"
        else
            failed_compilers+=("$comp")
            echo "❌ $comp: FAILED"
        fi
        echo ""
    done
    
    echo "============================================="
    echo "Compiladores exitosos: $success_count/3"
    
    if [ ${#failed_compilers[@]} -gt 0 ]; then
        echo "Compiladores con errores:"
        for comp in "${failed_compilers[@]}"; do
            echo "  - $comp"
        done
        exit 1
    else
        echo "🎉 Todos los compiladores completaron exitosamente"
    fi
    exit 0
fi

# Para compilador específicotest() {
    TEST_EXE="$BASE_PATH/$1.exe"
    if [ -f "$TEST_EXE" ]; then
        echo ">>> Ejecutando $1..."
        "$TEST_EXE"
        echo ""
    else
        echo "⚠️  No encontrado: $TEST_EXE"
        echo ""
    fi
}

# Función helper para ejecutar todos los tests directos
run_all_direct_tests() {
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
    run_test test_15_conversions
    run_test test_LUT_of_primes
    run_test test_primes_compiletime_catch2
    run_test basic_compilation_test
}

# Lista de tests en orden
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
run_test test_15_conversions
run_test test_LUT_of_primes
run_test test_primes_compiletime_catch2
run_test basic_compilation_test

echo "=========================================="
echo " EJECUCIÓN DIRECTA COMPLETADA: $COMPILER $MODE"
echo "=========================================="