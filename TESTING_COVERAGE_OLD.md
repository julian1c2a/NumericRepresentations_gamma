# 📋 COBERTURA DE TESTING - dig_t<B>

## 🎯 Test Principal: `test_dig_t_comprehensive.cpp`

**Estado:** ✅ TODOS LOS TESTS PASANDO (100%)  
**Última ejecución:** 13 de noviembre de 2025  
**Bases testeadas:** B=256 (potencia de 2), B=257 (primo)  
**Total secciones:** 9 baterías de tests

---

## 📊 COBERTURA DETALLADA POR FUNCIONALIDAD

### 1️⃣ CONSTRUCTORES Y INICIALIZACIÓN
**Archivo:** `test_dig_t_comprehensive.cpp` líneas 28-55  
**Estado:** ✅ COMPLETADO

| Funcionalidad | Test realizado | Estado |
|--------------|----------------|--------|
| Constructor por defecto | `dig_t<B>()` → 0 | ✅ OK |
| Constructor desde int positivo | `dig_t<B>(42)` → 42 % B | ✅ OK |
| Constructor desde int negativo | `dig_t<B>(-5)` → normalización correcta | ✅ OK |
| Constructor copia | `dig_t d2 = d1` | ✅ OK |

**Cobertura:** 4/4 constructores principales

---

### 2️⃣ PARSER COMPILE-TIME
**Archivo:** `test_parse_ct_4formats.cpp` (146 líneas, 3 bases)  
**Estado:** ✅ **COMPLETADO** - 4/4 formatos soportados

| Formato | Descripción | Estado |
|---------|-------------|--------|
| `d[N]BM` | Prefijo corto + brackets | ✅ TESTEADO |
| `d#N#BM` | Prefijo corto + hashes | ✅ TESTEADO (NUEVO) |
| `dig#N#BM` | Prefijo largo + hashes | ✅ TESTEADO |
| `dig[N]BM` | Prefijo largo + brackets | ✅ TESTEADO (NUEVO) |

**Verificaciones realizadas:**
- ✅ Equivalencia: Los 4 formatos producen el mismo valor
- ✅ Normalización: Valores >= base se normalizan automáticamente
- ✅ Múltiples bases: Base 10, 16, 256

**Cobertura:** 4/4 formatos (100%), paridad completa con parse_impl (runtime)

---

### 3️⃣ OPERADORES ARITMÉTICOS
**Archivo:** `test_dig_t_comprehensive.cpp` líneas 107-159  
**Estado:** ✅ COMPLETADO

#### 3.1 Operadores sin asignación (+, -, *, /, %)

| Operador | Base 256 | Base 257 | Verificación |
|----------|----------|----------|--------------|
| `a + b` | 7+3=10 ✅ | 7+3=10 ✅ | Suma modular |
| `a - b` | 7-3=4 ✅ | 7-3=4 ✅ | Resta modular |
| `a * b` | 7×3=21 ✅ | 7×3=21 ✅ | Multiplicación modular |
| `a / b` | ⏭️ N/A | 7/3=88 ✅ | División modular (88×3=7 verificado) |
| `a % b` | 15%7=1 ✅ | 15%7=1 ✅ | Módulo |

**Nota:** División modular no testeada en base 256 (no todos los elementos son unidades)

#### 3.2 Operadores con asignación (+=, -=, *=, /=, %=)

| Operador | Test realizado | Estado |
|----------|----------------|--------|
| `a += b` | Incremento y verificación | ✅ OK |
| `a -= b` | Decremento y verificación | ✅ OK (implícito) |
| `a *= b` | Multiplicación y verificación | ✅ OK |
| `a /= b` | División modular verificada | ✅ OK |
| `a %= b` | Módulo verificado | ✅ OK (implícito) |

#### 3.3 Incremento/Decremento (++, --)
**Archivo:** `test_increment_decrement.cpp` (301 líneas, 6 bases)  
**Estado:** ✅ **COMPLETADO** - 4/4 operadores verificados

| Operador | Test realizado | Estado |
|----------|----------------|--------|
| `++a` (pre-incremento) | Retorna referencia, incrementa antes | ✅ OK |
| `a++` (post-incremento) | Retorna copia, incrementa después | ✅ OK |
| `--a` (pre-decremento) | Retorna referencia, decrementa antes | ✅ OK |
| `a--` (post-decremento) | Retorna copia, decrementa después | ✅ OK |

**Verificaciones especiales:**
- ✅ Wraparound overflow: `(B-1)++ → 0` (todas las bases)
- ✅ Wraparound underflow: `0-- → B-1` (todas las bases)
- ✅ Secuencias mixtas: `y=x++; z=++x;` funciona correctamente
- ✅ Ciclos completos: 0→1→...→(B-1)→0 verificado (bases pequeñas)
- ✅ Equivalencias matemáticas: `++x ≡ x+=1`, `--x ≡ x-=1`
- ✅ Referencias vs copias: Pre-operadores devuelven referencia, post devuelven copia

**Bases testeadas:** 2, 5, 10, 16, 256, 257 (incluye casos extremos)

**Cobertura:** 4/4 operadores (100%)

---

### 4️⃣ OPERADORES LÓGICOS (INTERPRETACIÓN MIN/MAX)
**Archivo:** `test_dig_t_comprehensive.cpp` líneas 164-203  
**Estado:** ✅ COMPLETADO

| Operador | Interpretación | Base 256 | Base 257 | Estado |
|----------|----------------|----------|----------|--------|
| `a & b` | min(a, b) | 5&9=5 ✅ | 5&9=5 ✅ | ✅ OK |
| `a \| b` | max(a, b) | 5\|9=9 ✅ | 5\|9=9 ✅ | ✅ OK |
| `a && b` | min(a, b) | 5&&9=5 ✅ | 5&&9=5 ✅ | ✅ OK |
| `a \|\| b` | max(a, b) | 5\|\|9=9 ✅ | 5\|\|9=9 ✅ | ✅ OK |
| `a &= b` | min assignment | Verificado ✅ | Verificado ✅ | ✅ OK |
| `a \|= b` | max assignment | Verificado ✅ | Verificado ✅ | ✅ OK |

**Cobertura:** 6/6 operadores verificados

---

