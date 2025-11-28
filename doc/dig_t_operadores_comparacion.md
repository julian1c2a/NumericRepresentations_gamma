# dig_t<Base> - Análisis de Operadores de Comparación

## Resumen Ejecutivo

Los operadores de comparación de `dig_t<Base>` implementan una **jerarquía completa de comparaciones** que incluye operadores tradicionales de C++ junto con el moderno **operator<=>()**. El sistema utiliza comparación directa de valores internos con soporte para **C++20 three-way comparison** y comparaciones mixed-type con enteros.

## Arquitectura de Comparaciones

### 1. Operadores Tradicionales de Igualdad

#### operator== - Igualdad
```cpp
constexpr bool operator==(dig_t a) const noexcept
{
  return ((a.m_d == m_d) ? true : false);
}
```

**Análisis:**
- **Implementación**: Comparación directa de miembros `m_d`
- **Optimización**: Expresión ternaria explícita (podría ser simplificada)
- **Semántica**: Igualdad de valores en la misma base
- **Complejidad**: O(1) - comparación primitiva

#### operator!= - Desigualdad
```cpp
constexpr bool operator!=(dig_t a) const noexcept
{
  return ((a.m_d != m_d) ? true : false);
}
```

**Análisis:**
- **Implementación**: Negación lógica de igualdad
- **Consistencia**: Simétrico con `operator==`
- **Redundancia**: En C++20 se puede generar automáticamente desde `==`

### 2. Operadores Relacionales Tradicionales

#### operator>= - Mayor o igual
```cpp
constexpr bool operator>=(dig_t a) const noexcept
{
  return ((a.m_d <= m_d) ? true : false);
}
```

#### operator> - Mayor que
```cpp
constexpr bool operator>(dig_t a) const noexcept
{
  return ((a.m_d < m_d) ? true : false);
}
```

#### operator<= - Menor o igual  
```cpp
constexpr bool operator<=(dig_t a) const noexcept
{
  return ((a.m_d >= m_d) ? true : false);
}
```

#### operator< - Menor que
```cpp
constexpr bool operator<(dig_t a) const noexcept
{
  return ((a.m_d > m_d) ? true : false);
}
```

**Análisis Conjunto:**
- **Patrón**: Todos invierten la comparación `a.m_d <op> m_d`
- **Semántica**: Comparación de valores numéricos en aritmética modular
- **Consistencia**: Relaciones matemáticas correctas
- **Redundancia**: C++20 puede generar automáticamente desde `<=>`

### 3. Operador de Comparación Triádica (C++20)

#### operator<=> para dig_t (same-type)
```cpp
constexpr std::strong_ordering operator<=>(dig_t rhs) const noexcept
{
  if (m_d < rhs.m_d)
    return std::strong_ordering::less;
  else if (m_d > rhs.m_d)
    return std::strong_ordering::greater;
  else
    return std::strong_ordering::equal;
}
```

**Análisis Detallado:**
- **Tipo de retorno**: `std::strong_ordering` - orden total y estricto
- **Semántica**: Comparación completa en una sola operación
- **Implementación**: Estructura if-else-if explícita
- **Optimización**: Podría usar `std::compare_three_way{}(m_d, rhs.m_d)`
- **Equivalencia**: Genera automáticamente todos los operadores relacionales

#### operator<=> para tipos integrales (mixed-type)
```cpp
template <type_traits::signed_integral_c Int_t>
constexpr std::weak_ordering operator<=>(Int_t rhs) const noexcept
{
  const auto nrhs{normaliza<Int_t>(rhs)};
  
  if (m_d < nrhs)
    return std::weak_ordering::less;
  else if (m_d > nrhs)
    return std::weak_ordering::greater;
  else
    return std::weak_ordering::equivalent;
}
```

**Análisis Avanzado:**
- **Template**: Restringido a tipos enteros con signo via concepts
- **Tipo de retorno**: `std::weak_ordering` - permite equivalencia no-igualdad
- **Normalización**: `normaliza<Int_t>(rhs)` convierte el entero al rango [0, Base)
- **Semántica**: Comparación cross-type con conversión automática
- **Flexibilidad**: Permite comparar dig_t con cualquier tipo integral

## Funciones de Normalización

### normaliza<Int_t> - Conversión Modular
```cpp
const auto nrhs{normaliza<Int_t>(rhs)};
```

**Análisis de la función auxiliar:**
- **Propósito**: Convierte cualquier entero a su equivalente modular en base B
- **Implementación**: Probablemente `((rhs % static_cast<Int_t>(B)) + B) % B`
- **Manejo de negativos**: Garantiza resultado en rango [0, B) para enteros negativos
- **Type safety**: Template especializado por tipo de entrada

