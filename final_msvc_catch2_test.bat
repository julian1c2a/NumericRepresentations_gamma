@echo off
cd /d "D:\GDrive\NumericRepresentations_gamma"
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1
echo Reconfigurando meson con nuevos archivos...
meson setup builddir-msvc-real --reconfigure
echo Compilando test_04_dig_t con MSVC + Catch2 amalgamated...
meson compile -C builddir-msvc-real test_04_dig_t
if %ERRORLEVEL% == 0 (
    echo.
    echo ✅ COMPILACIÓN EXITOSA! Ejecutando test...
    .\builddir-msvc-real\tests\test_04_dig_t.exe
) else (
    echo.
    echo ❌ Error en compilación
)