#!/bin/bash

# ==============================================================================
# SCRIPT DE INSTALACIÓN DE DEPENDENCIAS (CATCH2)
# ==============================================================================
# Uso: ./install_deps.bash [msvc|gcc|clang] [print]
# Ejemplo: ./install_deps.bash msvc print  -> Genera deps_log_msvc.txt
# ==============================================================================

# Leemos el modo/preset (por defecto 'msvc')
INPUT_ARG=${1:-msvc}
# Leemos el segundo argumento para saber si imprimimos a fichero
SECOND_ARG=${2:-""}

# --- LÓGICA DE LOGGING DINÁMICO ---
if [ "$SECOND_ARG" == "print" ]; then
    SAFE_SUFFIX="${INPUT_ARG//-/_}"
    LOG_FILE="deps_log_${SAFE_SUFFIX}.txt"
    exec > "$LOG_FILE" 2>&1
fi

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

# CAMBIO IMPORTANTE: Subimos a v3.7.1 para corregir error C7595 con spaceship operator en MSVC C++23
CATCH2_VERSION="v3.7.1"
BASE_INSTALL_DIR="$(pwd)/libs_install"
BUILD_ROOT="build_deps"
TOOLCHAIN_FILE="$(pwd)/msvc_toolchain.cmake" 

mkdir -p "$BUILD_ROOT"

# Lógica robusta de descarga/actualización
if [ ! -d "$BUILD_ROOT/Catch2" ]; then
    echo ">>> Clonando Catch2 $CATCH2_VERSION..."
    git clone --branch $CATCH2_VERSION --depth 1 https://github.com/catchorg/Catch2.git "$BUILD_ROOT/Catch2"
else
    echo ">>> Verificando versión de Catch2..."
    pushd "$BUILD_ROOT/Catch2" > /dev/null
    # Hacemos fetch y checkout forzado para asegurar que estamos en la versión correcta
    git fetch --tags
    git checkout $CATCH2_VERSION
    echo ">>> Actualizado a $CATCH2_VERSION"
    popd > /dev/null
fi

COMPILER_INSTALL_ROOT="$BASE_INSTALL_DIR/$COMPILER_MODE"
BUILD_DIR_BASE="$BUILD_ROOT/build_catch_$COMPILER_MODE"

build_cmake() {
    # NOTA: Ninja es un generador "Single-Configuration"
    
    # CASO A: MSVC CON NINJA + TOOLCHAIN
    if [ "$COMPILER_MODE" == "msvc" ]; then
        INSTALL_PATH="$COMPILER_INSTALL_ROOT/Catch2"
        
        # Limpieza inicial para asegurar re-compilación limpia con la nueva versión
        rm -rf "$BUILD_DIR_BASE"
        rm -rf "$INSTALL_PATH"

        echo ">>> [MSVC-Ninja] Configurando y Compilando Catch2 $CATCH2_VERSION..."

        for BTYPE in Release Debug; do
            CURRENT_BUILD_DIR="${BUILD_DIR_BASE}_${BTYPE}"
            
            echo "   --- Construyendo $BTYPE ---"
            
            cmake -S "$BUILD_ROOT/Catch2" -B "$CURRENT_BUILD_DIR" \
                -G "Ninja" \
                -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE" \
                -DCMAKE_INSTALL_PREFIX="$INSTALL_PATH" \
                -DCMAKE_BUILD_TYPE=$BTYPE \
                -DBUILD_TESTING=OFF \
                -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
                "$@"

            cmake --build "$CURRENT_BUILD_DIR" --target install
        done

    # CASO B: GCC / CLANG
    else
        INSTALL_PATH_REL="$COMPILER_INSTALL_ROOT/release/Catch2"
        BUILD_DIR_REL="$BUILD_DIR_BASE/release"
        INSTALL_PATH_DBG="$COMPILER_INSTALL_ROOT/debug/Catch2"
        BUILD_DIR_DBG="$BUILD_DIR_BASE/debug"
        
        rm -rf "$BUILD_DIR_REL" "$BUILD_DIR_DBG"
        # No borramos INSTALL_ROOT entero por si acaso, solo las carpetas destino si existen
        rm -rf "$INSTALL_PATH_REL" "$INSTALL_PATH_DBG"

        echo ">>> [$COMPILER_MODE] Configurando y Compilando Catch2 $CATCH2_VERSION..."
        
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
        # INTEGRACIÓN AUTOMÁTICA DEL TOOLCHAIN UPDATER
        if [ -f "./update_msvc_toolchain_v4.bash" ]; then
            ./update_msvc_toolchain_v4.bash || exit 1
        elif [ -f "./update_msvc_toolchain.bash" ]; then
            ./update_msvc_toolchain.bash || exit 1
        fi
        
        build_cmake 
        ;;
    gcc)
        ARGS=("-DCMAKE_CXX_COMPILER=g++")
        if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "win32" ]]; then
            ARGS+=("-G" "MinGW Makefiles")
        fi
        build_cmake "${ARGS[@]}"
        ;;
    clang)
        ARGS=("-DCMAKE_CXX_COMPILER=clang++" "-DCMAKE_CXX_FLAGS=-stdlib=libc++")
        if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "win32" ]]; then
            ARGS+=("-G" "MinGW Makefiles")
        fi
        build_cmake "${ARGS[@]}"
        ;;
esac

echo "=========================================="
echo " INSTALACIÓN COMPLETADA ($CATCH2_VERSION) para $COMPILER_MODE"
echo "=========================================="
```