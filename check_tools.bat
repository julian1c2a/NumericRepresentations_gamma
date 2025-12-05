@echo off
echo Checking for Meson, Ninja, Conan, Doxygen...
C:\msys64\usr\bin\bash.exe -l -c "which meson ninja conan doxygen 2>&1"
echo.
echo Versions:
C:\msys64\usr\bin\bash.exe -l -c "meson --version 2>&1"
C:\msys64\usr\bin\bash.exe -l -c "ninja --version 2>&1"
C:\msys64\usr\bin\bash.exe -l -c "conan --version 2>&1"
C:\msys64\usr\bin\bash.exe -l -c "doxygen --version 2>&1"