### 5️⃣ EXPONENCIACIÓN BINARIA
**Archivo:** `test_dig_t_comprehensive.cpp` líneas 208-251  
**Estado:** ✅ COMPLETADO (con verificaciones avanzadas)

| Test | Base 256 | Base 257 | Verificación |
|------|----------|----------|--------------|
| `0^0 = 1` | ✅ OK | ✅ OK | Convención matemática |
| `a^0 = 1` | 3^0=1 ✅ | 3^0=1 ✅ | Exponente cero |
| `a^1 = a` | 3^1=3 ✅ | 3^1=3 ✅ | Exponente uno |
| `a^2 = a×a` | 3^2=9 ✅ | 3^2=9 ✅ | Cuadrado |
| `a^= b` | 5^3=125 ✅ | 5^3=125 ✅ | Operador con asignación |
| Teorema de Fermat | N/A | 2^256≡1 ✅ | **Verificación criptográfica** |
| Exponente grande | N/A | 3^1000=227 ✅ | Algoritmo O(log 1000)≈10 iter |

**Características verificadas:**
- ✅ Algoritmo binario O(log exp) funciona correctamente
- ✅ Fermat: 2^(p-1) ≡ 1 (mod p) para p=257 primo
- ✅ Optimización para exponentes grandes (3^1000 en ~10 iteraciones)

**Cobertura:** 7/7 casos + verificación matemática avanzada

---

### 6️⃣ OPERADORES DE COMPARACIÓN
**Archivo:** `test_dig_t_comprehensive.cpp` líneas 256-313  
**Estado:** ✅ COMPLETADO

#### 6.1 Comparación entre dig_t

| Operador | Base 256 | Base 257 | Estado |
|----------|----------|----------|--------|
| `a == b` | 5==5 ✅ | 5==5 ✅ | ✅ OK |
| `a != b` | 5!=9 ✅ | 5!=9 ✅ | ✅ OK |
| `a < b` | 5<9 ✅ | 5<9 ✅ | ✅ OK |
| `a > b` | 9>5 ✅ | 9>5 ✅ | ✅ OK |
| `a <= b` | Verificado ✅ | Verificado ✅ | ✅ OK |
| `a >= b` | Verificado ✅ | Verificado ✅ | ✅ OK |
| `a <=> b` (less) | 5<=>9 ✅ | 5<=>9 ✅ | ✅ OK |
| `a <=> b` (equal) | 5<=>5 ✅ | 5<=>5 ✅ | ✅ OK |

#### 6.2 Comparación con enteros

| Test | Base 256 | Base 257 | Verificación |
|------|----------|----------|--------------|
| `dig==int` (normalización) | 3==259 ✅ | 3==260 ✅ | Normaliza automáticamente |
| `dig<=>int` (weak_ordering) | 3<=>259 equiv ✅ | 3<=>260 equiv ✅ | Ordenamiento débil correcto |

**Características verificadas:**
- ✅ `strong_ordering` para comparaciones dig_t vs dig_t
- ✅ `weak_ordering` para comparaciones dig_t vs enteros (por normalización)

**Cobertura:** 10/10 operadores de comparación verificados

---

### 7️⃣ FUNCIONES AUXILIARES
**Archivo:** `test_dig_t_comprehensive.cpp` líneas 318-361  
**Estado:** ✅ COMPLETADO

| Función | Base 256 | Base 257 | Verificación |
|---------|----------|----------|--------------|
| `is_unit(1)` | true ✅ | true ✅ | 1 es unidad universal |
| `is_unit(0)` | false ✅ | false ✅ | 0 nunca es unidad |
| `is_unit(par)` en B=256 | is_unit(4)=false ✅ | N/A | Pares no son unidades en base par |
| `is_unit(impar)` en B=257 | N/A | is_unit(7)=true ✅ | Todo ≠0 es unidad en base prima |
| `mult_inv(a)` | inv(5)=205 ✅ | inv(7)=147 ✅ | Verificado: a×inv(a)≡1 |
| `is_Bm1(B-1)` | is_Bm1(255)=true ✅ | is_Bm1(256)=true ✅ | Detecta máximo valor |
| `is_Bm1(B-2)` | is_Bm1(254)=false ✅ | is_Bm1(255)=false ✅ | No falsos positivos |

**Verificaciones matemáticas:**
- ✅ Base 256: 5 × 205 = 1025 ≡ 1 (mod 256) ✓
- ✅ Base 257: 7 × 147 = 1029 ≡ 1 (mod 257) ✓

**Cobertura:** 7/7 funciones auxiliares verificadas

---

### 8️⃣ CASOS EDGE Y LÍMITES
**Archivo:** `test_dig_t_comprehensive.cpp` líneas 366-419  
**Estado:** ✅ COMPLETADO

| Caso Edge | Base 256 | Base 257 | Descripción |
|-----------|----------|----------|-------------|
| Overflow suma | (255+1)%256=0 ✅ | (256+1)%257=0 ✅ | Wraparound superior |
| Underflow resta | (0-1)%256=255 ✅ | (0-1)%257=256 ✅ | Wraparound inferior |
| Multiplicación por 0 | 42×0=0 ✅ | 42×0=0 ✅ | Absorción |
| Multiplicación por 1 | 42×1=42 ✅ | 42×1=42 ✅ | Neutro multiplicativo |
| Exponente 0 | 123^0=1 ✅ | 123^0=1 ✅ | Convención exponente cero |
| Propiedad Fermat | N/A | 2^8≡256≡-1 ✅ | 2^8=-1 (mod 257) |
| Propiedad Fermat | N/A | 2^16≡1 ✅ | (2^8)^2=(-1)^2=1 |

**Cobertura:** 7/7 casos edge verificados

---

### 9️⃣ BENCHMARKS DE RENDIMIENTO
**Archivo:** `test_dig_t_comprehensive.cpp` líneas 424-464  
**Estado:** ✅ COMPLETADO

#### Base 256 (potencia de 2)

| Operación | Iteraciones | Tiempo total | µs/operación | Estado |
|-----------|-------------|--------------|--------------|--------|
| Suma | 100,000 | ~1030 µs | 0.0103 µs | ✅ OK |
| Multiplicación | 1,000 | ~10 µs | 0.01 µs | ✅ OK |

#### Base 257 (primo)

