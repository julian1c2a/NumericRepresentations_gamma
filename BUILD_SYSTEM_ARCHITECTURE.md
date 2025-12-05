# Sistema de Configuración y Scripts de Construcción

Este documento describe el conjunto completo de archivos de configuración y scripts de construcción del proyecto NumericRepresentations.

## 🔧 Sistema de Build Principal (Meson)

### Archivos Core
- **`meson.build`** - Configuración principal (208 líneas)
  - Soporte multi-compilador (GCC/Clang/MSVC)
  - Detección automática de dependencias
  - Configuración de 17+ test executables
  - Flags específicos por compilador

- **`meson_options.txt`** - Opciones de configuración (20+ opciones)
  - `compiler_simulation` - Simulación de comportamiento de compiladores
  - `constexpr_depth` y `constexpr_ops` - Límites para template metaprogramming
  - `enable_tests`, `enable_docs`, `enable_benchmarks` - Control de features
  - Configuración manual de Catch2 para Conan

## 📦 Gestión de Dependencias (Conan)

### Configuración Principal
- **`conanfile.py`** - Configuración de dependencias
  - Catch2/3.11.0 para testing
  - Meson/1.3.0 como herramienta de build
  - Opciones configurables (tests, docs, benchmarks)
  - Layout y generación de toolchain automática

### Perfiles de Compilador
- **`conan/`** - Directorio con perfiles específicos
- **`profiles/`** - Perfiles de compilador
  - `profiles/gcc` - GCC 15.2, C++23, libstdc++
  - `profiles/clang` - Configuración Clang
  - `profiles/msvc` - Configuración MSVC

## 🏗️ Sistema Legacy (CMake)

### Archivos CMake
- **`CMakeLists.txt`** - Configuración CMake (167 líneas)
  - C++23 standard obligatorio
  - Flags específicos MSVC: `/bigobj`, `/constexpr:depth8192`, `/DNOMINMAX`
  - Soporte para Clang en Windows (fix para MSYS2)
  - Configuración de todos los test targets

- **`CMakePresets.json`** - 6 presets multi-compilador
  - `gcc-debug/gcc-release` - GCC con MSYS2/UCRT64
  - `clang-debug/clang-release` - Clang con MSYS2/CLANG64  
  - `msvc-debug/msvc-release` - MSVC con Ninja generator

- **`msvc_toolchain.cmake`** - Toolchain específico MSVC
- **`msvc_toolchain.cmake.bak`** - Backup del toolchain

## 🚀 Scripts de Construcción

### Scripts Master
- **`build_tests.bash`** - Script maestro construcción CMake (36 líneas)
  - Usa presets de CMake por defecto (`gcc-release`)
  - Construye todos los test targets (test_01 a test_15)
  - Logging opcional con redirección automática

- **`check_tests.bash`** - Ejecución de tests CMake
  - Ejecuta todos los tests compilados
  - Genera logs de ejecución por compilador

- **`check_direct_tests.bash`** - Tests directos sin build
- **`install_deps.bash`** - Instalación dependencias + toolchain MSVC (123 líneas)
  - Auto-detección de Visual Studio en C: y D:
  - Generación automática de `msvc_toolchain.cmake`
  - Configuración de flags avanzados MSVC

### Scripts de Utilidades
- **`update_msvc_toolchain.bash`** - Actualización toolchain
- **`check_tools.bat`** - Verificación herramientas Windows
  - Verifica Meson, Ninja, Conan, Doxygen
  - Usa MSYS2 bash para compatibilidad

## ⚙️ Configuración de Herramientas

### Archivos de Configuración
- **`gcc_native.ini`** - Configuración específica GCC
- **`scripts/detect_catch2.py`** - Script detección Catch2 (Python)

## 📁 Directorios de Build

### Meson Builds
- **`builddir/`** - Build Meson principal
- **`builddir-gcc/`** - Build específico GCC
- **`builddir-clang/`** - Build específico Clang  
- **`builddir-msvc/`** - Build específico MSVC simulation

### CMake Builds (Legacy)
- **`build/`** - Build CMake genérico
- **`build-gcc/`** - Build CMake GCC
- **`build-clang/`** - Build CMake Clang
- **`build-release/`** - Build release específico
- **`build_deps/`** - Dependencias compiladas (Catch2)

## 📋 Logs y Reportes

### Logs Sistemáticos
- **`build_log_{compiler}_{mode}.txt`** - Logs de construcción
  - `build_log_gcc_release.txt`, `build_log_clang_debug.txt`, etc.
- **`check_log_{compiler}_{mode}.txt`** - Logs de ejecución de tests
- **`check_direct_log_{compiler}_{mode}.txt`** - Logs de tests directos
- **`deps_log*.txt`** - Logs de instalación de dependencias

## 🔍 Configuración Adicional

### VS Code Integration
- **`.vscode/`** - Configuración VS Code
  - Tasks para compilación individual
  - Settings del proyecto
  - Launch configurations

### GitHub Integration  
- **`.github/`** - Configuración GitHub
  - `copilot-instructions.md` - Guía para AI agents
  - Workflows CI/CD (si los hay)

### Documentación
- **`docs/`** - Configuración Doxygen
  - `Doxyfile.in` - Template de configuración
  - `meson.build` - Integración con sistema de build

## 🎯 Arquitectura del Sistema de Build

### Filosofía Dual System
```
MESON (Principal)          CMake (Legacy)
├── Multi-compilador       ├── Presets específicos  
├── Conan integration     ├── Scripts bash
├── Configuración moderna ├── Toolchains manuales
└── 17 test executables   └── Mantenido para transición
```

### Flujo de Trabajo Típico
```bash
# Método Meson (Recomendado)
conan install . --build=missing
meson setup builddir --buildtype=release  
ninja -C builddir
meson test -C builddir

# Método CMake (Legacy)
./build_tests.bash gcc-release
./check_tests.bash gcc-release
```

### Multi-Compilador Support
- **GCC 15.2.0** - MSYS2/UCRT64, flags optimizados
- **Clang 21.1.7** - MSYS2/CLANG64, flags corregidos para constexpr
- **MSVC 19.50** - Auto-detección, toolchain generado dinámicamente

Esta arquitectura permite desarrollo robusto con múltiples compiladores mientras mantiene compatibilidad durante la transición de CMake a Meson.