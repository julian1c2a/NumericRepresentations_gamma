#!/bin/bash

# ==============================================================================
# UPDATE MSVC TOOLCHAIN (V2 - MULTI-DRIVE & DYNAMIC PATH)
# ==============================================================================
# Detecta automáticamente instalaciones en C: o D:, sin importar si la carpeta
# se llama "2022", "18", "Community" o "Professional".
# ==============================================================================

TOOLCHAIN_FILE="./msvc_toolchain.cmake"
echo "=================================================="
echo " 🔎 BUSCANDO COMPILADOR MSVC (C: y D:)"
echo "=================================================="

# 1. DEFINICIÓN DE RUTAS DE BÚSQUEDA (Roots)
# Convertimos rutas de Windows a Bash. Buscaremos en C y luego en D.
POSSIBLE_ROOTS=(
    "/c/Program Files/Microsoft Visual Studio"
    "/d/Program Files/Microsoft Visual Studio"
)

FOUND_MSVC_PATH=""
HIGHEST_VERSION="0.0.0"

# 2. BUCLE DE BÚSQUEDA
for ROOT in "${POSSIBLE_ROOTS[@]}"; do
    if [ -d "$ROOT" ]; then
        echo "   -> Explorando: $ROOT ..."
        
        # Buscamos la carpeta 'MSVC' dentro de la estructura. 
        # -maxdepth 4 suele ser suficiente para: VS/2022/Community/VC/Tools/MSVC
        # Usamos 'find' para localizar la carpeta padre de las versiones.
        MSVC_TOOLS_DIRS=$(find "$ROOT" -maxdepth 4 -path "*/VC/Tools/MSVC" -type d 2>/dev/null)
        
        for TOOLS_DIR in $MSVC_TOOLS_DIRS; do
            # Dentro de MSVC, buscamos las carpetas de versión (ej: 14.50.35717)
            LATEST_VER_IN_DIR=$(find "$TOOLS_DIR" -maxdepth 1 -type d -regextype posix-extended -regex ".*/[0-9]+\.[0-9]+\.[0-9]+" | sort -V | tail -n 1)
            
            if [ ! -z "$LATEST_VER_IN_DIR" ]; then
                VER_NUM=$(basename "$LATEST_VER_IN_DIR")
                echo "      Encontrada versión: $VER_NUM en $(dirname "$TOOLS_DIR")"
                
                # Comparamos versiones (lógica simple de sort -V)
                # Si la encontrada es mayor que la actual guardada, la actualizamos.
                if [ "$(printf '%s\n' "$HIGHEST_VERSION" "$VER_NUM" | sort -V | tail -n1)" == "$VER_NUM" ]; then
                    HIGHEST_VERSION=$VER_NUM
                    FOUND_MSVC_PATH=$LATEST_VER_IN_DIR
                fi
            fi
        done
    fi
done

# 3. VERIFICACIÓN DEL RESULTADO MSVC
if [ -z "$FOUND_MSVC_PATH" ]; then
    echo "❌ ERROR: No se encontró ninguna instalación válida de MSVC en C: o D:."
    exit 1
fi

# Convertir ruta de Bash a Windows para el archivo CMake (ej: /c/Prog... -> C:/Prog...)
# Truco: sed 's|^/\(.\)/|\1:/|' convierte /c/ a c:/ (minúscula), luego lo pasamos a mayúscula si quieres, 
# pero Windows es case-insensitive, así que c:/ funciona bien.
MSVC_BASE_WIN=$(echo "$FOUND_MSVC_PATH" | sed -E 's|^/([a-zA-Z])|\1:|')

echo "✅ SELECCIONADA: $MSVC_BASE_WIN"

# ------------------------------------------------------------------------------
# 4. DETECCIÓN DE WINDOWS KITS (Generalmente siempre en C:)
# ------------------------------------------------------------------------------
KIT_ROOT="/c/Program Files (x86)/Windows Kits/10/Include"
if [ ! -d "$KIT_ROOT" ]; then
    echo "❌ ERROR: No se encuentra Windows Kits en C:."
    exit 1
fi

NEW_KIT_VERSION=$(find "$KIT_ROOT" -maxdepth 1 -type d -regextype posix-extended -regex ".*/10\.0\.[0-9]+\.[0-9]+" | sort -V | tail -n 1)
if [ -z "$NEW_KIT_VERSION" ]; then
    echo "❌ ERROR: No se detectó versión del Windows Kit."
    exit 1
fi
NEW_KIT_VERSION=$(basename "$NEW_KIT_VERSION")
echo "✅ WINDOWS KIT:  $NEW_KIT_VERSION"

echo "--------------------------------------------------"
echo "⚙️  Actualizando $TOOLCHAIN_FILE..."

# 5. APLICAR CAMBIOS
# Reemplazamos MSVC_BASE con la nueva ruta (sea C: o D:)
sed -i.bak "s|set(MSVC_BASE \".*\")|set(MSVC_BASE \"$MSVC_BASE_WIN\")|g" "$TOOLCHAIN_FILE"

# Reemplazamos KIT_VER
sed -i.bak "s|set(KIT_VER   \".*\")|set(KIT_VER   \"$NEW_KIT_VERSION\")|g" "$TOOLCHAIN_FILE"

if [ $? -eq 0 ]; then
    echo "🎉 Archivo actualizado correctamente."
    echo "   NUEVA RUTA MSVC: $MSVC_BASE_WIN"
else
    echo "❌ Fallo al editar el archivo."
    exit 1
fi