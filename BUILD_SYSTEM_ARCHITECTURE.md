# Sistema de Construcción Tri-Compilador + Scripts Unificados

Este documento describe el sistema completo de construcción tri-compilador del proyecto NumericRepresentations con soporte para **GCC 15.2.0**, **MSVC 19.50.35719** y **Clang 21.1.7**, incluyendo **scripts unificados** y **soporte para benchmarks**.

## 🚀 Comandos de Uso Rápido (Scripts Unificados - RECOMENDADO)

### **Compilación Unificada**
```bash
# Compilar test específico con compilador específico
./build_tests.bash test_lookup_tables gcc release
./build_tests.bash test_04_dig_t clang debug  
./build_tests.bash test_10_dig_t_io msvc release

# Compilar todos los tests
./build_tests.bash all gcc release
./build_tests.bash all msvc debug
```

### **Testing Unificado + Benchmarks**
```bash
# Ejecutar con benchmarks
./check_tests.bash test_lookup_tables gcc bench    # Solo benchmarks (12)
./check_tests.bash test_04_dig_t clang nobench     # Sin benchmarks (~2,000 aserciones)

# Testing completo
./check_tests.bash all gcc nobench                 # Todos los tests sin benchmarks
./check_tests.bash all clang bench                 # Todos los benchmarks disponibles
```

## 🎯 Arquitectura de Scripts Unificados

### **build_tests.bash** - Compilación Inteligente
- **Parámetros**: `[test_name] [compiler] [mode] [print]`
- **Compiladores**: `gcc`, `clang`, `msvc`, `all`
- **Modos**: `debug`, `release`
- **Build Systems**: GCC (Meson), Clang (CMake), MSVC (Direct)

### **check_tests.bash** - Testing + Benchmarks  
- **Parámetros**: `[test_name] [compiler] [benchmark] [print]`
- **Benchmark Control**: `bench` (solo benchmarks), `nobench` (aserciones normales)
- **Catch2 Filters**: `[benchmark]` vs `~[benchmark]`
- **Output**: Conteo de aserciones y benchmarks ejecutados

## 🔧 Sistema de Build Subyacente

### Archivos Core
- **`meson.build`** - Configuración GCC (248 líneas)
- **`CMakeLists.txt`** - Configuración Clang/MSVC
- **`CMakePresets.json`** - Presets para tri-compilación
- **Scripts batch** - Compilación directa MSVC

#### **GCC Configuration**
```meson
# Flags específicos GCC
compile_args += [
  '-fconstexpr-depth=4096',
  '-fconstexpr-ops-limit=104857600',
  '-Wall', '-Wextra', '-Wpedantic'
]
# Catch2: Bibliotecas compiladas (óptimo rendimiento)
```

#### **MSVC Configuration** 
```meson
# Flags específicos MSVC
compile_args += [
  '/constexpr:depth4096',
  '/constexpr:steps104857600', 
  '/bigobj', '/std:c++latest'
]
# Catch2: Header-only amalgamated
```

#### **Clang Configuration**
```meson
# Flags específicos Clang
compile_args += [
  '-fconstexpr-depth=4096',
  '-fconstexpr-steps=104857600',
  '-stdlib=libstdc++',  # Forzar libstdc++ en MSYS2
  '-Wno-c++98-compat'
]
# Catch2: Header-only amalgamated
```

## 🧪 Sistema de Testing Tri-Compilador

### **Catch2 Dual Approach**
El sistema utiliza dos enfoques diferentes para Catch2 según el compilador:

#### **GCC: Bibliotecas Compiladas**
- **Archivos**: `libCatch2.a` + `libCatch2Main.a`
- **Ventajas**: Máximo rendimiento, compilación rápida de tests
- **Tests**: 6,700+ assertions ejecutadas exitosamente
- **Detección**: Automática via `cpp.find_library()`

#### **MSVC/Clang: Header-only**
- **Archivos**: `external/Catch2/extras/catch_amalgamated.hpp`
- **Implementación**: `tests/catch2_implementation.cpp`
- **Ventajas**: Máxima compatibilidad, sin dependencias externas
- **Tests**: 62+ assertions por test, completamente funcional

### **Smart Wrapper System**
```cpp
// tests/catch2_wrapper.hpp
#if defined(_MSC_VER) || defined(__clang__)
    #include "catch_amalgamated.hpp"  // Header-only
#else
    #include <catch2/catch_test_macros.hpp>  // GCC libraries
#endif
```

### **Implementation File**
```cpp
// tests/catch2_implementation.cpp
#if defined(_MSC_VER) || defined(__clang__)
#define CATCH_CONFIG_MAIN
#include "../external/Catch2/extras/catch_amalgamated.cpp"
#endif
```

