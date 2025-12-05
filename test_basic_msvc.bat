@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
cd /d "d:\gdrive\numericrepresentations_gamma"
cl /EHsc /std:c++latest test_basic_msvc.cpp /Fe:test_basic_msvc.exe