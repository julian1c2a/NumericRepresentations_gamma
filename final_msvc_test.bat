@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1
echo Reconfigurando Meson con MSVC (sin verificación de archivo)...
meson setup builddir-msvc-real --wipe
if %ERRORLEVEL% neq 0 (
    echo Error en la configuración de Meson
    pause
    exit /b 1
)
echo Compilando test_04_dig_t con MSVC...
meson compile -C builddir-msvc-real test_04_dig_t
echo Compilación completada.
pause