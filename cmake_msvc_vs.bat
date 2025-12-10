@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 > nul 2>&1
cd /d "d:\gdrive\numericrepresentations_gamma"

echo Usando CMake de Visual Studio...
set CMAKE_CMD="C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"

%CMAKE_CMD% --version

echo Configurando proyecto...
%CMAKE_CMD% -S . -B build\msvc-vs-test -G "Visual Studio 17 2022" -A x64 -DCMAKE_CXX_STANDARD=20

if %ERRORLEVEL% equ 0 (
    echo ✅ Configuración exitosa, compilando test_04_dig_t...
    %CMAKE_CMD% --build build\msvc-vs-test --config Release --target test_04_dig_t
    if %ERRORLEVEL% equ 0 (
        echo ✅ Compilación exitosa, ejecutando...
        build\msvc-vs-test\Release\test_04_dig_t.exe
    )
) else (
    echo ❌ Error en configuración
)
pause