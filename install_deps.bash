#!/bin/bash

# ==============================================================================
# SCRIPT DE INSTALACIÓN DE DEPENDENCIAS (CATCH2)
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
TOOLCHAIN_FILE="$(pwd)/msvc_toolchain.cmake" # Ruta absoluta al toolchain

# Descarga
mkdir -p "$BUILD_ROOT"
if [ ! -d "$BUILD_ROOT/Catch2" ]; then
    echo ">>> Clonando Catch2 $CATCH2_VERSION..."
    git clone --branch $CATCH2_VERSION --depth 1 https://github.com/catchorg/Catch2.git "$BUILD_ROOT/Catch2"
fi

COMPILER_INSTALL_ROOT="$BASE_INSTALL_DIR/$COMPILER_MODE"
BUILD_DIR_BASE="$BUILD_ROOT/build_catch_$COMPILER_MODE"

build_cmake() {
    # NOTA: Ninja es un generador "Single-Configuration", por lo que necesitamos
    # carpetas de compilación separadas para Debug y Release, igual que en GCC/Clang.
    
    # Rutas base
    # Nota: MSVC con Ninja suele requerir carpetas separadas si no es Multi-Config
    # A diferencia del generador de Visual Studio, Ninja no soporta --config en build time para cambiar de target
    
    # Sin embargo, para mantener consistencia con tu estructura de carpetas de instalación:
    # Si usabas "Visual Studio", se instalaba todo en /libs_install/msvc/Catch2 (con subcarpetas lib/cmake/Catch2 dentro gestionando configs)
    # Pero usando Ninja, es mejor separar explícitamente o instalar en el mismo prefijo con cuidado.
    # Vamos a usar la misma estrategia que GCC/Clang para asegurar que no haya conflictos.

    if [ "$COMPILER_MODE" == "msvc" ]; then
        # MSVC CON NINJA + TOOLCHAIN
        # Usamos el mismo directorio de instalación base, pero compilamos dos veces.
        # Catch2 maneja bien la instalación de configs Debug/Release en el mismo prefijo si se hace correctamente,
        # pero para evitar conflictos con los .lib (catch2.lib vs catch2d.lib), es seguro instalar en el mismo sitio
        # si CMakeLists de Catch2 lo soporta. Catch2 añade 'd' al final en debug.

        INSTALL_PATH="$COMPILER_INSTALL_ROOT/Catch2"
        
        # Limpieza inicial
        rm -rf "$BUILD_DIR_BASE"
        rm -rf "$INSTALL_PATH"

        echo ">>> [MSVC-Ninja] Configurando y Compilando con Toolchain..."

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
        
        # Limpieza por seguridad
        rm -rf "$BUILD_DIR_REL" "$BUILD_DIR_DBG"
        rm -rf "$COMPILER_INSTALL_ROOT" # Limpiamos install para evitar mezclas

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
        # LLAMADA AL NUEVO SCRIPT DE DETECCIÓN Y REPARACIÓN
        ./update_msvc_toolchain.bash || exit 1
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
        # IMPORTANTE: Forzamos libc++ para coincidir con la librería nativa en clang64
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