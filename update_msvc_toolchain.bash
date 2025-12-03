#!/bin/bash

# ==============================================================================
# UPDATE MSVC TOOLCHAIN (V3 - DEEP SEARCH)
# ==============================================================================
# Busca agresivamente la carpeta MSVC aumentando la profundidad de exploración
# para soportar estructuras complejas (ej: .../18/Community/...)
# ==============================================================================

TOOLCHAIN_FILE="./msvc_toolchain.cmake"
echo "=================================================="
echo " 🔎 BUSCANDO COMPILADOR MSVC (C: y D:)"
echo "=================================================="

# 1. DEFINICIÓN DE RUTAS DE BÚSQUEDA (Roots)
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
        
        # CAMBIO V3: Usamos -name "MSVC" directamente y aumentamos maxdepth a 9
        # Esto encontrará .../VC/Tools/MSVC sin importar lo que haya en medio.
        MSVC_TOOLS_DIRS=$(find "$ROOT" -maxdepth 9 -type d -name "MSVC" 2>/dev/null)
        
        for TOOLS_DIR in $MSVC_TOOLS_DIRS; do
            # Verificar si dentro de MSVC hay carpetas con números (versiones)
            # Buscamos carpetas tipo 14.xx.xxxxx
            LATEST_VER_IN_DIR=$(find "$TOOLS_DIR" -maxdepth 1 -type d -regextype posix-extended -regex ".*/[0-9]+\.[0-9]+\.[0-9]+" | sort -V | tail -n 1)
            
            if [ ! -z "$LATEST_VER_IN_DIR" ]; then
                VER_NUM=$(basename "$LATEST_VER_IN_DIR")
                echo "      ✅ Encontrado candidato: $VER_NUM"
                echo "         Ruta: $LATEST_VER_IN_DIR"
                
                # Lógica de versión mayor
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
    echo "❌ ERROR CRÍTICO: No se encontró la carpeta 'MSVC' con versiones válidas inside."
    echo "   Por favor, verifica manualmente que existe:"
    echo "   C:/Program Files/Microsoft Visual Studio/.../VC/Tools/MSVC/<version>"
    exit 1
fi

# Convertir ruta de Bash a Windows (ej: /c/Prog... -> c:/Prog...)
MSVC_BASE_WIN=$(echo "$FOUND_MSVC_PATH" | sed -E 's|^/([a-zA-Z])|\1:|')

echo "🏆 SELECCIONADA FINAL: $MSVC_BASE_WIN"

# ------------------------------------------------------------------------------
# 4. DETECCIÓN DE WINDOWS KITS (Generalmente siempre en C:)
# ------------------------------------------------------------------------------
KIT_ROOT="/c/Program Files (x86)/Windows Kits/10/Include"
if [ ! -d "$KIT_ROOT" ]; then
    echo "❌ ERROR: No se encuentra Windows Kits en C: ($KIT_ROOT)."
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
# Reemplazamos MSVC_BASE con la nueva ruta
# Usamos separador | en sed para evitar conflictos con /
sed -i.bak "s|set(MSVC_BASE \".*\")|set(MSVC_BASE \"$MSVC_BASE_WIN\")|g" "$TOOLCHAIN_FILE"

# Reemplazamos KIT_VER
sed -i.bak "s|set(KIT_VER   \".*\")|set(KIT_VER   \"$NEW_KIT_VERSION\")|g" "$TOOLCHAIN_FILE"

if [ $? -eq 0 ]; then
    echo "🎉 ¡ÉXITO! Archivo actualizado."
else
    echo "❌ Fallo al editar el archivo."
    exit 1
fi