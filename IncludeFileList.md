En `NumRepr/CMakeLists.txt` tengo que mis cabeceras están en `NumRepr/include/.`, mis tests están en `NumRepr/tests/.`, tengo `cpp` en `NumRepr/src` con la intención de crear tablas para las cabeceras `hpp` etc. Pídeme los archivos que te subo. No sé si debería haber incorporado el sistema de directorios y subdirectorios al sistema de `namespaces`, pero hubiesen quedado unos nombres horrorosamente largos.

***CONFIGURACIÓN DEL PROYECTO NUMREPR***

**FICHEROS DE CONFIGURACIÓN DEL PROYECTO**

Configuración del proyecto:
    
    //NumRepr/CMakeLists.txt
    //NumRepr/CMakePresets.json
    //NumRepr/msvc_toolchain.cmake

Aún queda por introducir los flags de compilación para `msvc` `/constexpr:depth8192 /constexpr:steps4194304 /bigobj /permissive-`.

Estos ficheros debeberían ser absolutamente intocables, puesto que ha costado una barbaridad que mediante la herramienta `cmake` consigamos desde un terminal de `MSYS2/MINGW64 (BASH, UCRT64 o CLANG64)` compilar el proyecto. Los comandos son:
    
Comandos de configuración e inicialización de los path de los ejecutables:

    PROMPT> cmake --preset gcc-release
    PROMPT> cmake --preset gcc-debug
    PROMPT> cmake --preset clang-release
    PROMPT> cmake --preset clang-debug
    PROMPT> cmake --preset msvc-release
    PROMPT> cmake --preset msvc-debug

Comandos de construcción. La primera vez que compilan, compilan Catch2, pero dentro de los caminos de los ejecutables que se van a producir:

    PROMPT> cmake --build --preset gcc-release --target xxxx // xxxx: target definido en CMakeLists.txt
    PROMPT> cmake --build --preset gcc-debug --target xxxx // xxxx: target definido en CMakeLists.txt
    PROMPT> cmake --build --preset clang-release --target xxxx // xxxx: target definido en CMakeLists.txt
    PROMPT> cmake --build --preset clang-debug --target xxxx // xxxx: target definido en CMakeLists.txt
    PROMPT> cmake --build --preset msvc-release --target xxxx // xxxx: target definido en CMakeLists.txt
    PROMPT> cmake --build --preset msvc-debug --target xxxx // xxxx: target definido en CMakeLists.txt

Los comandos de testeo son los proporcionados por `CTest` o los de ejecución directa:

    PROMPT> ctest --preset gcc-debug -R xxxx // xxxx: target definido en CMakeLists.txt
    PROMPT> ctest --preset gcc-release -R xxxx // xxxx: target definido en CMakeLists.txt
    PROMPT> ctest --preset clang-debug -R xxxx // xxxx: target definido en CMakeLists.txt
    PROMPT> ctest --preset clang-release -R xxxx // xxxx: target definido en CMakeLists.txt
    PROMPT> ctest --preset msvc-debug -R xxxx // xxxx: target definido en CMakeLists.txt
    PROMPT> ctest --preset msvc-release -R xxxx // xxxx: target definido en CMakeLists.txt

    PROMPT> build/build_targets/gcc/debug/xxxx.exe // xxxx: target definido en CMakeLists.txt
    PROMPT> build/build_targets/gcc/release/xxxx.exe // xxxx: target definido en CMakeLists.txt
    PROMPT> build/build_targets/clang/debug/xxxx.exe // xxxx: target definido en CMakeLists.txt
    PROMPT> build/build_targets/clang/release/xxxx.exe // xxxx: target definido en CMakeLists.txt
    PROMPT> build/build_targets/msvc/debug/xxxx.exe // xxxx: target definido en CMakeLists.txt
    PROMPT> build/build_targets/msvc/debug/xxxx.exe // xxxx: target definido en CMakeLists.txt

Los archivos log que tenemos en nuestro proyecto son:

    //NumRepr/build_log.txt  # Se consiguen mediante PROMPT> comando_cmake > build_log.txt 2>&1
    //NumRepr/test_log.txt  # Se consiguen mediante PROMPT> comando_ctest > test_log.txt 2>&1
    //NumRepr/test_log_direct.txt  # Se consiguen mediante PROMPT> build/build_targets/[target]/xxxx.exe > test_log_direct.txt 2>&1

**ARCHIVOS EJECUTABLES DEL PROYECTO EN //NUMREPR/BUILD/BUILD_TARGETS.**

