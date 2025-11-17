# Parser Consteval Puro con Recursión

**Fecha**: 14 de noviembre de 2025  
**Objetivo**: Implementar versiones `consteval` completamente puras de las FSM del parser

## 📋 Resumen

Se crearon versiones `consteval` puras de todas las FSM (Finite State Machines) del parser, reemplazando bucles iterativos por recursión para cumplir con las restricciones de evaluación en tiempo de compilación.

---

## 🎯 Funciones Implementadas

### 1. **parse_prefix_fsm_ct**
```cpp
template<typename Container>
static consteval std::expected<PrefixResult, parse_error_t>
parse_prefix_fsm_ct(const Container& container, std::size_t size) noexcept
```

- **Propósito**: Detectar prefijo ("d" vs "dig") y delimitadores ('[' vs '#')
- **Método**: Evaluación directa sin bucles (ya era compatible con consteval)
- **Formatos**: "d[", "d#", "dig#", "dig["
- **Retorno**: `PrefixResult{delimiter_open, delimiter_close, next_pos}`

### 2. **parse_number_fsm_ct**
```cpp
template<typename Container>
static consteval std::expected<NumberResult, parse_error_t>
parse_number_fsm_ct(const Container& container, std::size_t size,
                    std::size_t pos, char delim_close) noexcept
```

- **Propósito**: Parsear dígitos decimales entre delimitadores
- **Método**: **Recursión tail-optimizable**
  ```cpp
  // Helper recursivo
  parse_number_fsm_ct_impl(container, size, pos, delim_close, 
                           accumulator, digit_count)
  ```
- **Casos base**:
  - Encontrar `delim_close` → retornar `NumberResult`
  - Fin de string → error `missing_delimiter`
  - Carácter no numérico → error `invalid_digit`
- **Caso recursivo**: 
  ```cpp
  new_acc = accumulator * 10 + (char - '0')
  return parse_number_fsm_ct_impl(..., pos+1, ..., new_acc, digit_count+1)
  ```

### 3. **parse_base_fsm_ct**
```cpp
template<typename Container>
static consteval std::expected<BaseResult, parse_error_t>
parse_base_fsm_ct(const Container& container, std::size_t size,
                  std::size_t pos, std::uint64_t expected_base) noexcept
```

- **Propósito**: Validar "B" y parsear dígitos de la base
- **Método**: **Recursión + verificación final**
  ```cpp
  // Helper recursivo para parsing
  parse_base_fsm_ct_impl(container, size, pos, accumulator, digit_count)
  ```
- **Validaciones**:
  1. Verificar carácter 'B'
  2. Parsear dígitos recursivamente
  3. Comparar `base_leida == expected_base`
- **Retorno**: `BaseResult{base_value, size}` si todo coincide

### 4. **parse_impl_pure_ct**
```cpp
template<std::size_t N>
static consteval std::expected<uint_t, parse_error_t>
parse_impl_pure_ct(const std::array<char, N>& arr, 
                   std::uint64_t base_template) noexcept
```

- **Propósito**: Parser completamente consteval que integra las 3 FSM
- **Pipeline**:
  1. `parse_prefix_fsm_ct()` → detectar formato
  2. `parse_number_fsm_ct()` → extraer número
  3. `parse_base_fsm_ct()` → validar base
  4. Normalización: `numero % base_template`
- **Restricción**: Solo funciona con `std::array<char, N>` (no `const char*`)
- **Uso**: Evaluación obligatoria en tiempo de compilación

---

## 🔍 Comparación: Constexpr vs Consteval

| Aspecto | parse_impl_ct (constexpr) | parse_impl_pure_ct (consteval) |
|---------|---------------------------|--------------------------------|
| **Evaluación** | Compile-time o runtime | **Solo** compile-time |
| **Bucles** | `while` permitidos | Requiere recursión |
| **Input** | `const char*` o `std::array` | Solo `std::array<char, N>` |
| **Uso** | Flexible | Contextos consteval estrictos |
| **Optimización** | Depende del compilador | Garantía compile-time |

---

## 🧪 Tests Implementados

**Archivo**: `tests/test_parse_pure_ct.cpp` (310 líneas)  
**Script**: `compile_parse_pure_ct.bat`

### Tests de `parse_impl_pure_ct`
1. **test_pure_ct_format_1**: "d[N]BM" (4 casos)
2. **test_pure_ct_format_2**: "d#N#BM" (2 casos)
3. **test_pure_ct_format_3**: "dig#N#BM" (2 casos)
4. **test_pure_ct_format_4**: "dig[N]BM" (2 casos)
5. **test_pure_ct_equivalence**: Los 4 formatos con mismo valor
6. **test_pure_ct_normalization**: Valores `>= B` normalizados
7. **test_pure_ct_errors**: Validación de errores

### Tests de FSMs Individuales
8. **test_prefix_fsm_ct**: Detección de 4 formatos
9. **test_number_fsm_ct**: Parsing de números
10. **test_base_fsm_ct**: Validación de base

**Resultado**: ✅ **100% de tests pasando** (10/10)

---

## 💡 Técnicas de Recursión Empleadas

