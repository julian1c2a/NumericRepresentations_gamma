#!/bin/bash

# Script adaptado para Meson
# Uso: check_tests.bash <test_name> <compiler> <mode> [print]
#   O: check_tests.bash <compiler> <mode> [print] (para todos los tests)
# Ejemplos:
#   check_tests.bash test_04_dig_t gcc release
#   check_tests.bash test_02_append gcc debug print
#   check_tests.bash all gcc release print
#   check_tests.bash gcc release print (ejecuta todos)

# Parsear argumentos de manera inteligente
if [[ $# -ge 3 ]] && [[ "$1" =~ ^test_|^all$ || "$1" == "all_tests" ]]; then
    # Formato: test_name compiler mode [benchmark] [print]
    TEST_NAME="$1"
    COMPILER="$2"
    MODE="$3"
    # Verificar si 4to argumento es benchmark o print
    if [ "$4" == "bench" ] || [ "$4" == "nobench" ]; then
        BENCHMARK="$4"
        PRINT_ARG="$5"
    else
        BENCHMARK="nobench"
        PRINT_ARG="$4"
    fi
elif [[ $# -ge 2 ]] && [[ "$1" =~ ^(gcc|clang|msvc|all)$ ]]; then
    # Formato: compiler mode [benchmark] [print] (asume all tests)
    TEST_NAME="all"
    COMPILER="$1"
    MODE="$2"
    # Verificar si 3er argumento es benchmark o print
    if [ "$3" == "bench" ] || [ "$3" == "nobench" ]; then
        BENCHMARK="$3"
        PRINT_ARG="$4"
    else
        BENCHMARK="nobench"
        PRINT_ARG="$3"
    fi
elif [[ $# -ge 2 ]]; then
    # Formato legacy: test_name mode [benchmark] [print] (asume gcc)
    TEST_NAME="$1"
    COMPILER="gcc"
    MODE="$2"
    # Verificar si 3er argumento es benchmark o print
    if [ "$3" == "bench" ] || [ "$3" == "nobench" ]; then
        BENCHMARK="$3"
        PRINT_ARG="$4"
    else
        BENCHMARK="nobench"
        PRINT_ARG="$3"
    fi
else
    # Defaults
    TEST_NAME="${1:-all}"
    COMPILER="gcc"
    MODE="release"
    BENCHMARK="nobench"
    PRINT_ARG="$2"
fi

# Si el argumento print está presente, redirigir salida
if [ "$PRINT_ARG" == "print" ]; then
    LOG_FILE="check_log_${TEST_NAME}_${COMPILER}_${MODE}.txt"
    exec > "$LOG_FILE" 2>&1
fi

# Función para ejecutar un test específico
run_test() {
    local test_name="$1"
    local benchmark_mode="${2:-nobench}"
    
    # Determinar la ruta del ejecutable según el compilador
    local exe_path
    if [[ "$BUILDDIR" == *"clang"* ]]; then
        # Para Clang (CMake), los ejecutables están directamente en el builddir
        exe_path="$BUILDDIR/$test_name.exe"
    else
        # Para GCC (Meson), los ejecutables están en tests/
        exe_path="$BUILDDIR/tests/$test_name.exe"
    fi
    
    if [ ! -f "$exe_path" ]; then
        echo "❌ $test_name: Ejecutable no encontrado en $exe_path"
        return 1
    fi
    
    echo "--- Ejecutando: $test_name $([ "$benchmark_mode" == "bench" ] && echo "(benchmarks)" || echo "") ---"
    
    # Configurar argumentos para benchmarks
    local test_args=""
    if [ "$benchmark_mode" == "bench" ]; then
        # Solo ejecutar benchmarks
        test_args="[benchmark]"
    else
        # Excluir benchmarks (comportamiento por defecto)
        test_args="~[benchmark]"
    fi
    
    if "$exe_path" "$test_args"; then
        echo "✅ $test_name: PASÓ"
        return 0
    else
        echo "❌ $test_name: FALLÓ"
        return 1
    fi
}

# Función para ejecutar todos los tests
run_all_tests() {
    local benchmark_mode="${1:-nobench}"
    echo "--- Ejecutando todos los tests $([ "$benchmark_mode" == "bench" ] && echo "(benchmarks)" || echo "") ---"
    
    local tests=(
        "test_01_math_tables"
        "test_02_append" 
        "test_03_core_internal"
        "test_04_dig_t"
        "test_05_dig_t_constructors"
        "test_06_dig_t_assignations"
        "test_07_dig_t_conversions"
        "test_08_dig_t_operadores"
        "test_09_dig_t_algebra"
        "test_10_dig_t_io"
        "test_11_basic_types"
        "test_12_Int_ExpLog"
        "test_13_IntRoot"
        "test_14_primes"
        "test_15_conversions"
        "test_LUT_of_primes"
        "test_primes_compiletime_catch2"
        "basic_compilation_test"
    )
    
    local failed_tests=()
    local success_count=0
    local total_assertions=0
    
    for test in "${tests[@]}"; do
        if run_test "$test" "$BENCHMARK"; then
            ((success_count++))
            # Intento simple de extraer número de assertions
            # Solo si el ejecutable existe
            if [ -f "$BUILDDIR/tests/$test.exe" ]; then
                local test_output
                test_output=$("$BUILDDIR/tests/$test.exe" 2>&1 | grep -o '[0-9]* assertion' | head -1 | grep -o '[0-9]*' || echo "0")
                if [[ "$test_output" =~ ^[0-9]+$ ]]; then
                    ((total_assertions += test_output))
                fi
            fi
        else
            failed_tests+=("$test")
        fi
        echo ""
    done
    
    echo "==========================================="
    echo " RESUMEN DE EJECUCIÓN"
    echo "==========================================="
    echo "Tests exitosos: $success_count/${#tests[@]}"
    echo "Total assertions: $total_assertions"
    
    if [ ${#failed_tests[@]} -gt 0 ]; then
        echo "Tests fallidos:"
        for test in "${failed_tests[@]}"; do
            echo "  - $test"
        done
        return 1
    else
        echo "🎉 Todos los tests pasaron exitosamente"
        return 0
    fi
}

# Función para ejecutar tests con un compilador específico
run_tests_with_compiler() {
    local comp="$1"
    local mode="$2"
    local test_name="$3"
    
    echo ">>> Ejecutando tests con $comp $mode <<<"
    
    case "$comp" in
        "msvc")
            echo "Para MSVC, usar los scripts .bat específicos:"
            echo "  test_02_append_msvc.bat"
            echo "  test_LUT_of_primes_msvc.bat"
            echo "  etc."
            return 0
            ;;
        "clang")
            # Tests de Clang están en build CMake
            case "$mode" in
                "debug")
                    local builddir="./build/build_targets/clang/debug"
                    ;;
                "release")
                    local builddir="./build/build_targets/clang/release"
                    ;;
                *)
                    echo "Error: Modo no válido '$mode'. Usar 'debug' o 'release'"
                    return 1
                    ;;
            esac
            
            if [ ! -d "$builddir" ]; then
                echo "Error: Directorio Clang '$builddir' no existe."
                echo "Compila primero con: cmake --preset clang-$mode && cmake --build --preset clang-$mode"
                return 1
            fi
            ;;
        "gcc")
            # Tests de GCC están en Meson
            case "$mode" in
                "debug")
                    local builddir="./build/build_targets/gcc/debug"
                    ;;
                "release")
                    local builddir="./build/build_targets/gcc/release"
                    ;;
                *)
                    echo "Error: Modo no válido '$mode'. Usar 'debug' o 'release'"
                    return 1
                    ;;
            esac
            
            if [ ! -d "$builddir" ]; then
                echo "Error: El directorio '$builddir' no existe."
                echo "Ejecuta primero: bash ./build_tests.bash $comp $mode"
                return 1
            fi
            
            # Usar las funciones existentes con el builddir correcto
            BUILDDIR="$builddir"
            
            case "$test_name" in
                "all"|"all_tests")
                    run_all_tests "$BENCHMARK"
                    ;;
                *)
                    run_test "$test_name" "$BENCHMARK"
                    ;;
            esac
            return $?
            ;;
    esac
}

