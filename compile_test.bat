@echo off
cd /d "D:\GDrive\NumericRepresentations_gamma"
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
meson compile -C builddir-msvc-real test_04_dig_t