## 📦 Gestión de Dependencias

### **Meson 1.10.0 (Principal)**
- **Detección automática**: Catch2 via `find_library()` y `pkg_config()`
- **Fallbacks**: Sistema robusto con múltiples métodos de detección
- **Cross-platform**: Windows con soporte tri-compilador

### **Conan 2.x (Auxiliar)**
- **`conanfile.py`** - Configuración de dependencias
  - Catch2/3.11.0 para testing
  - Meson/1.10.0 como herramienta de build
  - Opciones configurables (tests, docs, benchmarks)
  - Layout y generación de toolchain automática

### **Directorios de Build**
- **`builddir/`** - GCC build (default)
- **`builddir-msvc-real/`** - MSVC native build  
- **`builddir-clang/`** - Clang build
- **`external/Catch2/extras/`** - Header-only Catch2

### **Meson Options**
- **`meson_options.txt`** - 20+ opciones configurables
  - `compiler_simulation` - Simulación de comportamiento de compiladores
  - `constexpr_depth` y `constexpr_ops` - Límites para template metaprogramming
  - `enable_tests`, `enable_docs`, `enable_benchmarks` - Control de features
  - Configuración manual de Catch2 para Conan

### Perfiles de Compilador
- **`conan/`** - Directorio con perfiles específicos
- **`profiles/`** - Perfiles de compilador
  - `profiles/gcc` - GCC 15.2, C++23, libstdc++
  - `profiles/clang` - Configuración Clang
  - `profiles/msvc` - Configuración MSVC

## 🏗️ Sistema Legacy (CMake)

### **Estado**: Mantenido como respaldo al sistema principal Meson

### Archivos CMake
- **`CMakeLists.txt`** - Configuración CMake (167 líneas)
  - C++23 standard obligatorio
  - Flags específicos MSVC: `/bigobj`, `/constexpr:depth8192`, `/DNOMINMAX`
  - Soporte para Clang en Windows (fix para MSYS2)
  - Configuración de todos los test targets

- **`CMakePresets.json`** - 6 presets tri-compilador
  - `gcc-debug/gcc-release` - GCC con MSYS2/UCRT64
  - `clang-debug/clang-release` - Clang con MSYS2/CLANG64  
  - `msvc-debug/msvc-release` - MSVC con Ninja generator

- **`msvc_toolchain.cmake`** - Toolchain específico MSVC
- **`msvc_toolchain.cmake.bak`** - Backup del toolchain

### **Comandos CMake (Legacy)**
```bash
# GCC
cmake --preset gcc-release
cmake --build --preset gcc-release

# Clang  
cmake --preset clang-release
cmake --build --preset clang-release

# MSVC
cmake --preset msvc-release  
cmake --build --preset msvc-release
```

## ⚡ Scripts de Automatización

### **Scripts Principales**
- **`build_tests.bash`** - Construcción automática todos los tests
- **`check_tests.bash`** - Ejecución automática todos los tests  
- **`check_direct_tests.bash`** - Tests directos sin build system

### **Scripts MSVC**
- **`final_msvc_catch2_test.bat`** - Test completo MSVC con Catch2
- **`setup_msvc_meson.bat`** - Configuración rápida MSVC
- **`compile_test.bat`** - Compilación rápida individual

### **Logs de Build**
El sistema genera logs automáticos para cada combinación compilador-modo:
- `build_log_gcc_release.txt`
- `build_log_clang_release.txt`  
- `build_log_msvc_release.txt`
- `check_log_*.txt` - Logs de ejecución tests

- **`check_tests.bash`** - Ejecución de tests CMake
  - Ejecuta todos los tests compilados
  - Genera logs de ejecución por compilador

- **`check_direct_tests.bash`** - Tests directos sin build
- **`install_deps.bash`** - Instalación dependencias + toolchain MSVC (123 líneas)
  - Auto-detección de Visual Studio en C: y D:
  - Generación automática de `msvc_toolchain.cmake`
  - Configuración de flags avanzados MSVC

### Scripts de Utilidades
## 🎯 Estado Actual del Sistema (10/12/2025)

### ✅ **TRI-COMPILADOR COMPLETAMENTE FUNCIONAL**

#### **Compiladores Validados**
- **GCC 15.2.0 (MSYS2/UCRT64)**: ✅ 62 assertions, bibliotecas Catch2
- **MSVC 19.50.35719 (VS 2026)**: ✅ 62 assertions, header-only Catch2  
- **Clang 21.1.7 (MSYS2/MINGW64)**: ✅ 62 assertions, header-only Catch2

