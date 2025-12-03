# msvc_toolchain.cmake (Generado automáticamente por v5)
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR AMD64)

# Rutas detectadas
set(MSVC_BASE "c:/Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.50.35717")
set(KIT_BASE  "C:/Program Files (x86)/Windows Kits/10")
set(KIT_VER   "10.0.26100.0")

# Herramientas
set(CMAKE_C_COMPILER   "${MSVC_BASE}/bin/Hostx64/x64/cl.exe")
set(CMAKE_CXX_COMPILER "${MSVC_BASE}/bin/Hostx64/x64/cl.exe")
set(CMAKE_LINKER       "${MSVC_BASE}/bin/Hostx64/x64/link.exe")
set(CMAKE_RC_COMPILER  "${KIT_BASE}/bin/${KIT_VER}/x64/rc.exe")
set(CMAKE_MT           "${KIT_BASE}/bin/${KIT_VER}/x64/mt.exe")

# Directorios de Librerías y Cabeceras
set(MSVC_LIB_DIRS
    "${MSVC_BASE}/lib/x64"
    "${KIT_BASE}/Lib/${KIT_VER}/ucrt/x64"
    "${KIT_BASE}/Lib/${KIT_VER}/um/x64"
)

set(MSVC_INC_DIRS
    "${MSVC_BASE}/include"
    "${KIT_BASE}/Include/${KIT_VER}/ucrt"
    "${KIT_BASE}/Include/${KIT_VER}/shared"
    "${KIT_BASE}/Include/${KIT_VER}/um"
    "${KIT_BASE}/Include/${KIT_VER}/winrt"
)

# Inyección de Rutas en Flags (Ninja Workaround)
set(FLAGS_INCLUDE "")
foreach(DIR ${MSVC_INC_DIRS})
    file(TO_NATIVE_PATH "${DIR}" DIR_NATIVE)
    string(APPEND FLAGS_INCLUDE " /I \"${DIR_NATIVE}\"")
endforeach()

set(FLAGS_LIB "")
foreach(DIR ${MSVC_LIB_DIRS})
    file(TO_NATIVE_PATH "${DIR}" DIR_NATIVE)
    string(APPEND FLAGS_LIB " /LIBPATH:\"${DIR_NATIVE}\"")
endforeach()

# --- FLAGS INICIALES (AQUÍ ESTÁ LA MAGIA) ---
# Incluimos /Zc:preprocessor y /DNOMINMAX aquí para que coexistan con los Includes.
set(COMMON_FLAGS "/DWIN32 /D_WINDOWS /W3 /GR /EHsc /DNOMINMAX /Zc:preprocessor")

set(CMAKE_C_FLAGS_INIT   "${COMMON_FLAGS} ${FLAGS_INCLUDE}")
set(CMAKE_CXX_FLAGS_INIT "${COMMON_FLAGS} ${FLAGS_INCLUDE}")

set(CMAKE_EXE_LINKER_FLAGS_INIT    "${FLAGS_LIB}")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "${FLAGS_LIB}")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "${FLAGS_LIB}")
