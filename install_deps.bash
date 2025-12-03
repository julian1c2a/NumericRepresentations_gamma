#!/bin/bash

# ==============================================================================
# SCRIPT DE INSTALACIÓN DE DEPENDENCIAS (CATCH2) - VERSIÓN V3.7.1 + FIX RUNTIME
# ==============================================================================
# Uso: ./install_deps.bash [msvc|gcc|clang] [print]
# ==============================================================================

INPUT_ARG=${1:-msvc}
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
        
        # Limpieza inicial CRÍTICA para evitar mezclas de configuraciones previas
        rm -rf "$BUILD_DIR_BASE"
        rm -rf "$INSTALL_PATH"

        echo ">>> [MSVC-Ninja] Configurando y Compilando Catch2 $CATCH2_VERSION..."

        for BTYPE in Release Debug; do
            CURRENT_BUILD_DIR="${BUILD_DIR_BASE}_${BTYPE}"
            
            # --- FIX CRÍTICO RUNTIME LIBRARY ---
            # Forzamos a Catch2 a usar la misma CRT que el proyecto principal (/MD o /MDd)
            # Esto evita el error LNK2038 (mismatch MD_DynamicRelease vs MT_StaticRelease)
            if [ "$BTYPE" == "Debug" ]; then
                RT_LIB="MultiThreadedDebugDLL"
            else
                RT_LIB="MultiThreadedDLL"
            fi

            echo "   --- Construyendo $BTYPE (CRT: $RT_LIB) ---"
            
            cmake -S "$BUILD_ROOT/Catch2" -B "$CURRENT_BUILD_DIR" \
                -G "Ninja" \
                -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE" \
                -DCMAKE_INSTALL_PREFIX="$INSTALL_PATH" \
                -DCMAKE_BUILD_TYPE=$BTYPE \
                -DBUILD_TESTING=OFF \
                -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
                -DCMAKE_POLICY_DEFAULT_CMP0091=NEW \
                -DCMAKE_MSVC_RUNTIME_LIBRARY="$RT_LIB" \
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
