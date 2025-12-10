@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 > nul 2>&1
cd /d "d:\gdrive\numericrepresentations_gamma"

echo === Diagnóstico de entorno ===
echo PATH actual:
echo %PATH%
echo.
echo Test de compilador:
echo int main() { return 0; } > test.cpp
cl.exe /nologo test.cpp /Fe:test.exe
if %ERRORLEVEL% equ 0 (
    echo ✅ Compilador funciona directamente
    del test.cpp test.exe test.obj 2>nul
) else (
    echo ❌ Compilador no funciona
)

echo.
echo Verificando CMake...
"C:\Program Files\CMake\bin\cmake.exe" --version

echo.
echo Intentando configuración manual...
set CMAKE_C_COMPILER=cl.exe
set CMAKE_CXX_COMPILER=cl.exe
"C:\Program Files\CMake\bin\cmake.exe" -S . -B build\msvc-test -G "Visual Studio 17 2022" -A x64

pause