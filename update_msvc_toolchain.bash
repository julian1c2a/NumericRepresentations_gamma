#!/bin/bash

# ==============================================================================
# UPDATE MSVC TOOLCHAIN (V5 - GENERATOR)
# ==============================================================================
# 1. Busca la instalación de MSVC en C: y D:.
# 2. GENERA un archivo msvc_toolchain.cmake nuevo con todas las flags correctas
#    (Incluyendo /Zc:preprocessor y rutas de inclusión inyectadas).
# ==============================================================================

TOOLCHAIN_FILE="./msvc_toolchain.cmake"
echo "=================================================="
echo " 🔎 BUSCANDO COMPILADOR MSVC (C: y D:)"
echo "=================================================="

# --- 1. DETECCIÓN DE RUTAS (Lógica v4 probada) ---
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

MSVC_BASE_WIN=$(echo "$FOUND_MSVC_PATH" | sed -E 's|^/([a-zA-Z])|\1:|')
echo "🏆 MSVC: $MSVC_BASE_WIN"

# --- 2. DETECCIÓN DE WINDOWS KITS ---
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

# --- 3. GENERACIÓN DEL ARCHIVO .CMAKE ---
echo "--------------------------------------------------"
echo "⚙️  Generando $TOOLCHAIN_FILE..."

cat > "$TOOLCHAIN_FILE" <<EOF
# msvc_toolchain.cmake (Generado automáticamente por v5)
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR AMD64)

# Rutas detectadas
set(MSVC_BASE "$MSVC_BASE_WIN")
set(KIT_BASE  "C:/Program Files (x86)/Windows Kits/10")
set(KIT_VER   "$NEW_KIT_VERSION")

# Herramientas
set(CMAKE_C_COMPILER   "\${MSVC_BASE}/bin/Hostx64/x64/cl.exe")
set(CMAKE_CXX_COMPILER "\${MSVC_BASE}/bin/Hostx64/x64/cl.exe")
set(CMAKE_LINKER       "\${MSVC_BASE}/bin/Hostx64/x64/link.exe")
set(CMAKE_RC_COMPILER  "\${KIT_BASE}/bin/\${KIT_VER}/x64/rc.exe")
set(CMAKE_MT           "\${KIT_BASE}/bin/\${KIT_VER}/x64/mt.exe")

# Directorios de Librerías y Cabeceras
set(MSVC_LIB_DIRS
    "\${MSVC_BASE}/lib/x64"
    "\${KIT_BASE}/Lib/\${KIT_VER}/ucrt/x64"
    "\${KIT_BASE}/Lib/\${KIT_VER}/um/x64"
)

set(MSVC_INC_DIRS
    "\${MSVC_BASE}/include"
    "\${KIT_BASE}/Include/\${KIT_VER}/ucrt"
    "\${KIT_BASE}/Include/\${KIT_VER}/shared"
    "\${KIT_BASE}/Include/\${KIT_VER}/um"
    "\${KIT_BASE}/Include/\${KIT_VER}/winrt"
)

# Inyección de Rutas en Flags (Ninja Workaround)
set(FLAGS_INCLUDE "")
foreach(DIR \${MSVC_INC_DIRS})
    file(TO_NATIVE_PATH "\${DIR}" DIR_NATIVE)
    string(APPEND FLAGS_INCLUDE " /I \\"\${DIR_NATIVE}\\"")
endforeach()

set(FLAGS_LIB "")
foreach(DIR \${MSVC_LIB_DIRS})
    file(TO_NATIVE_PATH "\${DIR}" DIR_NATIVE)
    string(APPEND FLAGS_LIB " /LIBPATH:\\"\${DIR_NATIVE}\\"")
endforeach()

# --- FLAGS INICIALES (AQUÍ ESTÁ LA MAGIA) ---
# Incluimos /Zc:preprocessor y /DNOMINMAX aquí para que coexistan con los Includes.
set(COMMON_FLAGS "/DWIN32 /D_WINDOWS /W3 /GR /EHsc /DNOMINMAX /Zc:preprocessor")

set(CMAKE_C_FLAGS_INIT   "\${COMMON_FLAGS} \${FLAGS_INCLUDE}")
set(CMAKE_CXX_FLAGS_INIT "\${COMMON_FLAGS} \${FLAGS_INCLUDE}")

set(CMAKE_EXE_LINKER_FLAGS_INIT    "\${FLAGS_LIB}")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "\${FLAGS_LIB}")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "\${FLAGS_LIB}")
EOF

echo "🎉 Archivo generado correctamente."