# Función para ejecutar todos los compiladores
run_all_compilers() {
    local MODE="$1"
    local TEST_NAME="$2"
    
    local failed_compilers=()
    local success_count=0
    
    for comp in "gcc" "clang" "msvc"; do
        if run_tests_with_compiler "$comp" "$MODE" "$TEST_NAME"; then
            ((success_count++))
            echo "✅ $comp: OK"
        else
            failed_compilers+=("$comp")
            echo "❌ $comp: FAILED"
        fi
        echo ""
    done
    
    echo "=========================================="
    echo "Compiladores exitosos: $success_count/3"
    
    if [ ${#failed_compilers[@]} -gt 0 ]; then
        echo "Compiladores con errores:"
        for comp in "${failed_compilers[@]}"; do
            echo "  - $comp"
        done
        return 1
    else
        echo "🎉 Todos los compiladores completaron exitosamente"
        return 0
    fi
}

# Manejar compilador='all'
if [ "$COMPILER" == "all" ]; then
    echo "Ejecutando tests con TODOS los compiladores ($MODE):"
    echo "=========================================="
    
    run_all_compilers "$MODE" "$TEST_NAME"
    exit $?
fi

# Configurar para compilador específico
case "$COMPILER" in
    "msvc")
        echo "Para MSVC, usar los scripts .bat específicos:"
        echo "  test_02_append_msvc.bat"
        echo "  test_LUT_of_primes_msvc.bat"
        echo "  etc."
        exit 0
        ;;
    "clang")
        # Tests de Clang están en build CMake
        case "$MODE" in
            "debug")
                BUILDDIR="./build/build_targets/clang/debug"
                ;;
            "release")
                BUILDDIR="./build/build_targets/clang/release"
                ;;
            *)
                echo "Error: Modo no válido '$MODE'. Usar 'debug' o 'release'"
                exit 1
                ;;
        esac
        ;;
    "gcc"|*)
        # Tests de GCC están en Meson
        case "$MODE" in
            "debug")
                BUILDDIR="builddir-debug"
                ;;
            "release")
                BUILDDIR="builddir"
                ;;
            *)
                echo "Error: Modo no válido '$MODE'. Usar 'debug' o 'release'"
                exit 1
                ;;
        esac
        ;;
esac

# Verificar que el builddir existe
if [ ! -d "$BUILDDIR" ]; then
    echo "Error: Directorio de build '$BUILDDIR' no existe"
    echo "Ejecuta primero: build_tests.bash $TEST_NAME $MODE"
    exit 1
fi

# Ejecutar según el test solicitado
case "$TEST_NAME" in
    "all"|"all_tests")
        run_all_tests "$BENCHMARK"
        result=$?
        ;;
    *)
        run_test "$TEST_NAME" "$BENCHMARK"
        result=$?
        ;;
esac

if [ $result -eq 0 ]; then
    echo "--- Ejecución completada para $TEST_NAME ($COMPILER $MODE) ---"
else
    echo "--- Error en ejecución de $TEST_NAME ($COMPILER $MODE) ---"
    exit 1
fi