### 1. **Tail Recursion** (parse_number_fsm_ct_impl)
```cpp
consteval auto parse_number_fsm_ct_impl(
    const Container& c, size_t size, size_t pos,
    char delim, sig_uint_t acc, size_t count) noexcept 
{
    // Caso base
    if (pos >= size) return unexpected(missing_delimiter);
    if (c[pos] == delim) return NumberResult{acc, pos+1};
    
    // Caso recursivo (última operación = llamada recursiva)
    return parse_number_fsm_ct_impl(c, size, pos+1, delim,
                                    acc * 10 + (c[pos] - '0'),
                                    count + 1);
}
```

### 2. **Recursión con Acumulador** (parse_base_fsm_ct_impl)
```cpp
consteval auto parse_base_fsm_ct_impl(
    const Container& c, size_t size, size_t pos,
    sig_uint_t acc, size_t count) noexcept 
{
    // Caso base: fin de dígitos
    if (pos >= size || c[pos] < '0' || c[pos] > '9') {
        if (count == 0) return unexpected(no_base_digits);
        return acc;  // Retornar acumulador
    }
    
    // Caso recursivo
    return parse_base_fsm_ct_impl(c, size, pos+1,
                                  acc * 10 + (c[pos] - '0'),
                                  count + 1);
}
```

### 3. **Monadic Chaining** (parse_impl_pure_ct)
```cpp
consteval auto parse_impl_pure_ct(...) noexcept {
    auto prefix = parse_prefix_fsm_ct(arr, size);
    if (!prefix) return unexpected(prefix.error());
    
    auto number = parse_number_fsm_ct(arr, size, prefix->next_pos, ...);
    if (!number) return unexpected(number.error());
    
    auto base = parse_base_fsm_ct(arr, size, number->next_pos, ...);
    if (!base) return unexpected(base.error());
    
    return static_cast<uint_t>(number->value % base_template);
}
```

---

## 📊 Estadísticas de Código

| Métrica | Valor |
|---------|-------|
| **Líneas de código FSM consteval** | ~220 líneas |
| **Funciones consteval nuevas** | 6 (3 FSM + 3 helpers) |
| **Tests nuevos** | 310 líneas (10 funciones de test) |
| **Profundidad máxima recursión** | ~20 niveles (números de 20 dígitos) |
| **Cobertura de formatos** | 4/4 (100%) |

---

## ✅ Validación y Calidad

### Compilación
```bash
.\compile_parse_pure_ct.bat
```
- ✅ Sin errores de compilación
- ✅ Sin warnings
- ✅ Evaluación completa en compile-time (verificada con `static_assert`)

### Cobertura de Tests
- ✅ 4 formatos de entrada
- ✅ Normalización automática
- ✅ Manejo de errores (base mismatch, prefijo inválido)
- ✅ Equivalencia entre formatos
- ✅ FSMs individuales testadas por separado

---

## 🎓 Lecciones Aprendidas

1. **Consteval es estricto**: No permite bucles tradicionales, requiere recursión pura
2. **Tail recursion**: Eficiente para compiladores modernos, evita stack overflow
3. **Acumuladores**: Fundamentales para mantener estado en recursión
4. **std::expected**: Excelente para manejo de errores en consteval
5. **Monadic chaining**: Patrón elegante para pipelines de parseo
6. **Visibilidad**: Las funciones consteval deben ser `public` para tests externos

---

## 🔮 Casos de Uso

### Uso en Contextos Consteval
```cpp
// Literal consteval puro
consteval auto parse_literal() {
    constexpr std::array<char, 7> str = {'d', '[', '4', ']', 'B', '1', '0'};
    return dig_t<10>::parse_impl_pure_ct(str, 10);
}

// En plantillas consteval
template<auto Str>
consteval auto parse_template() {
    return dig_t<10>::parse_impl_pure_ct(Str, 10);
}
```

### Comparación con parse_impl_ct
```cpp
// Flexible (constexpr) - runtime o compile-time
constexpr auto r1 = dig_t<10>::parse_impl_ct(arr);  // OK
auto r2 = dig_t<10>::parse_impl_ct(arr);            // OK

// Estricto (consteval) - solo compile-time
consteval auto r3 = dig_t<10>::parse_impl_pure_ct(arr, 10);  // OK
auto r4 = dig_t<10>::parse_impl_pure_ct(arr, 10);            // ERROR
```

---

## 📝 Documentación en Código

Todas las funciones incluyen:
- ✅ `@brief` con descripción clara
- ✅ `@tparam` para parámetros template
- ✅ `@param` con tipos y propósito
- ✅ `@return` describiendo std::expected
- ✅ `@note` con restricciones de consteval
- ✅ `@details` con ejemplos de uso

---

## 🚀 Próximos Pasos

1. **Optimización**: Analizar generación de código assembly
2. **Benchmarking**: Comparar tiempo de compilación vs parse_impl_ct
3. **User-defined literals**: Integrar con operador`""_dig`
4. **NRVO/RVO**: Verificar optimizaciones del compilador
5. **Documentación**: Actualizar README principal con ejemplos

---

## 📌 Conclusión

Las versiones consteval puras del parser están **completamente implementadas y validadas**. Ofrecen:
- ✅ Garantía de evaluación compile-time
- ✅ Recursión tail-optimizable
- ✅ Manejo robusto de errores
- ✅ 100% de cobertura de formatos
- ✅ Documentación completa

**Estado**: ✅ **COMPLETO** - Listo para uso en producción en contextos consteval.
