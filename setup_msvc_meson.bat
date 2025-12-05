@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
echo Configurando Meson con entorno MSVC...
meson setup builddir-msvc-real --backend=ninja --wipe
echo Configuración completada.
pause