## Diferencias Entre strong_ordering y weak_ordering

### std::strong_ordering (dig_t vs dig_t)
**Características:**
- **Equivalencia implica igualdad**: `a <=> b == 0` significa `a == b`
- **Orden total**: Todos los valores son comparables
- **Substitutabilidad**: Valores equivalentes son intercambiables

### std::weak_ordering (dig_t vs integral)
**Características:**
- **Equivalencia no implica igualdad**: Valores distintos pueden ser equivalentes en mod B
- **Ejemplo**: `dig_t<10>{3} <=> 13` retorna `equivalent` (ambos ≡ 3 mod 10)
- **Flexibilidad**: Permite comparaciones cross-type meaningfuls

## Análisis de Diseño y Optimización

### 1. Redundancia de Operadores
**Problema identificado:**
En C++20, la presencia de `operator<=>` hace redundantes los operadores tradicionales:
```cpp
// Estos se pueden generar automáticamente:
operator==, operator!=, operator<, operator>, operator<=, operator>=
```

**Solución potencial:**
```cpp
// Solo necesario en C++20:
constexpr std::strong_ordering operator<=>(const dig_t&) const = default;
constexpr bool operator==(const dig_t&) const = default;
```

### 2. Optimización de Implementación
**Implementación actual:**
```cpp
constexpr std::strong_ordering operator<=>(dig_t rhs) const noexcept
{
  if (m_d < rhs.m_d) return std::strong_ordering::less;
  // ...
}
```

**Implementación optimizada:**
```cpp
constexpr std::strong_ordering operator<=>(dig_t rhs) const noexcept
{
  return std::compare_three_way{}(m_d, rhs.m_d);
}
```

### 3. Consistencia de Parámetros
**Observación**: Los operadores tradicionales reciben parámetros por valor (`dig_t a`) mientras que `operator<=>` recibe por valor también (`dig_t rhs`). Esto es consistente y eficiente para tipos pequeños.

## Casos de Uso y Ejemplos

### Comparaciones Same-Type
```cpp
dig_t<10> a{7}, b{3};

// Operadores tradicionales
bool eq = (a == b);        // false
bool lt = (a < b);         // false  
bool gt = (a > b);         // true

// Operador C++20
auto cmp = a <=> b;        // std::strong_ordering::greater
```

### Comparaciones Mixed-Type
```cpp
dig_t<10> d{7};
int valor = 17;

// Comparación con normalización automática
auto cmp = d <=> valor;    // std::weak_ordering::equivalent (17 ≡ 7 mod 10)
bool eq = (d == valor);    // Requiere conversión explícita en operadores tradicionales
```

### Aritmética Modular en Comparaciones
```cpp
dig_t<10> a{9}, b{19};
auto cmp = a <=> b;        // Compara 9 vs 9: std::strong_ordering::equal
```

## Consideraciones de Rendimiento

### Complejidades Computacionales
- **Operadores tradicionales**: O(1) - comparación primitiva
- **operator<=> same-type**: O(1) - comparación primitiva  
- **operator<=> mixed-type**: O(1) + costo de normalización
- **Normalización**: O(1) - operación módulo

### Optimizaciones del Compilador
- **constexpr**: Todas las comparaciones son evaluables en compile-time
- **noexcept**: Garantía de no-lanzamiento de excepciones
- **Inlining**: Funciones triviales altamente optimizables

## Análisis de Correctitud Matemática

### Propiedades de Orden
Para `dig_t<Base>` los operadores mantienen las propiedades matemáticas:

**Reflexividad**: `a == a` siempre true
**Antisimetría**: Si `a <= b` y `b <= a`, entonces `a == b`  
**Transitividad**: Si `a <= b` y `b <= c`, entonces `a <= c`
**Totalidad**: Para cualquier `a, b`: `a < b` o `a > b` o `a == b`

### Aritmética Modular
Las comparaciones respetan la semántica modular:
- Valores se comparan después de reducción mod B
- Comparaciones cross-type utilizan normalización apropiada
- Equivalencia modular se maneja correctamente

## Conclusiones

Los operadores de comparación de `dig_t<Base>` proporcionan:

**Fortalezas:**
- Soporte completo para C++20 three-way comparison
- Comparaciones mixed-type con normalización automática  
- Semántica matemática correcta para aritmética modular
- Optimización compile-time completa

**Áreas de optimización:**
- Redundancia de operadores tradicionales en C++20
- Implementación manual vs `std::compare_three_way`
- Posible consolidación usando `= default` para generación automática

Esta implementación demuestra un enfoque **híbrido** que mantiene compatibilidad con C++ tradicional mientras aprovecha las características modernas de C++20.