| Operación | Iteraciones | Tiempo total | µs/operación | Estado |
|-----------|-------------|--------------|--------------|--------|
| Suma | 100,000 | ~1013 µs | 0.0101 µs | ✅ OK |
| Multiplicación | 1,000 | ~10 µs | 0.01 µs | ✅ OK |
| Exponenciación 2^1000 | 10,000 | ~1023 µs | 0.1023 µs | ✅ OK |

**Notas de rendimiento:**
- ✅ Suma: ~0.01 µs/op (100 millones ops/segundo)
- ✅ Multiplicación: similar a suma
- ✅ Exponenciación: ~0.1 µs/op con algoritmo O(log exp)
- ✅ Algoritmo binario: 2^1000 en solo ~10 iteraciones (teórico: log₂(1000)≈10)

---

## 📝 TESTS ADICIONALES EN EL REPOSITORIO


### Tests específicos (carpeta `tests/`)


#### `core/internal/*.hpp` - Tests integrales y scripts multi-compilador

| Archivo                                      | Funcionalidad principal                        | Estado     | Cobertura |
|-----------------------------------------------|------------------------------------------------|------------|-----------|
| `test_core_internal_comprehensive.cpp`        | Test integral de todos los headers internos     | ✅ NUEVO   | Tipos, utilidades, pack2array, funciones auxiliares |
| `test_basic_types_comprehensive.cpp`          | Test integral de basic_types.hpp y pack2array   | ✅ NUEVO   | typedefs, pack2array, for_each, casos reales |
| `test_auxiliary_functions_comprehensive.cpp`  | Test integral de auxiliary_functions.hpp        | ✅ NUEVO   | min, max, clamp, abs, sign, swap, to_string, from_string |
| `test_pack2array.cpp`                         | `pack2array`, `assign_with_order`              | ✅ Existe  | Conversión de packs, for_each, análisis de copias/movimientos, uso real |

**Descripción:**
- Se han creado tests integrales para cubrir exhaustivamente todos los headers de `/core/internal/*.hpp`.
- Los archivos `test_core_internal_comprehensive.cpp`, `test_basic_types_comprehensive.cpp` y `test_auxiliary_functions_comprehensive.cpp` usan Catch2 y están preparados para integración con CTest.
- Cada test cubre tanto casos básicos como avanzados, incluyendo typedefs, metaprogramación, utilidades auxiliares y conversión de tipos.
- Scripts batch asociados (`compile_core_internal_comprehensive.bat`, `compile_basic_types_comprehensive.bat`, `compile_auxiliary_functions_comprehensive.bat`) permiten compilar y ejecutar con MSVC, g++ o clang++ mediante argumento (`msvc`, `g++`, `clang++`).

**Cobertura:** Todas las funcionalidades principales de los headers internos están testeadas y verificadas en múltiples compiladores. No se han reportado fallos ni comportamientos inesperados.


| Archivo | Funcionalidad | Estado | Cobertura |
|---------|---------------|--------|-----------|
| `test_dig_t_comprehensive.cpp` | **Suite principal completa** | ✅ Activo | 9 secciones, 91% |
| `test_dig_t_bitwise.cpp` | **Operadores bitwise/lógicos + unarios** | ✅ Activo | ~, -, &, \|, ^, propiedades |
| `test_dig_t_basic_construct.cpp` | Constructores básicos | ✅ Existe | Constructor default, int, copia |
| `test_dig_t_arithmetic.cpp` | Operadores aritméticos | ✅ Existe | +, -, *, /, % |
| `test_dig_t_comparison.cpp` | Operadores comparación | ✅ Existe | ==, !=, <, >, <=, >=, <=> |
| `test_dig_t_io.cpp` | Entrada/salida | ✅ Existe | I/O streams |
| `test_mult_inv_optimized.cpp` | Inversión multiplicativa optimizada | ✅ Existe | mult_inv() |
| `test_fast_exponentiation.cpp` | Exponenciación binaria | ✅ Existe | Algoritmo O(log n) |
| `test_parse_formats.cpp` | **Parser runtime (4 formatos)** | ✅ **Existe (NUEVO)** | parse_impl - 4 formatos |
| `test_parse_ct_4formats.cpp` | **Parser compile-time (4 formatos)** | ✅ **Existe (NUEVO)** | parse_impl_ct - 4 formatos |
| `test_parse_pure_ct.cpp` | **Parser consteval puro (recursión)** | ✅ **Existe (NUEVO)** | parse_impl_pure_ct - 3 FSM recursivas |
| `test_to_cstr.cpp` | **Serialización constexpr** | ✅ **Existe (NUEVO)** | to_cstr() - compile-time |
| `test_boolean_queries.cpp` | **Funciones is_*** (16 funciones) | ✅ **Existe (NUEVO)** | Todas las consultas booleanas |
| `test_auxiliary_functions.cpp` | Funciones auxiliares | ✅ Existe | Varias funciones |

**Total de archivos de test:** ~50+ archivos

---

## 🔍 DETALLE DE TESTS ESPECÍFICOS

### `test_dig_t_bitwise.cpp` - Operadores Unarios y Lógicos
**Líneas:** 242  
**Bases testeadas:** 5, 10, 16, 17, 64  
**Cobertura:**

| Operador/Función | Test realizado | Verificación |
|------------------|----------------|--------------|
| `operator~` | ~5, ~0, ~(B-1) | Complemento B-1 correcto |
| `operator- unario` | -7, -0 | Complemento B correcto |
| `operator&` | 4 & 8 = min | Propiedad mínimo ✓ |
| `operator\|` | 4 \| 8 = max | Propiedad máximo ✓ |
| `operator^` | Exponenciación | Con optimizaciones exp=0,1,2 |
| **Propiedad:** x + (-x) ≡ 0 | Verificada ✅ | Inverso aditivo |
| **Propiedad:** ~~x = x | Verificada ✅ | Involución complemento |

**Tests faltantes en este archivo:**
- ⏭️ operator! (idéntico a ~)
- ⏭️ C_Bm1(), C_B() (versiones nombradas)
- ⏭️ mC_Bm1(), mC_B() (versiones con asignación)

---

### 🆕 test_boolean_queries.cpp (414 líneas)
**Cobertura:** ✅ **16/16 funciones de consulta booleanas (100%)**  
**Bases:** 2, 3, 5, 10, 16, 256, 257  
**Estado:** ✅ TODOS LOS TESTS PASANDO

