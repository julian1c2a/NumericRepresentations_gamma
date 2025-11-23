En `NumRepr/CMakeLists.txt` tengo que mis cabeceras están en `NumRepr/include/.`, mis tests están en `NumRepr/tests/.`, tengo `cpp` en `NumRepr/src` con la intención de crear tablas para las cabeceras `hpp` etc. Pídeme los archivos que te subo. No sé si debería haber incorporado el sistema de directorios y subdirectorios al sistema de `namespaces`, pero hubiesen quedado unos nombres horrorosamente largos.


**INCLUDE/CORE/INTERNAL/MATH/TABLES**

De estas cabeceras `C++ hpp` necesito pruebas (integradas en `Catch2` + `CTest`) en `NumRepr/tests`. A `date: 23/11/2025` están casi finalizadas. Falta una tabla `lookup table` que diga las parejas `{base, máximo exponente en uint64_t}` que se borró milagrosamente. 

    NumRepr/include/core/internal/math/tables/EratosthenesSieve_table.hpp
    NumRepr/include/core/internal/math/tables/EratosthenesSieve.hpp
    NumRepr/include/core/internal/math/tables/PrimeList.hpp

**INCLUDE/CORE/INTERNAL/MATH**

De estas cabeceras `C++ hpp` necesito pruebas (integradas en `Catch2` + `CTest`) en `NumRepr/tests`. A `date: 23/11/2025` están casi finalizadas. Falta una función de seguridad, tanto en tiempo de compilación como de ejecución para las funciones exponenciales, que tiene una cadena de `ifs` dependiente de la tabla de parejas `{base, máximo exponente en uint64_t}` que se borró milagrosamente. Para las demás sí que necesito las pruebas.

    NumRepr/include/core/internal/math/IntExpIntLog.hpp
    NumRepr/include/core/internal/math/IntRoot.hpp
    NumRepr/include/core/internal/math/math_utils.hpp
    NumRepr/include/core/internal/math/primes.hpp
    NumRepr/include/core/internal/math/primes_ct.hpp

**INCLUDE/CORE/INTERNAL/APPEND**

De estas cabeceras `C++ hpp` necesito pruebas (integradas en `Catch2` + `CTest`) en `NumRepr/tests`. A `date: 23/11/2025` están casi finalizadas. ¿Se podría integrar el sistema de meter la pareja de elementos de 64 bits en una pareja de registros y poner el código ensamblador para  `x86_64`, `arm64` y `risc-v` (64 bits)? No creo que merezca la pena pero como comentarios para saber el camino sí que es interesante.

    NumRepr/include/core/internal/append/int128_ops.hpp
    NumRepr/include/core/internal/append/integers.hpp
    NumRepr/include/core/internal/append/expected.hpp

**INCLUDE/CORE/INTERNAL**

De estas cabeceras `C++ hpp` necesito pruebas (integradas en `Catch2` + `CTest`) en `NumRepr/tests`. A `date: 23/11/2025` están casi finalizadas, a falta de tener los `includes` arreglados, (no lo están por la refactorización). Aquí se necesitaría una cobertura fuerte, profunda, amplia tanto en casos regulares como (mucho más) en los casos límite y distintos tipos. Necesito la recomposición de los `includes`.

    NumRepr/include/core/internal/aux_func.hpp
    NumRepr/include/core/internal/basic_types.hpp
    NumRepr/include/core/internal/conversions.hpp

**INCLUDE/CORE**

De estas cabeceras `C++ hpp` necesito pruebas (integradas en `Catch2` + `CTest`) en `NumRepr/tests`. A `date: 23/11/2025` están casi finalizadas, a falta de tener los `includes` arreglados, (no lo están por la refactorización). Aquí se necesitaría una cobertura fuerte, profunda, amplia tanto en casos regulares como (mucho más) en los casos límite y distintas bases que impliquen distintos tipos. Necesito la recomposición de los `includes`.

    NumRepr/include/core/dig_t.hpp
    NumRepr/include/core/dig_t_display_helpers.hpp

**INCLUDE/FIXED_LENGTH/CONTAINERS**

Por ahora, a `date: 23/11/2025`, no se mira.

    NumRepr/include/fixed_length/containers/reg_digs_t.hpp

**INCLUDE/FIXED_LENGTH**

Por ahora, a `date: 23/11/2025`, no se mira.

    NumRepr/include/fixed_length/nat_reg_digs_t.hpp
    NumRepr/include/fixed_length/int_reg_digs_t.hpp
    NumRepr/include/fixed_length/numeric_traits_specializations.hpp

**INCLUDE/UNLIMITED_LENGTH/CONTAINERS**

Por ahora, a `date: 23/11/2025`, no se mira.

    NumRepr/include/unlimited_length/containers/dig_string_t.hpp

**INCLUDE/VARIANTS**

Por ahora, a `date: 23/11/2025`, no se mira.

    NumRepr/include/variants/variant_types.hpp
    NumRepr/include/variants/digit_variant_t.hpp
    NumRepr/include/variants/register_variant_t.hpp

**SRC**

A `date: 23/11/2025`, solo tengo que compilar `GetTableMaxExp4Base.cpp`, que tiene problemas con los `include`.

    NumRepr/src/GetTableMaxExp4Base.cpp
    NumRepr/src/GetTableOfPrimes.cpp
    NumRepr/src/GetTableIsPrimeEratosthenesSieve.cpp

**TESTS**

A generar y readaptar los que se puedan, a `date: 23/11/2025`. Inhabilitar los que no se puedan para que el panorama quede claro.