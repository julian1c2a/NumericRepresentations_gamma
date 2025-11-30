#!/bin/bash

# Uso: ./install_deps.bash [msvc|gcc|clang]
COMPILER_MODE=${1:-msvc}
CATCH2_VERSION="v3.11.0"
BASE_INSTALL_DIR="$(pwd)/libs_install"
BUILD_ROOT="build_deps"

echo "=========================================="
echo " PREPARANDO DEPENDENCIAS (CATCH2)"
echo " Compilador objetivo: $COMPILER_MODE"
echo "=========================================="

# 1. Descargar Catch2 si hace falta
mkdir -p "$BUILD_ROOT"
if [ ! -d "$BUILD_ROOT/Catch2" ]; then
    echo ">>> Clonando Catch2 $CATCH2_VERSION..."
    git clone --branch $CATCH2_VERSION --depth 1 https://github.com/catchorg/Catch2.git "$BUILD_ROOT/Catch2"
fi

# Definimos la raíz para este compilador
COMPILER_INSTALL_ROOT="$BASE_INSTALL_DIR/$COMPILER_MODE"
BUILD_DIR_BASE="$BUILD_ROOT/build_catch_$COMPILER_MODE"

# Función auxiliar para compilar
build_cmake() {
    local EXTRA_ARGS="$1" 
    
    # Caso MSVC: Multi-configuración (Todo en una carpeta)
    if [ "$COMPILER_MODE" == "msvc" ]; then
        INSTALL_PATH="$COMPILER_INSTALL_ROOT/Catch2"
        BUILD_DIR="$BUILD_DIR_BASE"
        
        echo ">>> Configurando (MSVC Multi-Config) en $INSTALL_PATH..."
        cmake -S "$BUILD_ROOT/Catch2" -B "$BUILD_DIR" \
            -DCMAKE_INSTALL_PREFIX="$INSTALL_PATH" \
            -DBUILD_TESTING=OFF \
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
            $EXTRA_ARGS

        echo ">>> Compilando e Instalando (Release y Debug)..."
        cmake --build "$BUILD_DIR" --config Release --target install
        cmake --build "$BUILD_DIR" --config Debug --target install

    # Caso GCC/Clang: Single-config (Carpetas separadas para evitar conflictos)
    else
        # 1. Release
        INSTALL_PATH_REL="$COMPILER_INSTALL_ROOT/release/Catch2"
        BUILD_DIR_REL="$BUILD_DIR_BASE/release"
        
        echo ">>> Configurando (Release) en $INSTALL_PATH_REL..."
        cmake -S "$BUILD_ROOT/Catch2" -B "$BUILD_DIR_REL" \
            -DCMAKE_INSTALL_PREFIX="$INSTALL_PATH_REL" \
            -DCMAKE_BUILD_TYPE=Release \
            -DBUILD_TESTING=OFF \
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
            $EXTRA_ARGS
        cmake --build "$BUILD_DIR_REL" --target install

        # 2. Debug
        INSTALL_PATH_DBG="$COMPILER_INSTALL_ROOT/debug/Catch2"
        BUILD_DIR_DBG="$BUILD_DIR_BASE/debug"

        echo ">>> Configurando (Debug) en $INSTALL_PATH_DBG..."
        cmake -S "$BUILD_ROOT/Catch2" -B "$BUILD_DIR_DBG" \
            -DCMAKE_INSTALL_PREFIX="$INSTALL_PATH_DBG" \
            -DCMAKE_BUILD_TYPE=Debug \
            -DBUILD_TESTING=OFF \
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
            $EXTRA_ARGS
        cmake --build "$BUILD_DIR_DBG" --target install
    fi
}

# Selección de parámetros
case "$COMPILER_MODE" in
    msvc)
        build_cmake ""
        ;;
    gcc)
        ARGS="-DCMAKE_CXX_COMPILER=g++"
        if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "win32" ]]; then
            ARGS="$ARGS -G \"MinGW Makefiles\""
        fi
        build_cmake "$ARGS"
        ;;
    clang)
        ARGS="-DCMAKE_CXX_COMPILER=clang++"
        if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "win32" ]]; then
            ARGS="$ARGS -G \"MinGW Makefiles\""
        fi
        build_cmake "$ARGS"
        ;;
    *)
        echo "Error: Compilador '$COMPILER_MODE' no reconocido."
        exit 1
        ;;
esac

echo "=========================================="
echo " INSTALACIÓN COMPLETADA para $COMPILER_MODE"
echo "=========================================="