#### Tests implementados:

**1. Consultas básicas (6 funciones):**
- ✅ `is_0()`, `is_1()`, `is_0or1()` + negaciones
- ✅ Casos: valores exactos (0, 1), normalización (B+1→1)

**2. Consultas de máximos (6 funciones):**
- ✅ `is_Bm1()`, `is_Bm2()`, `is_Bm1orBm2()` + negaciones
- ✅ Casos: B-1, B-2, B-3 donde aplique

**3. Consultas de extremos (2 funciones):**
- ✅ `is_maxormin()`, `is_not_maxormin()`
- ✅ Verificación: solo 0 y B-1 son extremos

**4. Consultas de proximidad (2 funciones):**
- ✅ `is_near_maxormin()`: {0, 1, B-2, B-1} (B≥4)
- ✅ `is_far_maxormin()`: [2, B-3] (B≥4)
- ✅ **Caso especial B=2:** todos near, ninguno far
- ✅ **Caso especial B=3:** todos near, ninguno far

#### Propiedades matemáticas verificadas:

| Propiedad | Verificación |
|-----------|--------------|
| `is_X()` ≡ `!is_not_X()` | ✅ Para todas las funciones |
| `is_0or1()` ≡ `(is_0() \|\| is_1())` | ✅ OK |
| `is_Bm1orBm2()` ≡ `(is_Bm1() \|\| is_Bm2())` | ✅ OK |
| `is_maxormin()` ≡ `(is_0() \|\| is_Bm1())` | ✅ OK |
| `near ∩ far = ∅` | ✅ Disjuntos |
| `near ∪ far = [0, B-1]` | ✅ Completo |

#### Tests por base:

| Base | Valores | Near | Far | Verificados |
|------|---------|------|-----|-------------|
| 2    | 2       | 2    | 0   | ✅ 2/2 |
| 3    | 3       | 3    | 0   | ✅ 3/3 |
| 5    | 5       | 4    | 1   | ✅ 5/5 |
| 10   | 10      | 4    | 6   | ✅ 10/10 |
| 16   | 16      | 4    | 12  | ✅ 16/16 |
| 256  | 256     | 4    | 252 | ✅ 256/256 |
| 257  | 257     | 4    | 253 | ✅ 257/257 |

**Cobertura:** 100% de todas las funciones is_*

---

### 🆕 test_parse_formats.cpp (220 líneas)
**Cobertura:** ✅ **4/4 formatos de parsing runtime (100%)**  
**Bases testeadas:** 2, 10, 16, 50, 256, 257  
**Estado:** ✅ TODOS LOS TESTS PASANDO

#### Formatos soportados:

**1. Formatos cortos con delimitador `[...]`:**
- ✅ `d[N]BM` - formato original con corchetes
- ✅ Ejemplo: `d[42]B50` → dig_t<50>(42)

**2. Formatos cortos con delimitador `#...#` (NUEVO):**
- ✅ `d#N#BM` - formato simétrico con almohadillas
- ✅ Ejemplo: `d#42#B50` → dig_t<50>(42)

**3. Formatos largos con delimitador `#...#`:**
- ✅ `dig#N#BM` - formato original con prefijo largo
- ✅ Ejemplo: `dig#42#B50` → dig_t<50>(42)

**4. Formatos largos con delimitador `[...]` (NUEVO):**
- ✅ `dig[N]BM` - formato simétrico con prefijo largo
- ✅ Ejemplo: `dig[42]B50` → dig_t<50>(42)

#### Verificaciones implementadas:

| Test | Verificación | Casos |
|------|--------------|-------|
| **Formato individual** | Cada formato parsea correctamente | 4 formatos × 3 casos |
| **Equivalencia** | Todos los formatos producen el mismo resultado | 5 valores testeados |
| **Normalización** | N>B se normaliza correctamente (N%B) | 4 casos en diferentes bases |
| **Rechazo base** | Rechaza si base no coincide | 4 casos (uno por formato) |
| **Múltiples bases** | Funciona en B=2,16,256,257 | 4 bases especiales |

#### Implementación técnica:

**Modificaciones a `parse_impl` en dig_t.hpp:**
- ✅ Detecta delimitadores `[` o `#` dinámicamente
- ✅ Soporta prefijos cortos (`d`) y largos (`dig`)
- ✅ Parsing unificado para todos los formatos
- ✅ Normalización delegada al constructor (evita doble módulo)

**Código limpio:** No hay debugging residual, todos los includes correctos

---

### 🆕 test_to_cstr.cpp (270 líneas)
**Cobertura:** ✅ **Serialización constexpr a string (100%)**  
**Bases testeadas:** 2, 3, 5, 10, 16, 50, 100, 256, 257, 1000  
**Estado:** ✅ TODOS LOS TESTS PASANDO

#### Funcionalidad implementada:

**`to_cstr()` - Serialización compile-time:**
```cpp
constexpr std::array<char, 32> to_cstr() const noexcept
```

- ✅ Devuelve `std::array<char, 32>` con formato "d[N]BM"
- ✅ Compatible con constexpr (evaluable en compile-time)
- ✅ Null-terminated (compatible con C-strings)
- ✅ Tamaño fijo 32 bytes (suficiente para cualquier base válida)

#### Tests implementados:

| Suite | Verificación | Casos |
|-------|--------------|-------|
| **Compile-time** | static_assert con constexpr | 3 casos verificados |
| **Runtime básico** | Serialización correcta | Base 2, 10, 16 |
| **Valores especiales** | Cero y máximos | 4 valores en bases diferentes |
| **Normalización** | N>B se serializa correctamente | 3 casos de normalización |
| **Round-trip** | to_cstr → from_cstr → valor original | 2 casos verificados |
| **Múltiples bases** | 9 bases diferentes | 2, 3, 5, 10, 16, 100, 256, 257, 1000 |

#### Características técnicas:

**Implementación en dig_t.hpp (líneas ~2834-2912):**
- ✅ Conversión uint_t → string decimal
- ✅ Conversión Base (uint64_t) → string decimal
- ✅ Construcción eficiente con buffers temporales
- ✅ Orden correcto de dígitos (inversión en lugar)
- ✅ Manejo especial de cero

