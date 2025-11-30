#!/bin/bash

# ==============================================================================
# SCRIPT INTELIGENTE DE INSTALACIÓN DE DEPENDENCIAS (CATCH2)
# ==============================================================================
# Uso: 
#   ./install_deps.bash              -> Instala para MSVC (Visual Studio) por defecto
#   ./install_deps.bash msvc         -> Instala para MSVC
#   ./install_deps.bash gcc          -> Instala para GCC (MinGW)
#   ./install_deps.bash clang        -> Instala para Clang
#   ./install_deps.bash gcc-debug    -> Detecta 'gcc' e instala para GCC
# ==============================================================================

# 1. LÓGICA DE DETECCIÓN DE ARGUMENTOS
# ------------------------------------
INPUT_ARG=${1:-msvc} # Si está vacío, asumimos msvc

if [[ "$INPUT_ARG" == *"msvc"* ]]; then
    COMPILER_MODE="msvc"
elif [[ "$INPUT_ARG" == *"gcc"* ]]; then
    COMPILER_MODE="gcc"
elif [[ "$INPUT_ARG" == *"clang"* ]]; then
    COMPILER_MODE="clang"
else
    echo "Advertencia: No se pudo deducir el compilador de '$INPUT_ARG'. Asumiendo 'msvc'."
    COMPILER_MODE="msvc"
fi

CATCH2_VERSION="v3.4.0"
BASE_INSTALL_DIR="$(pwd)/libs_install"
BUILD_ROOT="build_deps"

echo "=========================================="
echo " PREPARANDO DEPENDENCIAS (CATCH2)"
echo " Argumento recibido: '$INPUT_ARG'"
echo " Compilador detectado: $COMPILER_MODE"
echo "=========================================="

# 2. DESCARGA
# -----------
mkdir -p "$BUILD_ROOT"
if [ ! -d "$BUILD_ROOT/Catch2" ]; then
    echo ">>> Clonando Catch2 $CATCH2_VERSION..."
    git clone --branch $CATCH2_VERSION --depth 1 https://github.com/catchorg/Catch2.git "$BUILD_ROOT/Catch2"
fi

# Definimos la raíz de instalación para este compilador
COMPILER_INSTALL_ROOT="$BASE_INSTALL_DIR/$COMPILER_MODE"
BUILD_DIR_BASE="$BUILD_ROOT/build_catch_$COMPILER_MODE"

# 3. FUNCIÓN DE COMPILACIÓN ROBUSTA (ARRAYS)
# ------------------------------------------
build_cmake() {
    # Recibimos argumentos en "$@" (preserva espacios como "MinGW Makefiles")
    
    # CASO A: MSVC (Multi-Configuración)
    # Visual Studio maneja Debug y Release en la misma carpeta de proyecto.
    # Instalamos ambos en la MISMA ruta final (libs_install/msvc/Catch2)
    # y CMake generará los targets que eligen la lib correcta automáticamente.
    if [ "$COMPILER_MODE" == "msvc" ]; then
        INSTALL_PATH="$COMPILER_INSTALL_ROOT/Catch2"
        BUILD_DIR="$BUILD_DIR_BASE"
        
        echo ">>> [MSVC] Configurando en $BUILD_DIR..."
        
        cmake -S "$BUILD_ROOT/Catch2" -B "$BUILD_DIR" \
            -DCMAKE_INSTALL_PREFIX="$INSTALL_PATH" \
            -DBUILD_TESTING=OFF \
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
            "$@"

        echo ">>> [MSVC] Compilando e Instalando (Release)..."
        cmake --build "$BUILD_DIR" --config Release --target install
        echo ">>> [MSVC] Compilando e Instalando (Debug)..."
        cmake --build "$BUILD_DIR" --config Debug --target install

    # CASO B: GCC / CLANG (Single-Config)
    # Makefiles/Ninja son de una sola configuración.
    # Instalamos en carpetas SEPARADAS (.../release/Catch2 y .../debug/Catch2)
    # para evitar que una sobrescriba a la otra.
    else
        # --- Release ---
        INSTALL_PATH_REL="$COMPILER_INSTALL_ROOT/release/Catch2"
        BUILD_DIR_REL="$BUILD_DIR_BASE/release"
        
        echo ">>> [$COMPILER_MODE] Configurando RELEASE en $BUILD_DIR_REL..."
        cmake -S "$BUILD_ROOT/Catch2" -B "$BUILD_DIR_REL" \
            -DCMAKE_INSTALL_PREFIX="$INSTALL_PATH_REL" \
            -DCMAKE_BUILD_TYPE=Release \
            -DBUILD_TESTING=OFF \
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
            "$@"
        
        echo ">>> [$COMPILER_MODE] Instalando RELEASE..."
        cmake --build "$BUILD_DIR_REL" --target install

        # --- Debug ---
        INSTALL_PATH_DBG="$COMPILER_INSTALL_ROOT/debug/Catch2"
        BUILD_DIR_DBG="$BUILD_DIR_BASE/debug"

        echo ">>> [$COMPILER_MODE] Configurando DEBUG en $BUILD_DIR_DBG..."
        cmake -S "$BUILD_ROOT/Catch2" -B "$BUILD_DIR_DBG" \
            -DCMAKE_INSTALL_PREFIX="$INSTALL_PATH_DBG" \
            -DCMAKE_BUILD_TYPE=Debug \
            -DBUILD_TESTING=OFF \
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
            "$@"
            
        echo ">>> [$COMPILER_MODE] Instalando DEBUG..."
        cmake --build "$BUILD_DIR_DBG" --target install
    fi
}

# 4. EJECUCIÓN SEGÚN COMPILADOR
# -----------------------------
case "$COMPILER_MODE" in
    msvc)
        # MSVC suele autodetectarse en Windows sin generador explícito
        build_cmake
        ;;
    gcc)
        # Usamos ARRAYS para que "-G MinGW Makefiles" no se rompa por espacios
        ARGS=("-DCMAKE_CXX_COMPILER=g++")
        if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "win32" ]]; then
            ARGS+=("-G" "MinGW Makefiles")
        fi
        build_cmake "${ARGS[@]}"
        ;;
    clang)
        ARGS=("-DCMAKE_CXX_COMPILER=clang++")
        if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "win32" ]]; then
            ARGS+=("-G" "MinGW Makefiles")
        fi
        build_cmake "${ARGS[@]}"
        ;;
esac

echo "=========================================="
echo " INSTALACIÓN COMPLETADA para $COMPILER_MODE"
echo "=========================================="