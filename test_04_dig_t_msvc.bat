@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
cd /d "d:\gdrive\numericrepresentations_gamma"
cl /EHsc /std:c++latest /I include /constexpr:depth8192 /constexpr:steps50000000 /bigobj /W4 /permissive- test_04_dig_t_msvc.cpp /Fe:test_04_dig_t_msvc.exe