#### **Build System**
- **Meson 1.10.0**: ✅ Configuración tri-compilador automática
- **Ninja 1.13.2**: ✅ Build backend funcional en todos los compiladores
- **Catch2 3.11.0**: ✅ Dual approach (bibliotecas + header-only)

#### **Tests Validados**
- **test_04_dig_t**: ✅ 62 assertions pasando en los 3 compiladores
- **15/17 tests**: ✅ Operacionales multi-compilador
- **6,700+ assertions**: ✅ En GCC con máximo rendimiento

### 🔧 **Configuraciones Cross-Platform**
- **`gcc_native.ini`**: ✅ Rutas MSYS2/UCRT64 optimizadas
- **`msvc_native.ini`**: ✅ Visual Studio 2026 + Windows SDK
- **Clang detection**: ✅ Automática con flags `-stdlib=libstdc++`

## ⚙️ Configuración y Herramientas

### **Archivos de Configuración**
- **`gcc_native.ini`** - Cross-file específico GCC con rutas MSYS2
- **`scripts/detect_catch2.py`** - Script detección automática Catch2 (Python)
- **`check_tools.bat`** - Verificación herramientas Windows completas
  - Verifica Meson, Ninja, Conan, Doxygen, compiladores tri-suite
  - Compatibilidad MSYS2 + PowerShell + Command Prompt

## 📁 Directorios de Build

### **Meson Builds (Principales)**
- **`builddir/`** - GCC build (default)
- **`builddir-msvc-real/`** - MSVC native build  
- **`builddir-clang/`** - Clang build
- **`external/Catch2/extras/`** - Catch2 header-only shared

### **Legacy Builds (Respaldo)**
- **`build-gcc/`** - CMake GCC builds
- **`build-clang/`** - CMake Clang builds
- **`build/`** - CMake MSVC builds  
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

### **Documentación y Metadatos**
- **`docs/`** - Configuración Doxygen integrada
  - `Doxyfile.in` - Template de configuración automática
  - `meson.build` - Generación automática de documentación
- **`.github/`** - Configuración GitHub
  - `copilot-instructions.md` - Guía completa para AI agents
  - Workflows CI/CD preparados para multi-compilador

## 🏛️ Arquitectura del Sistema de Build

### **Filosofía Tri-Compilador**
```
MESON (Principal)              CMake (Legacy Respaldo)
├── Tri-compilador completo   ├── 6 presets específicos
├── Catch2 dual approach      ├── Scripts automatización bash  
├── Auto-detection inteligente├── Toolchains manuales
├── 17 test executables       └── Mantenido como failsafe
└── C++23 optimizado
```

### **Flujo de Trabajo Recomendado**
```bash
# 1. Seleccionar compilador
# GCC (default)
meson setup builddir --buildtype=release

# MSVC  
call vcvarsall.bat x64
meson setup builddir-msvc-real --buildtype=release

# Clang
$env:CC = "C:\msys64\mingw64\bin\clang.exe"
$env:CXX = "C:\msys64\mingw64\bin\clang++.exe"
meson setup builddir-clang --buildtype=release

# 2. Compilar y probar
meson compile -C <builddir>
meson test -C <builddir> test_04_dig_t

# 3. Test completo (GCC)
meson test -C builddir --verbose  # 6,700+ assertions
```

### **Sistema de Failover**
```bash
# Si Meson falla, usar CMake
cmake --preset gcc-release
cmake --build --preset gcc-release

# O usar scripts automáticos
./build_tests.bash gcc-release
./check_tests.bash gcc-release
```

## 🚀 Próximos Pasos

### **Optimizaciones Futuras**
- [ ] CI/CD pipeline tri-compilador en GitHub Actions
- [ ] Conan 2.x migration completa
- [ ] Benchmarking automático performance tri-compilador
- [ ] Documentación automática API con Doxygen

### **Mantenimiento**
- ✅ Sistema tri-compilador completamente funcional
- ✅ Tests validados 62 assertions × 3 compiladores  
- ✅ Documentación actualizada BUILD_SYSTEM_ARCHITECTURE.md
- ✅ Ready for production development
./build_tests.bash gcc-release
./check_tests.bash gcc-release
```

### Multi-Compilador Support
- **GCC 15.2.0** - MSYS2/UCRT64, flags optimizados
- **Clang 21.1.7** - MSYS2/CLANG64, flags corregidos para constexpr
- **MSVC 19.50** - Auto-detección, toolchain generado dinámicamente

Esta arquitectura permite desarrollo robusto con múltiples compiladores mientras mantiene compatibilidad durante la transición de CMake a Meson.