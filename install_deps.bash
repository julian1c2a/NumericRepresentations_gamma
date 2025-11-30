#!/bin/bash

# ==============================================================================
# SCRIPT CORREGIDO DE INSTALACIÓN DE DEPENDENCIAS (CATCH2)
# ==============================================================================

INPUT_ARG=${1:-msvc}

# Detección de modo
if [[ "$INPUT_ARG" == *"msvc"* ]]; then
    COMPILER_MODE="msvc"
elif [[ "$INPUT_ARG" == *"gcc"* ]]; then
    COMPILER_MODE="gcc"
elif [[ "$INPUT_ARG" == *"clang"* ]]; then
    COMPILER_MODE="clang"
else
    echo "Advertencia: Asumiendo 'msvc' para '$INPUT_ARG'."
    COMPILER_MODE="msvc"
fi

CATCH2_VERSION="v3.4.0"
BASE_INSTALL_DIR="$(pwd)/libs_install"
BUILD_ROOT="build_deps"

# Descarga
mkdir -p "$BUILD_ROOT"
if [ ! -d "$BUILD_ROOT/Catch2" ]; then
    echo ">>> Clonando Catch2 $CATCH2_VERSION..."
    git clone --branch $CATCH2_VERSION --depth 1 https://github.com/catchorg/Catch2.git "$BUILD_ROOT/Catch2"
fi

COMPILER_INSTALL_ROOT="$BASE_INSTALL_DIR/$COMPILER_MODE"
BUILD_DIR_BASE="$BUILD_ROOT/build_catch_$COMPILER_MODE"

build_cmake() {
    # CASO A: MSVC (Forzamos Visual Studio 2022 para generar .lib válidos)
    if [ "$COMPILER_MODE" == "msvc" ]; then
        INSTALL_PATH="$COMPILER_INSTALL_ROOT/Catch2"
        BUILD_DIR="$BUILD_DIR_BASE"
        
        # LIMPIEZA CRÍTICA: Borramos build previo para evitar caché de MinGW
        rm -rf "$BUILD_DIR"
        
        echo ">>> [MSVC] Configurando con Visual Studio 17 2022..."
        
        # Forzamos generador Visual Studio. 
        # NOTA: Esto requiere tener VS2022 instalado (que ya vi en tus logs que tienes).
        cmake -S "$BUILD_ROOT/Catch2" -B "$BUILD_DIR" \
            -G "Visual Studio 17 2022" -A x64 \
            -DCMAKE_INSTALL_PREFIX="$INSTALL_PATH" \
            -DBUILD_TESTING=OFF \
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
            "$@"

        echo ">>> [MSVC] Compilando (Release)..."
        cmake --build "$BUILD_DIR" --config Release --target install
        echo ">>> [MSVC] Compilando (Debug)..."
        cmake --build "$BUILD_DIR" --config Debug --target install

    # CASO B: GCC / CLANG
    else
        INSTALL_PATH_REL="$COMPILER_INSTALL_ROOT/release/Catch2"
        BUILD_DIR_REL="$BUILD_DIR_BASE/release"
        INSTALL_PATH_DBG="$COMPILER_INSTALL_ROOT/debug/Catch2"
        BUILD_DIR_DBG="$BUILD_DIR_BASE/debug"
        
        # Limpieza por seguridad
        rm -rf "$BUILD_DIR_REL" "$BUILD_DIR_DBG"

        echo ">>> [$COMPILER_MODE] Configurando y Compilando..."
        
        # Loop para Release y Debug
        for BTYPE in Release Debug; do
            if [ "$BTYPE" == "Release" ]; then
                TPATH="$INSTALL_PATH_REL"; TBUILD="$BUILD_DIR_REL"
            else
                TPATH="$INSTALL_PATH_DBG"; TBUILD="$BUILD_DIR_DBG"
            fi
            
            cmake -S "$BUILD_ROOT/Catch2" -B "$TBUILD" \
                -DCMAKE_INSTALL_PREFIX="$TPATH" \
                -DCMAKE_BUILD_TYPE=$BTYPE \
                -DBUILD_TESTING=OFF \
                -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
                "$@"
            
            cmake --build "$TBUILD" --target install
        done
    fi
}

case "$COMPILER_MODE" in
    msvc)
        build_cmake # Ya tiene los argumentos dentro
        ;;
    gcc)
        ARGS=("-DCMAKE_CXX_COMPILER=g++")
        if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "win32" ]]; then
            ARGS+=("-G" "MinGW Makefiles")
        fi
        build_cmake "${ARGS[@]}"
        ;;
    clang)
        # IMPORTANTE: Forzamos libc++ para coincidir con el entorno clang64
        ARGS=("-DCMAKE_CXX_COMPILER=clang++" "-DCMAKE_CXX_FLAGS=-stdlib=libc++")
        if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "win32" ]]; then
            ARGS+=("-G" "MinGW Makefiles")
        fi
        build_cmake "${ARGS[@]}"
        ;;
esac

echo "=========================================="
echo " INSTALACIÓN COMPLETADA para $COMPILER_MODE"
echo "=========================================="