@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 > nul 2>&1
cd /d "d:\gdrive\numericrepresentations_gamma"

echo Verificando compiladores...
where cl.exe
if %ERRORLEVEL% neq 0 (
    echo ❌ cl.exe no encontrado en PATH
    pause
    exit /b 1
)

echo Configurando con CMake nativo...
"C:\Program Files\CMake\bin\cmake.exe" -S . -B build\msvc-release -G "Visual Studio 17 2022" -A x64 -DCMAKE_CXX_STANDARD=20 -DCMAKE_CXX_FLAGS="/constexpr:depth8192 /constexpr:steps50000000 /bigobj"

if %ERRORLEVEL% equ 0 (
    echo ✅ Configuración exitosa, compilando...
    "C:\Program Files\CMake\bin\cmake.exe" --build build\msvc-release --config Release --target test_04_dig_t
    if %ERRORLEVEL% equ 0 (
        echo ✅ Compilación exitosa, ejecutando test...
        build\msvc-release\Release\test_04_dig_t.exe
    )
) else (
    echo ❌ Error en configuración
)
pause