Podemos compilar tanto con `gcc`, `clang` y `msvc`, tanto en modo `debug` como en modo `release`. Se crean 3 subdirectorios de `//NumRepr/build/build_targets/`, a saber, `//NumRepr/build/build_targets/gcc/`, `//NumRepr/build/build_targets/clang/` y `//NumRepr/build/build_targets/msvc/`. De cada uno de estos cuelgan 2 subdirectorios, que es dónde estarán los ejecutables:

    //NumRepr/build/build_targets/
    //NumRepr/build/build_targets/gcc/
    //NumRepr/build/build_targets/gcc/debug/[executables]       -- CORRESPONDEN AL TARGET GCC-DEBUG
    //NumRepr/build/build_targets/gcc/release/[executables]     -- CORRESPONDEN AL TARGET GCC-RELEASE
    //NumRepr/build/build_targets/clang/
    //NumRepr/build/build_targets/clang/debug/[executables]     -- CORRESPONDEN AL TARGET CLANG-DEBUG
    //NumRepr/build/build_targets/clang/release/[executables]   -- CORRESPONDEN AL TARGET CLANG-RELEASE
    //NumRepr/build/build_targets/msvc/
    //NumRepr/build/build_targets/msvc/debug/[executables]      -- CORRESPONDEN AL TARGET MSVC-DEBUG
    //NumRepr/build/build_targets/msvc/release/[executables]    -- CORRESPONDEN AL TARGET MSVC-DEBUG

***CODE***

**INCLUDE/CORE/INTERNAL/MATH/TABLES**

De estas cabeceras `C++ hpp` necesito pruebas (integradas en `Catch2` + `CTest`) en `NumRepr/tests`. A `date: 23/11/2025` están casi finalizadas. Falta una tabla `lookup table` que diga las parejas `{base, máximo exponente en uint64_t}` que se borró milagrosamente. 

    //NumRepr/include/core/internal/math/tables/EratosthenesSieve_table.hpp
    //NumRepr/include/core/internal/math/tables/EratosthenesSieve.hpp
    //NumRepr/include/core/internal/math/tables/PrimeList.hpp
    //NumRepr/include/core/internal/math/tables/MaxExp4Base_table.hpp

**INCLUDE/CORE/INTERNAL/MATH**

De estas cabeceras `C++ hpp` necesito pruebas (integradas en `Catch2` + `CTest`) en `NumRepr/tests`. A `date: 23/11/2025` están casi finalizadas. Falta una función de seguridad, tanto en tiempo de compilación como de ejecución para las funciones exponenciales, que tiene una cadena de `ifs` dependiente de la tabla de parejas `{base, máximo exponente en uint64_t}` que se borró milagrosamente. Para las demás sí que necesito las pruebas.

Estoy teniendo la mala idea de renombrar `IntExpIntLog.hpp` a `Int_ExpLog.hpp`, y de seguir ahondando en la refactorización, haciendo como en las cabeceras `primes`, dividiendo las que merezcan la pena entre sufijos `""` (para las funciones en tiempo de ejecución) y `"_ct"` (para las de tiempo de compilación). ¿?

    //NumRepr/include/core/internal/math/IntExpIntLog.hpp [[ERASED]] [[REFACTORIZED]] `date: 28/11/2025`
	//NumRepr/include/core/internal/math/Int_ExpLog.hpp [[RENAMED FROM IntExpIntLog.hpp]] `date: 28/11/2025`
    //NumRepr/include/core/internal/math/IntRoot.hpp
    //NumRepr/include/core/internal/math/math_utils.hpp [[DEPRECATED]][[EMPTY]]
    //NumRepr/include/core/internal/math/primes.hpp
    //NumRepr/include/core/internal/math/primes_ct.hpp

**INCLUDE/CORE/INTERNAL/APPEND**

De estas cabeceras `C++ hpp` necesito pruebas (integradas en `Catch2` + `CTest`) en `NumRepr/tests`. A `date: 23/11/2025` están casi finalizadas. ¿Se podría integrar el sistema de meter la pareja de elementos de 64 bits en una pareja de registros y poner el código ensamblador para  `x86_64`, `arm64` y `risc-v` (64 bits)? No creo que merezca la pena pero como comentarios para saber el camino sí que es interesante.

    NumRepr/include/core/internal/append/int128_ops.hpp
    NumRepr/include/core/internal/append/integers.hpp
    NumRepr/include/core/internal/append/expected.hpp

**INCLUDE/CORE/INTERNAL**

De estas cabeceras `C++ hpp` necesito pruebas (integradas en `Catch2` + `CTest`) en `NumRepr/tests`. A `date: 23/11/2025` están casi finalizadas, a falta de tener los `includes` arreglados, (no lo están por la refactorización). Aquí se necesitaría una cobertura fuerte, profunda, amplia tanto en casos regulares como (mucho más) en los casos límite y distintos tipos. Necesito la recomposición de los `includes`.

    //NumRepr/include/core/internal/AuxFunc.hpp  [[DEPRECATED]][[ALMOST EMPTY]]
    //NumRepr/include/core/internal/basic_types.hpp
    //NumRepr/include/core/internal/conversions.hpp

**INCLUDE/CORE**

