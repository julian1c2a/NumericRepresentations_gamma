#!/bin/bash

# ==============================================================================
# UPDATE MSVC TOOLCHAIN (V4 - SPACE SAFE)
# ==============================================================================
# Soluciona el problema de "Word Splitting" en rutas con espacios.
# Usa Process Substitution para manejar "Program Files" correctamente.
# ==============================================================================

TOOLCHAIN_FILE="./msvc_toolchain.cmake"

echo "=================================================="
echo " 🔎 BUSCANDO COMPILADOR MSVC (C: y D:)"
echo "=================================================="

# Rutas raíz donde buscar.
POSSIBLE_ROOTS=(
    "/c/Program Files/Microsoft Visual Studio"
    "/d/Program Files/Microsoft Visual Studio"
)

FOUND_MSVC_PATH=""
HIGHEST_VERSION="0.0.0"

# Recorremos cada unidad (C: y D:)
for ROOT in "${POSSIBLE_ROOTS[@]}"; do
    if [ -d "$ROOT" ]; then
        echo "   -> Explorando raíz: '$ROOT' ..."
        
        # ----------------------------------------------------------------------
        # SOLUCIÓN CLAVE: Usamos 'while read' con redirección de proceso (< <)
        # Esto evita que los espacios en "Program Files" rompan la variable.
        # ----------------------------------------------------------------------
        while IFS= read -r TOOLS_DIR; do
            
            # Si find no encuentra nada, a veces devuelve línea vacía. Ignorar.
            [ -z "$TOOLS_DIR" ] && continue
            
            echo "      📂 Analizando contenedor: '$TOOLS_DIR'"

            # Buscamos versiones dentro de esta carpeta MSVC encontrada.
            # Usamos el mismo truco 'while read' para la búsqueda interna por seguridad.
            while IFS= read -r VER_DIR; do
                if [ ! -z "$VER_DIR" ]; then
                    VER_NUM=$(basename "$VER_DIR")
                    
                    # Comparación de versiones
                    if [ "$(printf '%s\n' "$HIGHEST_VERSION" "$VER_NUM" | sort -V | tail -n1)" == "$VER_NUM" ]; then
                        # Evitamos falsos positivos con la versión "0.0.0" inicial
                        if [ "$VER_NUM" != "0.0.0" ]; then
                            echo "         ⭐ Nueva versión detectada: $VER_NUM"
                            HIGHEST_VERSION=$VER_NUM
                            FOUND_MSVC_PATH=$VER_DIR
                        fi
                    fi
                fi
            done < <(find "$TOOLS_DIR" -maxdepth 1 -type d -regextype posix-extended -regex ".*/[0-9]+\.[0-9]+\.[0-9]+" | sort -V)

        done < <(find "$ROOT" -maxdepth 9 -type d -name "MSVC" 2>/dev/null)
    else
        echo "   -> Saltando '$ROOT' (No existe)"
    fi
done

# ------------------------------------------------------------------------------
# VERIFICACIÓN DEL RESULTADO
# ------------------------------------------------------------------------------
if [ -z "$FOUND_MSVC_PATH" ]; then
    echo "❌ ERROR CRÍTICO: No se encontró ninguna instalación de MSVC."
    echo "   Verifica que tienes instalado el componente 'MSVC v143 - VS 2022 C++ x64/x86 build tools'."
    exit 1
fi

# Convertir ruta de Bash a Windows para CMake (ej: /c/Prog... -> C:/Prog...)
MSVC_BASE_WIN=$(echo "$FOUND_MSVC_PATH" | sed -E 's|^/([a-zA-Z])|\1:|')
echo "🏆 RUTA SELECCIONADA: $MSVC_BASE_WIN"

# ------------------------------------------------------------------------------
# DETECCIÓN DE WINDOWS KITS (Seguridad de espacios añadida)
# ------------------------------------------------------------------------------
KIT_ROOT="/c/Program Files (x86)/Windows Kits/10/Include"
NEW_KIT_VERSION=""

if [ -d "$KIT_ROOT" ]; then
    # Usamos find con sort para obtener el último
    while IFS= read -r K_PATH; do
        NEW_KIT_VERSION=$(basename "$K_PATH")
    done < <(find "$KIT_ROOT" -maxdepth 1 -type d -regextype posix-extended -regex ".*/10\.0\.[0-9]+\.[0-9]+" | sort -V | tail -n 1)
fi

if [ -z "$NEW_KIT_VERSION" ]; then
    echo "❌ ERROR: No se detectó versión del Windows Kit en '$KIT_ROOT'."
    exit 1
fi
echo "✅ WINDOWS KIT: $NEW_KIT_VERSION"

# ------------------------------------------------------------------------------
# ACTUALIZACIÓN DEL ARCHIVO
# ------------------------------------------------------------------------------
echo "--------------------------------------------------"
echo "⚙️  Actualizando $TOOLCHAIN_FILE..."

# Usamos | como delimitador para evitar conflicto con las barras de la ruta
sed -i.bak "s|set(MSVC_BASE \".*\")|set(MSVC_BASE \"$MSVC_BASE_WIN\")|g" "$TOOLCHAIN_FILE"
sed -i.bak "s|set(KIT_VER   \".*\")|set(KIT_VER   \"$NEW_KIT_VERSION\")|g" "$TOOLCHAIN_FILE"

if [ $? -eq 0 ]; then
    echo "🎉 ¡ÉXITO! Archivo actualizado correctamente."
else
    echo "❌ Fallo al escribir en el archivo."
    exit 1
fi