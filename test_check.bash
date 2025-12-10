#!/bin/bash

echo "Iniciando test de check script..."

# Parámetros
TEST_NAME=${1:-test_02_append}
MODE=${2:-release}

echo "Test: $TEST_NAME"
echo "Modo: $MODE"

# Configurar builddir
case "$MODE" in
    "debug")
        BUILDDIR="builddir-debug"
        ;;
    "release")
        BUILDDIR="builddir"
        ;;
    *)
        echo "Error: Modo no válido"
        exit 1
        ;;
esac

echo "Builddir: $BUILDDIR"

# Verificar que existe el ejecutable
EXE_PATH="$BUILDDIR/tests/$TEST_NAME.exe"
if [ ! -f "$EXE_PATH" ]; then
    echo "❌ Ejecutable no encontrado: $EXE_PATH"
    exit 1
fi

echo "Ejecutando $TEST_NAME..."
"$EXE_PATH"

echo "¡Test completado!"