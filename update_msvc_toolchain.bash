#!/bin/bash

# ==============================================================================
# UPDATE MSVC TOOLCHAIN (V8 - FORWARD SLASH FIX)
# ==============================================================================
# Genera msvc_toolchain.cmake usando SIEMPRE forward slashes (/)
# para evitar errores de "Invalid escape sequence" en CMake.
# ==============================================================================

TOOLCHAIN_FILE="./msvc_toolchain.cmake"
echo "=================================================="
echo " 🔎 BUSCANDO COMPILADOR MSVC (C: y D:)"
echo "=================================================="

# 1. DETECCIÓN DE RUTAS
POSSIBLE_ROOTS=("/c/Program Files/Microsoft Visual Studio" "/d/Program Files/Microsoft Visual Studio")
FOUND_MSVC_PATH=""
HIGHEST_VERSION="0.0.0"

for ROOT in "${POSSIBLE_ROOTS[@]}"; do
    if [ -d "$ROOT" ]; then
        echo "   -> Explorando: '$ROOT' ..."
        while IFS= read -r TOOLS_DIR; do
            [ -z "$TOOLS_DIR" ] && continue
            while IFS= read -r VER_DIR; do
                if [ ! -z "$VER_DIR" ]; then
                    VER_NUM=$(basename "$VER_DIR")
                    if [ "$VER_NUM" != "0.0.0" ] && [ "$(printf '%s\n' "$HIGHEST_VERSION" "$VER_NUM" | sort -V | tail -n1)" == "$VER_NUM" ]; then
                        HIGHEST_VERSION=$VER_NUM
                        FOUND_MSVC_PATH=$VER_DIR
                    fi
                fi
            done < <(find "$TOOLS_DIR" -maxdepth 1 -type d -regextype posix-extended -regex ".*/[0-9]+\.[0-9]+\.[0-9]+" | sort -V)
        done < <(find "$ROOT" -maxdepth 9 -type d -name "MSVC" 2>/dev/null)
    fi
done

if [ -z "$FOUND_MSVC_PATH" ]; then
    echo "❌ ERROR: No se encontró MSVC."
    exit 1
fi

# Convertir ruta a formato Windows PERO manteniendo SLASHES (/)
# CMake prefiere C:/Path/To/File incluso en Windows
MSVC_BASE_WIN=$(echo "$FOUND_MSVC_PATH" | sed -E 's|^/([a-zA-Z])|\1:/|')
echo "🏆 MSVC: $MSVC_BASE_WIN"

# 2. DETECCIÓN DE WINDOWS KITS
KIT_ROOT="/c/Program Files (x86)/Windows Kits/10/Include"
NEW_KIT_VERSION=""
if [ -d "$KIT_ROOT" ]; then
    while IFS= read -r K_PATH; do
        NEW_KIT_VERSION=$(basename "$K_PATH")
    done < <(find "$KIT_ROOT" -maxdepth 1 -type d -regextype posix-extended -regex ".*/10\.0\.[0-9]+\.[0-9]+" | sort -V | tail -n 1)
fi

if [ -z "$NEW_KIT_VERSION" ]; then
    echo "❌ ERROR: No se encontró Windows Kit."
    exit 1
fi
echo "✅ KIT:  $NEW_KIT_VERSION"

# 3. PREPARACIÓN DE RUTAS (TODO CON SLASHES)
KIT_BASE_WIN="C:/Program Files (x86)/Windows Kits/10"

# Construcción de variables de entorno (separadas por ;)
# IMPORTANTE: Usamos '/' en las rutas. Windows 10/11 y CMake lo aceptan bien.

# INCLUDE PATHS
INC_1="${MSVC_BASE_WIN}/include"
INC_2="${KIT_BASE_WIN}/Include/${NEW_KIT_VERSION}/ucrt"
INC_3="${KIT_BASE_WIN}/Include/${NEW_KIT_VERSION}/shared"
INC_4="${KIT_BASE_WIN}/Include/${NEW_KIT_VERSION}/um"
INC_5="${KIT_BASE_WIN}/Include/${NEW_KIT_VERSION}/winrt"
ENV_INCLUDE="${INC_1};${INC_2};${INC_3};${INC_4};${INC_5}"

# LIB PATHS
LIB_1="${MSVC_BASE_WIN}/lib/x64"
LIB_2="${KIT_BASE_WIN}/Lib/${NEW_KIT_VERSION}/ucrt/x64"
LIB_3="${KIT_BASE_WIN}/Lib/${NEW_KIT_VERSION}/um/x64"
ENV_LIB="${LIB_1};${LIB_2};${LIB_3}"

# PATH
ENV_PATH="${MSVC_BASE_WIN}/bin/Hostx64/x64"

# 4. GENERACIÓN DEL ARCHIVO .CMAKE
echo "--------------------------------------------------"
echo "⚙️  Generando $TOOLCHAIN_FILE (Modo Forward Slash)..."

cat > "$TOOLCHAIN_FILE" <<EOF
# msvc_toolchain.cmake (Generado V8 - Safe Slashes)
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR AMD64)

# 1. Configurar Entorno Virtual
# Usamos forward slashes (/) que no dan problemas de escape en CMake
set(ENV{INCLUDE} "${ENV_INCLUDE}")
set(ENV{LIB}     "${ENV_LIB}")
set(ENV{PATH}    "${ENV_PATH};\$ENV{PATH}")

# 2. Definir Compiladores
set(CMAKE_C_COMPILER   "${MSVC_BASE_WIN}/bin/Hostx64/x64/cl.exe")
set(CMAKE_CXX_COMPILER "${MSVC_BASE_WIN}/bin/Hostx64/x64/cl.exe")
set(CMAKE_LINKER       "${MSVC_BASE_WIN}/bin/Hostx64/x64/link.exe")
set(CMAKE_RC_COMPILER  "${KIT_BASE_WIN}/bin/${NEW_KIT_VERSION}/x64/rc.exe")
set(CMAKE_MT           "${KIT_BASE_WIN}/bin/${NEW_KIT_VERSION}/x64/mt.exe")

# 3. Flags Globales Modernas (C++23)
add_compile_options(
    /DWIN32 /D_WINDOWS /W3 /GR /EHsc 
    /DNOMINMAX 
    /Zc:preprocessor 
    /Zc:__cplusplus
)
EOF

echo "🎉 Archivo generado correctamente."
```