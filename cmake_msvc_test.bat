@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
cd /d "d:\gdrive\numericrepresentations_gamma"
echo Configurando CMake con MSVC...
cmake --preset msvc-release
if %ERRORLEVEL% equ 0 (
    echo ✅ Configuración exitosa, compilando test_04_dig_t...
    cmake --build --preset msvc-release --target test_04_dig_t
    if %ERRORLEVEL% equ 0 (
        echo ✅ Compilación exitosa, ejecutando test...
        build\build_targets\msvc\release\Release\test_04_dig_t.exe
    )
) else (
    echo ❌ Error en configuración
)
pause