# msvc_toolchain.cmake (Generado V8 - Safe Slashes)
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR AMD64)

# 1. Configurar Entorno Virtual
# Usamos forward slashes (/) que no dan problemas de escape en CMake
set(ENV{INCLUDE} "c://Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.50.35717/include;C:/Program Files (x86)/Windows Kits/10/Include/10.0.26100.0/ucrt;C:/Program Files (x86)/Windows Kits/10/Include/10.0.26100.0/shared;C:/Program Files (x86)/Windows Kits/10/Include/10.0.26100.0/um;C:/Program Files (x86)/Windows Kits/10/Include/10.0.26100.0/winrt")
set(ENV{LIB}     "c://Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.50.35717/lib/x64;C:/Program Files (x86)/Windows Kits/10/Lib/10.0.26100.0/ucrt/x64;C:/Program Files (x86)/Windows Kits/10/Lib/10.0.26100.0/um/x64")
set(ENV{PATH}    "c://Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.50.35717/bin/Hostx64/x64;$ENV{PATH}")

# 2. Definir Compiladores
set(CMAKE_C_COMPILER   "c://Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.50.35717/bin/Hostx64/x64/cl.exe")
set(CMAKE_CXX_COMPILER "c://Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.50.35717/bin/Hostx64/x64/cl.exe")
set(CMAKE_LINKER       "c://Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.50.35717/bin/Hostx64/x64/link.exe")
set(CMAKE_RC_COMPILER  "C:/Program Files (x86)/Windows Kits/10/bin/10.0.26100.0/x64/rc.exe")
set(CMAKE_MT           "C:/Program Files (x86)/Windows Kits/10/bin/10.0.26100.0/x64/mt.exe")

# 3. Flags Globales Modernas (C++23)
add_compile_options(
    /DWIN32 /D_WINDOWS /W3 /GR /EHsc 
    /DNOMINMAX 
    /Zc:preprocessor 
    /Zc:__cplusplus
)
