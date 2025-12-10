#!/bin/bash

echo "Iniciando test de build script..."

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

# Configurar builddir si no existe
if [ ! -d "$BUILDDIR" ]; then
    echo "Configurando builddir..."
    if [ "$MODE" == "debug" ]; then
        meson setup "$BUILDDIR" --buildtype=debug
    else
        meson setup "$BUILDDIR" --buildtype=release
    fi
fi

# Compilar
echo "Compilando $TEST_NAME..."
meson compile "$TEST_NAME" -C "$BUILDDIR"

echo "¡Listo!"