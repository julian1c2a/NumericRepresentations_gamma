cmake_minimum_required(VERSION 3.25)

message(STATUS "========================================================")
message(STATUS "   DIAGNÓSTICO DE ENTORNO MSVC PARA CMAKE")
message(STATUS "========================================================")

# 1. Definición de las rutas base (Copiadas de tu configuración)
set(MSVC_BASE "D:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207")
set(KIT_BASE  "C:/Program Files (x86)/Windows Kits/10")
set(KIT_VER   "10.0.26100.0")

# 2. Lista de rutas críticas a verificar
set(PATHS_TO_CHECK
    "${MSVC_BASE}/lib/x64/libcpmt.lib"       # Librería estándar de C++
    "${KIT_BASE}/Lib/${KIT_VER}/ucrt/x64/libucrt.lib"   # Universal C Runtime (La que falla)
    "${KIT_BASE}/Lib/${KIT_VER}/um/x64/kernel32.lib"    # Windows Kernel Libs
    "${MSVC_BASE}/bin/Hostx64/x64/cl.exe"     # El compilador
    "${MSVC_BASE}/bin/Hostx64/x64/link.exe"   # El linker
)

message(STATUS "1. VERIFICACIÓN FÍSICA DE ARCHIVOS")
set(ALL_FOUND TRUE)
foreach(P ${PATHS_TO_CHECK})
    if(EXISTS "${P}")
        message(STATUS "  [OK] Encontrado: ${P}")
    else()
        message(WARNING "  [FALLO] NO EXISTE: ${P}")
        set(ALL_FOUND FALSE)
    endif()
endforeach()

if(NOT ALL_FOUND)
    message(FATAL_ERROR "!!! ERROR CRÍTICO: Algunas rutas no existen. Revisa la versión del SDK o de VS en las variables.")
endif()

message(STATUS "--------------------------------------------------------")
message(STATUS "2. PRUEBA DE 'NATIVIZACIÓN' DE RUTAS")
message(STATUS "   (Verificando cómo traduce CMake las barras en este entorno)")

set(TEST_PATH "${KIT_BASE}/Lib/${KIT_VER}/ucrt/x64")
file(TO_NATIVE_PATH "${TEST_PATH}" NATIVE_RESULT)

message(STATUS "   Entrada (CMake):  ${TEST_PATH}")
message(STATUS "   Salida (Nativa):  ${NATIVE_RESULT}")

# Detección de problemas con MSYS2/Cygwin
if(NATIVE_RESULT MATCHES "/")
    message(WARNING "   ! ALERTA: file(TO_NATIVE_PATH) devolvió barras '/'. Esto puede confundir a cl.exe si se pasa en variables de entorno.")
else()
    message(STATUS "   [OK] Barras invertidas detectadas correctamente.")
endif()

message(STATUS "========================================================")
message(STATUS "DIAGNÓSTICO COMPLETADO.")