@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1
echo Reconfigurando Meson con MSVC...
meson setup builddir-msvc-real --reconfigure
echo Compilando test_04_dig_t con MSVC...
meson compile -C builddir-msvc-real test_04_dig_t
echo Compilación completada.
pause