**Casos límite manejados:**
- ✅ Valor 0 (no imprime vacío)
- ✅ Base grande (hasta 4 mil millones)
- ✅ Valores normalizados (serializa post-normalización)

**Uso en compile-time:**
```cpp
constexpr auto d = dig_t<10>(7);
constexpr auto str = d.to_cstr();
static_assert(str[0] == 'd');  // ✅ Verifica en compile-time
```

---

### 🆕 test_parse_pure_ct.cpp (310 líneas)
**Cobertura:** ✅ **Parsers consteval puros con recursión (100%)**  
**Bases testeadas:** 10, 16, 256  
**Estado:** ✅ TODOS LOS TESTS PASANDO

#### Funciones implementadas:

**1. FSM Consteval - parse_prefix_fsm_ct:**
```cpp
template<typename Container>
static consteval std::expected<PrefixResult, parse_error_t>
parse_prefix_fsm_ct(const Container& container, std::size_t size) noexcept
```
- ✅ Detecta 4 formatos: "d[", "d#", "dig#", "dig["
- ✅ Retorna delimitadores (open, close) y posición
- ✅ Completamente consteval (sin lambdas)

**2. FSM Consteval - parse_number_fsm_ct:**
```cpp
template<typename Container>
static consteval std::expected<NumberResult, parse_error_t>
parse_number_fsm_ct(const Container&, std::size_t pos, char delim_close) noexcept
```
- ✅ Parsea dígitos decimales con **recursión tail-call**
- ✅ Helper: `parse_number_fsm_ct_impl` (recursivo)
- ✅ Sin loops (while reemplazado por recursión)

**3. FSM Consteval - parse_base_fsm_ct:**
```cpp
template<typename Container>
static consteval std::expected<BaseResult, parse_error_t>
parse_base_fsm_ct(const Container&, std::size_t pos, uint64_t expected_base) noexcept
```
- ✅ Valida "B" + dígitos con **recursión**
- ✅ Helper: `parse_base_fsm_ct_impl` (recursivo)
- ✅ Verifica coincidencia de base

**4. Parser Integrado - parse_impl_pure_ct:**
```cpp
template<std::size_t N>
static consteval std::expected<uint_t, parse_error_t>
parse_impl_pure_ct(const std::array<char, N>& arr, uint64_t base_template) noexcept
```
- ✅ Integra las 3 FSM consteval
- ✅ Soporta los 4 formatos
- ✅ Normalización automática (value % base)
- ✅ Evaluable 100% en compile-time

#### Tests implementados:

| Suite | Verificación | Casos |
|-------|--------------|-------|
| **test_pure_ct_format_1** | "d[N]BM" | 4 valores (0, 5, 9, 15→5) |
| **test_pure_ct_format_2** | "d#N#BM" | 2 valores (7, 42→2) |
| **test_pure_ct_format_3** | "dig#N#BM" | 2 valores (10, 255→15) |
| **test_pure_ct_format_4** | "dig[N]BM" | 2 valores (100, 999→231) |
| **test_pure_ct_equivalence** | 4 formatos → mismo valor | Todos retornan 7 |
| **test_pure_ct_normalization** | N>B → N%B | 3 casos (10%10, 23%10, 100%10) |
| **test_pure_ct_errors** | Errores detectados | 3 casos (corto, base, prefijo) |
| **test_prefix_fsm_ct** | FSM prefijo | 4 formatos × delimitadores |
| **test_number_fsm_ct** | FSM número | 2 casos (42, 255) |
| **test_base_fsm_ct** | FSM base | 2 válidos + 1 mismatch |

#### Características técnicas:

**Recursión tail-call:**
- ✅ `parse_number_fsm_ct_impl`: acumulador + contador
- ✅ `parse_base_fsm_ct_impl`: acumulador + contador
- ✅ Optimizable por compilador (O(n) espacio → O(1))

**Ventajas sobre versiones anteriores:**
- ✅ **100% consteval**: Sin lambdas, sin std::is_pointer_v
- ✅ **Sin bucles**: While → recursión pura
- ✅ **Más limpio**: Separación clara FSM1, FSM2, FSM3
- ✅ **Verificable en compile-time**: Todos los tests usan static_assert

**Compilación:**
```bash
.\compile_parse_pure_ct.bat
```
- ✅ MSVC 19.44 compatible
- ✅ C++23 (/std:c++latest)
- ✅ Sin warnings

**Casos límite manejados:**
- ✅ String muy corto (< 4 chars)
- ✅ Base mismatch (B10 vs B16)
- ✅ Prefijo inválido ("x[5]B10")
- ✅ Dígitos no válidos
- ✅ Delimitador faltante

**Uso en compile-time:**
```cpp
constexpr std::array<char, 7> str = {'d', '[', '5', ']', 'B', '1', '0'};
constexpr auto result = dig_t<10>::parse_impl_pure_ct(str, 10);
static_assert(result.has_value() && *result == 5);
```

---

### 🆕 PRUEBAS DE ESTRÉS: BASES Y VALORES EXTREMOS
**Archivo:** `test_stress_make_digit.cpp` (actualizado 14/11/2025)
**Estado:** ✅ COMPLETADO

| Caso | Base | Valor(es) | Verificación |
|------|------|-----------|--------------|
| Mínima base | 2 | 0, 1, overflow | ✅ OK: valores válidos y overflow detectado |
| Máxima base | 36 | 10, 35, overflow | ✅ OK: valores válidos y overflow detectado |
| Límite base 10 | 10 | 0, 9, overflow | ✅ OK: extremos y overflow |
| String/array grandes | 16 | "F", "14" | ✅ OK: parsing correcto |

**Cobertura:**
- Construcción directa de dígitos en bases mínimas y máximas
- Validación de overflow fuera de rango
- Parsing desde string y array para valores altos
- Ejecución y aserciones exitosas en MSVC

---

## 🎯 RESUMEN DE COBERTURA

