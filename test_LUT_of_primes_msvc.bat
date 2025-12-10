@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
cd /d "d:\gdrive\numericrepresentations_gamma"
echo Compilando test_LUT_of_primes con MSVC...
cl /EHsc /std:c++latest /I include /I tests/external /constexpr:depth8192 /constexpr:steps50000000 /bigobj /W4 /permissive- tests/test_LUT_of_primes.cpp tests/external/catch_amalgamated.cpp /Fe:test_LUT_of_primes_msvc.exe
if %ERRORLEVEL% equ 0 (
    echo ✅ Compilación exitosa, ejecutando test...
    test_LUT_of_primes_msvc.exe
) else (
    echo ❌ Error en compilación
)
pause