De estas cabeceras `C++ hpp` necesito pruebas (integradas en `Catch2` + `CTest`) en `NumRepr/tests`. A `date: 23/11/2025` están casi finalizadas, a falta de tener los `includes` arreglados, (no lo están por la refactorización). Aquí se necesitaría una cobertura fuerte, profunda, amplia tanto en casos regulares como (mucho más) en los casos límite y distintas bases que impliquen distintos tipos. Necesito la recomposición de los `includes`. A `date: 28/11/2025` se puede considerar que excepto ampliaciones, todo es ya correcto y ha pasado los 10 test_*.cpp limpiamente.

    //NumRepr/include/core/dig_t.hpp
    //NumRepr/include/core/dig_t_display_helpers.hpp

**INCLUDE/FIXED_LENGTH/CONTAINERS**

Por ahora, a `date: 23/11/2025`, no se mira.

    //NumRepr/include/fixed_length/containers/reg_digs_t.hpp

**INCLUDE/FIXED_LENGTH**

Por ahora, a `date: 23/11/2025`, no se mira.

    //NumRepr/include/fixed_length/nat_reg_digs_t.hpp
    //NumRepr/include/fixed_length/int_reg_digs_t.hpp
    //NumRepr/include/fixed_length/numeric_traits_specializations.hpp

**INCLUDE/UNLIMITED_LENGTH/CONTAINERS**

Por ahora, a `date: 23/11/2025`, no se mira.

    //NumRepr/include/unlimited_length/containers/dig_string_t.hpp

**INCLUDE/VARIANTS**

Por ahora, a `date: 23/11/2025`, no se mira.

    //NumRepr/include/variants/variant_types.hpp
    //NumRepr/include/variants/digit_variant_t.hpp
    //NumRepr/include/variants/register_variant_t.hpp

**SRC**

A `date: 23/11/2025`, solo tengo que compilar `GetTableMaxExp4Base.cpp`, que tiene problemas con los `include`.

    //NumRepr/src/GetTableMaxExp4Base.cpp
    //NumRepr/src/GetTableOfPrimes.cpp
    //NumRepr/src/GetTableIsPrimeEratosthenesSieve.cpp

**TESTS**

A generar y readaptar los que se puedan, a `date: 23/11/2025`. Inhabilitar los que no se puedan para que el panorama quede claro.

Ya se han construido y pasado exitosamente test_01_math_tables y test_02_append `date: 24/11/2025` a las `hour: 18:09`. Los demás tests presentes ha pasado limpiamente el la fecha `date: 28/11/2025`.

    //NumRepr/tests/test_01_math_tables.cpp
    //NumRepr/tests/test_02_append.cpp
    //NumRepr/tests/test_03_core_internal.cpp
    //NumRepr/tests/test_04_dig_t.cpp
    //NumRepr/tests/test_05_dig_t_constructors.cpp
    //NumRepr/tests/test_06_dig_t_assignations.cpp
    //NumRepr/tests/test_07_dig_t_conversions.cpp
    //NumRepr/tests/test_08_dig_t_operadores.cpp
    //NumRepr/tests/test_09_dig_t_algebra.cpp
    //NumRepr/tests/test_10_dig_t_io.cpp

A fecha `date: 29/11/2025` y hora `hour: 18:00` se han añadido con éxito tanto en construcción (`cmake --build ...`) como en el paso de pruebas (`ctest --preset ...`)
	
	//NumRepr/tests/test_11_basic_types.cpp
	//NumRepr/tests/test_12_Int_ExpLog.cpp


**CATCH2 (INTEGRACIÓN)**

En el directorio `tests/external` tenemos lo siguientes archivos de *Catch2*:

    //NumRepr/tests/external/catch.hpp
    //NumRepr/tests/external/catch_amalgamated.hpp
    //NumRepr/tests/external/catch_amalgamated.cpp

**DOCUMENTACIÓN /DOC**

Se ha añadido el directorio `//NumRepr/doc`, dónde hay que ir documentando todo el proyecto a medida que lo vayamos testeando.

    //NumRepr/doc/dig_t_analysis.md
    //NumRepr/doc/dig_t_operadores_aritmeticos.md
    //NumRepr/doc/dig_t_operadores_comparacion.md
    //NumRepr/doc/dig_t_operadores_io.md

Aquí falta mucho por documentar incluso en la propia `dig_t.hpp`. Pero, además, no se ha documentado todo lo que hay dentro de `//NumRepr/include/core/`, esto es, `//NumRepr/include/core/internal` (aquí hay 2 archivos a documentar: `basic_types.hpp` y `conversions.hpp`),  `//NumRepr/include/core/internal/append` (aquí tenemos sin documentar `int128_ops.hpp`, `integer.hpp` y `expected.hpp`),  `//NumRepr/include/core/internal/math` sin documentar `IntExpIntLog.hpp`, `IntRoot.hpp`, `primes.hpp` y `primes_ct.hpp`, y por último el subdirectorio `//NumRepr/include/core/internal/math/tables` con los headers ``EratosthenesSieve_table.hpp`, `EratosthenesSieve.hpp` (wrapper del anterior), `MaxExp4Base_table.hpp` y `PrimeList.hpp`.
Se ha probado ya todo lo anterior.