### ✅ Funcionalidades COMPLETAMENTE testeadas:
1. ✅ Constructores (4/4)
2. ✅ Operadores aritméticos simples (+, -, *, /, %) (5/5)
3. ✅ Operadores aritméticos con asignación (+=, -=, *=, /=, %=) (5/5)
4. ✅ Operadores lógicos (&, |, &&, ||) (6/6)
5. ✅ Exponenciación binaria con verificación Fermat (7/7)
6. ✅ Operadores de comparación (10/10)
7. ✅ Funciones auxiliares (7/7)
8. ✅ Casos edge (7/7)
9. ✅ Benchmarks de rendimiento (3 operaciones)
10. ✅ **Funciones de consulta booleanas (16/16) - COMPLETADO**
11. ✅ **Parser runtime from_cstr (4/4 formatos) - COMPLETADO**
12. ✅ **Serialización compile-time to_cstr() - COMPLETADO**
13. ✅ **Parser compile-time parse_impl_ct (4/4 formatos) - COMPLETADO**
14. ✅ **Parser consteval puro parse_impl_pure_ct (recursión) - COMPLETADO**

### ⚠️ Funcionalidades PARCIALMENTE testeadas:
1. ⚠️ Incremento/Decremento (0/4 operadores explícitamente)

### ⚠️ Funcionalidades PARCIALMENTE testeadas (tests separados):
1. ✅ **Operadores unarios - COMPLETAMENTE TESTEADOS en `test_dig_t_bitwise.cpp`**
   - ✅ operator~ (complemento B-1) - casos: valor normal, 0, B-1, doble comp
   - ✅ operator- unario (complemento B) - casos: valor normal, 0, propiedad x+(-x)≡0
   - ✅ operator! (complemento B-1) - **TESTEADO** - equivalente a ~
   - ✅ C_Bm1() (complemento B-1 con nombre) - **TESTEADO** - equivalente a ~
   - ✅ C_B() (complemento B con nombre) - **TESTEADO** - equivalente a - unario
   - ✅ mC_Bm1() (complemento B-1 in-place) - **TESTEADO** - retorna referencia, involución
   - ✅ mC_B() (complemento B in-place) - **TESTEADO** - retorna referencia, x+mC_B(x)≡0
   - ✅ Propiedades: !≡~, C_Bm1()≡~, C_B()≡-, ~~x=x, mC_Bm1() dos veces = identidad
2. ✅ **Funciones de consulta booleanas - COMPLETAMENTE en `test_boolean_queries.cpp` (NUEVO)**
   - ✅ **TODAS las 16 funciones is_* testeadas exhaustivamente**
   - ✅ **7 bases testeadas:** 2, 3, 5, 10, 16, 256, 257
   - ✅ **Casos especiales:** Base 2 y 3 (comportamiento diferente)
   - ✅ **Propiedades lógicas:** negaciones, equivalencias, disjunciones
   - ✅ **Casos límite:** todos los valores en [0, B-1]
   - ✅ **414 líneas de tests**, todos pasando

### ❌ Funcionalidades NO testeadas:
1. ❌ Incremento/Decremento explícitos (++, --) - solo implícitos en comprehensive

---

## 📊 ESTADÍSTICAS GLOBALES

| Categoría | Total | Testeados | Cobertura | Ubicación tests |
|-----------|-------|-----------|-----------|-----------------|
| Constructores | 4 | 4 | **100%** | comprehensive |
| Operadores aritméticos | 14 | 12 | **86%** | comprehensive |
| Operadores lógicos | 6 | 6 | **100%** | comprehensive + bitwise |
| Operadores comparación | 10 | 10 | **100%** | comprehensive |
| **Operadores unarios** | **7** | **7** | **✅ 100%** | **bitwise (COMPLETADO)** |
| **Funciones consulta bool** | **16** | **16** | **✅ 100%** | **boolean_queries (NUEVO)** |
| Funciones auxiliares | 7+ | 7 | **~90%** | comprehensive |
| **Parser runtime (from_cstr)** | **4** | **4** | **✅ 100%** | **parse_formats (NUEVO)** |
| **Parser compile-time** | **4** | **4** | **✅ 100%** | **parse_ct_4formats (NUEVO)** |
| **Parser consteval puro** | **4** | **4** | **✅ 100%** | **parse_pure_ct (NUEVO)** |
| **Serialización (to_cstr)** | **1** | **1** | **✅ 100%** | **to_cstr (NUEVO)** |
| **TOTAL GENERAL** | **77+** | **75** | **✅ ~97%** | Múltiples archivos |

---

## 📈 DETALLE POR CATEGORÍA

### Operadores Unarios (✅ 100% cobertura - COMPLETADO)
| Función | Testeada | Archivo | Notas |
|---------|----------|---------|-------|
| `operator~` | ✅ | bitwise | Casos: normal, 0, B-1, propiedad ~~x=x |
| `operator!` | ✅ | bitwise | **NUEVO** - Equivalente a ~, testeado con casos especiales |
| `operator-` unario | ✅ | bitwise | Casos: normal, 0, propiedad x+(-x)=0 |
| `C_Bm1()` | ✅ | bitwise | **NUEVO** - Versión nombrada de ~, equivalencia verificada |
| `C_B()` | ✅ | bitwise | **NUEVO** - Versión nombrada de -, equivalencia verificada |
| `mC_Bm1()` | ✅ | bitwise | **NUEVO** - In-place, retorna ref, involución verificada |
| `mC_B()` | ✅ | bitwise | **NUEVO** - In-place, retorna ref, x+mC_B(x)≡0 verificado |

**Propiedades matemáticas verificadas:**
- ✅ `operator! ≡ operator~` (complemento B-1)
- ✅ `C_Bm1() ≡ operator~` (equivalencia funcional)
- ✅ `C_B() ≡ operator- unario` (complemento B)
- ✅ `~~x = x` (involución del complemento B-1)
- ✅ `x + (-x) ≡ 0 (mod B)` para x ≠ 0
- ✅ `x + mC_B(x) ≡ 0 (mod B)` para x ≠ 0
- ✅ `mC_Bm1()` aplicado dos veces retorna al original (involución)
- ✅ `mC_Bm1()` y `mC_B()` retornan referencia al objeto modificado

**Casos especiales testeados:**
- ✅ Complemento de 0 (ambos tipos)
- ✅ Complemento de B-1
- ✅ Verificación con 5 bases diferentes: 5, 10, 16, 17, 64

### Funciones de Consulta Booleanas (✅ 100% cobertura)
**Archivo:** `test_boolean_queries.cpp` (414 líneas, 7 bases)

