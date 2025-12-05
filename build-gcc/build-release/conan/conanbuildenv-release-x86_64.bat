@echo off
chcp 65001 > nul
setlocal
echo @echo off > "%~dp0/deactivate_conanbuildenv-release-x86_64.bat"
echo echo Restoring environment >> "%~dp0/deactivate_conanbuildenv-release-x86_64.bat"
for %%v in (CXX CC PATH) do (
    set foundenvvar=
    for /f "delims== tokens=1,2" %%a in ('set') do (
        if /I "%%a" == "%%v" (
            echo set "%%a=%%b">> "%~dp0/deactivate_conanbuildenv-release-x86_64.bat"
            set foundenvvar=1
        )
    )
    if not defined foundenvvar (
        echo set %%v=>> "%~dp0/deactivate_conanbuildenv-release-x86_64.bat"
    )
)
endlocal


set "CXX=c:/msys64/ucrt64/bin/g++.exe"
set "CC=c:/msys64/ucrt64/bin/gcc.exe"
set "PATH=C:\Users\julia\.conan2\p\meson85908f9d185b9\f\bin;C:\Users\julia\.conan2\p\ninjaa3155d7c5e37b\p\bin;%PATH%"