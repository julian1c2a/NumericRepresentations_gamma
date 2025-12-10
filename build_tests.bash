#!/bin/bash

#!/bin/bash

# Script adaptado para Meson - mantiene interfaz original de CMake
# Uso: build_tests.bash <test_name> <compiler> <mode> [print]
#   O: build_tests.bash <compiler> <mode> [print] (para todos los tests)
# Ejemplos:
#   build_tests.bash test_04_dig_t gcc release
#   build_tests.bash test_02_append gcc debug print
#   build_tests.bash all gcc release print
#   build_tests.bash gcc release print (compila todos)

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
    LOG_FILE="build_log_${TEST_NAME}_${COMPILER}_${MODE}.txt"
    exec > "$LOG_FILE" 2>&1
fi

echo "=========================================="
echo " CONSTRUYENDO: $TEST_NAME con $COMPILER $MODE $([ "$BENCHMARK" == "bench" ] && echo "(benchmarks)" || echo "")"
echo "========================================="

# Función para construir con un compilador específico
build_with_compiler() {
    local comp="$1"
    local mode="$2"
    local test_name="$3"
    
    echo ">>> Construyendo con $comp $mode <<<"
    
    case "$comp" in
        "msvc")
            echo "Para MSVC, usar los scripts .bat específicos:"
            echo "  test_02_append_msvc.bat"
            echo "  test_LUT_of_primes_msvc.bat"
            echo "  O usar los archivos .bat individuales por test"
            ;;
        "clang")
            echo "Para Clang, usar CMake:"
            echo "  cmake --preset clang-$mode"
            echo "  cmake --build --preset clang-$mode"
            ;;
        "gcc")
            # Solo GCC usa Meson en este script
            case "$mode" in
                "debug")
                    local builddir="builddir-debug"
                    local buildtype="debug"
                    ;;
                "release")
                    local builddir="builddir"
                    local buildtype="release"
                    ;;
                *)
                    echo "Error: Modo no válido '$mode'. Usar 'debug' o 'release'"
                    return 1
                    ;;
            esac
            
            # Configurar Meson si no existe el builddir
            if [ ! -d "$builddir" ]; then
                echo "Configurando Meson builddir: $builddir"
                meson setup "$builddir" --buildtype="$buildtype" || { echo "Error en configuración"; return 1; }
            fi
            
            # Compilar según el test solicitado
            case "$test_name" in
                "all"|"all_tests")
                    compile_all_tests "$builddir"
                    ;;
                *)
                    compile_test "$test_name" "$builddir"
                    ;;
            esac
            ;;
    esac
}

# Manejar compilador='all'
if [ "$COMPILER" == "all" ]; then
    echo "Construyendo con TODOS los compiladores ($MODE):"
    echo "========================================="
    
    local failed_compilers=()
    local success_count=0
    
    for comp in "gcc" "clang" "msvc"; do
        if build_with_compiler "$comp" "$MODE" "$TEST_NAME"; then
            ((success_count++))
            echo "✅ $comp: OK"
        else
            failed_compilers+=("$comp")
            echo "❌ $comp: FAILED"
        fi
        echo ""
    done
    
    echo "========================================="
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

# Configurar para compilador específico
case "$COMPILER" in
    "msvc")
        echo "Para MSVC, usar los scripts .bat específicos:"
        echo "  test_02_append_msvc.bat"
        echo "  test_LUT_of_primes_msvc.bat"
        echo "  O usar los archivos .bat individuales por test"
        exit 0
        ;;
    "clang")
        echo "Para Clang, usar CMake:"
        echo "  cmake --preset clang-$MODE"
        echo "  cmake --build --preset clang-$MODE"
        exit 0
        ;;
    "gcc"|*)
        # Solo GCC usa Meson en este script
        case "$MODE" in
            "debug")
                BUILDDIR="builddir-debug"
                BUILDTYPE="debug"
                ;;
            "release")
                BUILDDIR="builddir"
                BUILDTYPE="release"
                ;;
            *)
                echo "Error: Modo no válido '$MODE'. Usar 'debug' o 'release'"
                exit 1
                ;;
        esac
        ;;
esac

# Configurar Meson si no existe el builddir
if [ ! -d "$BUILDDIR" ]; then
    echo "Configurando Meson builddir: $BUILDDIR"
    meson setup "$BUILDDIR" --buildtype="$BUILDTYPE" || { echo "Error en configuración"; exit 1; }
fi

# Función para compilar un test específico
compile_test() {
    local test_name="$1"
    echo "--- Compilando: $test_name ---"
    meson compile -C "$BUILDDIR" "$test_name" || return 1
}

# Función para compilar todos los tests
compile_all_tests() {
    echo "--- Compilando todos los tests ---"
    
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
    
    for test in "${tests[@]}"; do
        if compile_test "$test"; then
            ((success_count++))
            echo "✅ $test: OK"
        else
            failed_tests+=("$test")
            echo "❌ $test: FAILED"
        fi
    done
    
    echo ""
    echo "=========================================="
    echo " RESUMEN DE COMPILACIÓN"
    echo "=========================================="
    echo "Tests exitosos: $success_count/${#tests[@]}"
    
    if [ ${#failed_tests[@]} -gt 0 ]; then
        echo "Tests fallidos:"
        for test in "${failed_tests[@]}"; do
            echo "  - $test"
        done
        return 1
    else
        echo "🎉 Todos los tests compilaron exitosamente"
        return 0
    fi
}

# Ejecutar según el test solicitado
case "$TEST_NAME" in
    "all"|"all_tests")
        compile_all_tests
        result=$?
        ;;
    *)
        compile_test "$TEST_NAME"
        result=$?
        ;;
esac

if [ $result -eq 0 ]; then
    echo "--- Construcción completada para $TEST_NAME ($COMPILER $MODE) ---"
else
    echo "--- Error en construcción de $TEST_NAME ($COMPILER $MODE) ---"
    exit 1
fi