| Función | Testeada | Archivo | Casos testeados |
|---------|----------|---------|-----------------|
| `is_0()` | ✅ | boolean_queries | 7 bases: valores 0 y no-0 |
| `is_1()` | ✅ | boolean_queries | 7 bases: valores 1 y no-1 |
| `is_0or1()` | ✅ | boolean_queries | 7 bases: {0,1} y otros |
| `is_not_0()` | ✅ | boolean_queries | Negación verificada |
| `is_not_1()` | ✅ | boolean_queries | Negación verificada |
| `is_not_0or1()` | ✅ | boolean_queries | Negación verificada |
| `is_Bm1()` | ✅ | boolean_queries + comprehensive | B-1 true, otros false |
| `is_not_Bm1()` | ✅ | boolean_queries | Negación verificada |
| `is_Bm2()` | ✅ | boolean_queries | B-2 true, otros false |
| `is_not_Bm2()` | ✅ | boolean_queries | Negación verificada |
| `is_Bm1orBm2()` | ✅ | boolean_queries | {B-1, B-2} true, otros false |
| `is_not_Bm1orBm2()` | ✅ | boolean_queries | Negación verificada |
| `is_maxormin()` | ✅ | boolean_queries | {0, B-1} true, otros false |
| `is_not_maxormin()` | ✅ | boolean_queries | Negación verificada |
| `is_near_maxormin()` | ✅ | boolean_queries | B=2 especial, B≥4: {0,1,B-2,B-1} |
| `is_far_maxormin()` | ✅ | boolean_queries | B=2: ninguno, B≥4: [2,B-3] |

**Propiedades verificadas:**
- ✅ `is_X()` ≡ `!is_not_X()` para todas las funciones
- ✅ `is_0or1()` ≡ `(is_0() || is_1())`
- ✅ `is_Bm1orBm2()` ≡ `(is_Bm1() || is_Bm2())`
- ✅ `is_maxormin()` ≡ `(is_0() || is_Bm1())`
- ✅ Disjunción near ∩ far = ∅
- ✅ Completitud near ∪ far = [0, B-1]

**Casos especiales testeados:**
- ✅ Base 2: todos near, ninguno far
- ✅ Base 3: todos near, ninguno far
- ✅ Base ≥4: 4 valores near, B-4 valores far
- ✅ Normalización: dig(B+1) → 1

**Cobertura:** 16/16 funciones (100%), 7 bases (2,3,5,10,16,256,257)

---

## 🐛 ISSUES CONOCIDOS

### ✅ RESUELTO:
- ~~**Parser compile-time:** 2/4 formatos soportados~~ ✅ **COMPLETADO**
- ~~**Parser consteval:** Necesitaba recursión pura~~ ✅ **COMPLETADO** - parse_impl_pure_ct con 3 FSM recursivas
- ~~**Incremento/Decremento:** Tests faltantes~~ ✅ **COMPLETADO** - 4/4 operadores (test_increment_decrement.cpp)
- ~~**Operadores unarios:** Tests faltantes~~ ✅ **COMPLETADO** - 7/7 funciones (test_dig_t_bitwise.cpp)

### 🟢 COBERTURA ACTUAL:
**~99% DE FUNCIONES TESTEADAS** (76/77)

**Únicamente falta:**
- Helper functions: `num_to_string`, `radix_str` (funciones auxiliares internas usadas por `to_string`)

---

## 🎯 RECOMENDACIONES

### 🔴 Alta prioridad:
1. ~~🔴 Corregir bug en `parse_impl_ct`~~ ✅ **COMPLETADO**
2. ~~🔴 Añadir tests para funciones de consulta booleanas~~ ✅ **COMPLETADO**
   - ~~Crear `test_boolean_queries.cpp`~~ ✅ Creado (414 líneas)
   - ~~Verificar casos especiales (B=2)~~ ✅ Verificado
   - **Resultado:** 16/16 funciones (100%), 7 bases, todos los tests pasando
3. ~~🔴 Implementar parsers consteval puros~~ ✅ **COMPLETADO**
   - ~~Crear parse_prefix_fsm_ct (consteval)~~ ✅ Implementado
   - ~~Crear parse_number_fsm_ct (recursión)~~ ✅ Implementado
   - ~~Crear parse_base_fsm_ct (recursión)~~ ✅ Implementado
   - ~~Crear parse_impl_pure_ct (integrador)~~ ✅ Implementado
   - **Resultado:** 4 FSM + 310 líneas de tests, 100% passing
4. 🟡 Añadir tests para ++/-- con wraparound
5. ~~🟡 Completar tests de operadores unarios (29% → 100%)~~ ✅ **COMPLETADO**
   - ~~Añadir operator!, C_Bm1(), C_B(), mC_Bm1(), mC_B()~~ ✅ Todos testeados

### Media prioridad:
5. 🟢 Consolidar `test_dig_t_bitwise.cpp` en suite principal
6. 🟢 Crear tests de integración entre operadores
7. 🟢 Añadir tests de propiedades matemáticas adicionales

### Baja prioridad:
8. ~~⚪ Tests de parsing runtime~~ ✅ **COMPLETADO** (test_parse_formats.cpp)
9. ~~⚪ Tests de I/O y conversión a string~~ ✅ **COMPLETADO** (test_to_cstr.cpp)
10. ~~⚪ Tests de parsing compile-time~~ ✅ **COMPLETADO** (test_parse_ct_4formats.cpp + test_parse_pure_ct.cpp)
11. ⚪ Tests de rendimiento comparativo

---

## 📌 NOTAS FINALES

- **Última actualización:** 14 de noviembre de 2025 - 10:00 ✨ **test_dig_t_bitwise.cpp completado**
- **Test principal:** `test_dig_t_comprehensive.cpp` (507 líneas)
- **Test operadores unarios:** `test_dig_t_bitwise.cpp` (367 líneas) ⭐ **COMPLETADO 100%**
- **Test funciones booleanas:** `test_boolean_queries.cpp` (414 líneas)
- **Test parser runtime:** `test_parse_formats.cpp` (220 líneas)
- **Test parser compile-time:** `test_parse_ct_4formats.cpp` (146 líneas)
- **Test parser consteval puro:** `test_parse_pure_ct.cpp` (310 líneas) ⭐ **NUEVO**
- **Test serialización:** `test_to_cstr.cpp` (270 líneas)
- **Resultado general:** ✅ TODOS LOS TESTS EXISTENTES PASANDO
- **Cobertura real:** ✅ **~97%** (75 de 77+ funcionalidades) - **↑6% desde última medición**
- **Compiladores verificados:** MSVC 19.44, Clang (múltiples versiones)
- **Estándar:** C++23 (`/std:c++latest`)

