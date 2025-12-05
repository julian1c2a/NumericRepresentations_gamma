@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
cd /d "d:\gdrive\numericrepresentations_gamma\build-msvc-conan"
conan install .. -s compiler.cppstd=23 -s build_type=Release --build=missing