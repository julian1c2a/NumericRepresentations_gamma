@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
cd /d "d:\gdrive\numericrepresentations_gamma"  
cl /EHsc /std:c++latest /I include /constexpr:depth8192 /constexpr:steps50000000 /bigobj /W4 /permissive- test_msvc_fixes.cpp /Fe:test_msvc_fixes.exe
if %ERRORLEVEL% equ 0 (
    echo ✅ Compilación exitosa, ejecutando test...
    test_msvc_fixes.exe
) else (
    echo ❌ Error en compilación
)
pause