### 🎯 Logros recientes:
- ✅ **Operadores unarios completados**: 5 funciones nuevas testeadas (7/7 = 100%)
- ✅ **Propiedades matemáticas**: 8 propiedades verificadas
- ✅ **Casos especiales**: 0, B-1, involución, equivalencias
- ✅ **125 líneas añadidas** a test_dig_t_bitwise.cpp
- ✅ **5 bases testeadas**: 5, 10, 16, 17, 64


#### **14 nov 2025 - 12:30** - Operadores de incremento/decremento (++/--)
✅ **Cobertura consolidada y revisada**
- Archivo: `test_increment_decrement.cpp` (301 líneas, 6 bases)
- Operadores testeados: `operator++`, `operator--`, pre y post (4/4)
- Casos especiales: wraparound, secuencias mixtas, ciclos, equivalencias matemáticas
- Bases: 2, 5, 10, 16, 256, 257
- Compilación: `compile_increment_decrement.bat`

**Estado:** Cobertura 100% y documentación actualizada. No hay notas pendientes ni advertencias sobre estos operadores.

---

### 🗂️ Estado de revisión de headers internos

**A 14 de noviembre de 2025:**
- Todo el contenido de `/core/internal/*.hpp` ha sido **revisado y validado** en la rama `revisada`.
- Todas las funcionalidades cubiertas por estos headers están correctamente documentadas y testeadas.

---

---

## 🚀 PRÓXIMOS PASOS

1. [x] ~~Corregir parser compile-time~~ ✅ **COMPLETADO - 4/4 formatos**
2. [x] ~~CREAR `test_boolean_queries.cpp`~~ ✅ **COMPLETADO - 100% de funciones is_***
3. [x] ~~CREAR `test_parse_formats.cpp`~~ ✅ **COMPLETADO - Parser runtime 4/4 formatos**
4. [x] ~~CREAR `test_to_cstr.cpp`~~ ✅ **COMPLETADO - Serialización 100%**
5. [x] ~~CREAR `test_parse_ct_4formats.cpp`~~ ✅ **COMPLETADO - Parser compile-time 4/4 formatos**
6. [x] ~~CREAR `test_parse_pure_ct.cpp`~~ ✅ **COMPLETADO - Parser consteval puro con recursión**
7. [x] ~~**COMPLETAR `test_dig_t_bitwise.cpp`**~~ ✅ **COMPLETADO - 7/7 operadores unarios (100%)**
   - [x] ~~Añadir operator!~~ ✅
   - [x] ~~Añadir C_Bm1(), C_B()~~ ✅
   - [x] ~~Añadir mC_Bm1(), mC_B()~~ ✅
8. [x] ~~Añadir tests explícitos para ++/--~~ ✅ **COMPLETADO - 4/4 operadores (100%)**
9. [ ] Consolidar documentación de todos los tests auxiliares
10. [ ] Añadir tests de integración con `nat_reg_digs_t`

---

## 📋 CHECKLIST DE FUNCIONES RECIÉN DOCUMENTADAS

### Operadores de Incremento/Decremento (4 funciones):
- [x] Documentadas ✅
- [x] operator++() pre - ✅ **TESTEADO (increment_decrement) - NUEVO**
- [x] operator++(int) post - ✅ **TESTEADO (increment_decrement) - NUEVO**
- [x] operator--() pre - ✅ **TESTEADO (increment_decrement) - NUEVO**
- [x] operator--(int) post - ✅ **TESTEADO (increment_decrement) - NUEVO**

### Operadores Unarios (7 funciones):
- [x] Documentadas ✅
- [x] operator~ - ✅ **TESTEADO (bitwise)**
- [x] operator! - ✅ **TESTEADO (bitwise) - NUEVO**
- [x] operator- unario - ✅ **TESTEADO (bitwise)**
- [x] C_Bm1() - ✅ **TESTEADO (bitwise) - NUEVO**
- [x] C_B() - ✅ **TESTEADO (bitwise) - NUEVO**
- [x] mC_Bm1() - ✅ **TESTEADO (bitwise) - NUEVO**
- [x] mC_B() - ✅ **TESTEADO (bitwise) - NUEVO**

### Funciones de Consulta Booleanas (16 funciones):
- [x] Documentadas ✅
- [x] is_0() - ✅ **TESTEADO (boolean_queries)**
- [x] is_1() - ✅ **TESTEADO (boolean_queries)**
- [x] is_0or1() - ✅ **TESTEADO (boolean_queries)**
- [x] is_not_0() - ✅ **TESTEADO (boolean_queries)**
- [x] is_not_1() - ✅ **TESTEADO (boolean_queries)**
- [x] is_not_0or1() - ✅ **TESTEADO (boolean_queries)**
- [x] is_Bm1() - ✅ **TESTEADO (boolean_queries + comprehensive)**
- [x] is_not_Bm1() - ✅ **TESTEADO (boolean_queries)**
- [x] is_Bm2() - ✅ **TESTEADO (boolean_queries)**
- [x] is_not_Bm2() - ✅ **TESTEADO (boolean_queries)**
- [x] is_Bm1orBm2() - ✅ **TESTEADO (boolean_queries)**
- [x] is_not_Bm1orBm2() - ✅ **TESTEADO (boolean_queries)**
- [x] is_maxormin() - ✅ **TESTEADO (boolean_queries)**
- [x] is_not_maxormin() - ✅ **TESTEADO (boolean_queries)**
- [x] is_near_maxormin() - ✅ **TESTEADO (boolean_queries)**
- [x] is_far_maxormin() - ✅ **TESTEADO (boolean_queries)**

**Total completado:** 23/23 funciones testeadas  
**Cobertura global:** ✅ **~97%** (75/77 funcionalidades)

**Solo faltan:** Incremento/Decremento explícito (++, --)
