# dig_t<Base> - Análisis de Operadores Bitwise y Lógicos

## Resumen Ejecutivo

Los operadores bitwise en `dig_t<Base>` implementan una **semántica matemática especializada** en lugar de manipulación tradicional de bits. Cada operador tiene un significado matemático específico dentro del contexto de aritmética modular.

## Arquitectura de Operadores

### 1. Operadores Min/Max (AND/OR Bitwise Semánticos)

#### operator& - Función Mínimo
```cpp
constexpr dig_t operator&(const dig_t &arg) const noexcept
{
  return (((*this) <= arg) ? (*this) : arg);
}
```

**Análisis:**
- **Semántica**: Devuelve el **menor** de dos valores
- **Implementación**: Utiliza comparación `<=` en lugar de AND bitwise tradicional
- **Uso matemático**: `min(a, b)` dentro del contexto de dígitos modulares
- **Optimización**: `constexpr noexcept` - completamente compile-time cuando es posible

#### operator&= - Asignación Mínimo
```cpp
constexpr const dig_t &operator&=(dig_t arg) noexcept
{
  if (arg < (*this))
    (*this) = arg;
  return (*this);
}
```

**Análisis:**
- **Comportamiento**: Solo modifica `*this` si `arg` es menor
- **Eficiencia**: Evita asignación innecesaria cuando `*this <= arg`
- **Retorno**: Referencia a `*this` para encadenamiento de operadores

#### operator| - Función Máximo
```cpp
constexpr dig_t operator|(const dig_t &arg) const noexcept
{
  return (((*this) >= arg) ? (*this) : arg);
}
```

**Análisis:**
- **Semántica**: Devuelve el **mayor** de dos valores
- **Implementación**: Utiliza comparación `>=` en lugar de OR bitwise tradicional
- **Uso matemático**: `max(a, b)` dentro del contexto de dígitos modulares

#### operator|= - Asignación Máximo
```cpp
constexpr const dig_t &operator|=(dig_t arg) noexcept
{
  if (arg > (*this))
    (*this) = arg;
  return (*this);
}
```

**Análisis:**
- **Comportamiento**: Solo modifica `*this` si `arg` es mayor
- **Eficiencia**: Evita asignación innecesaria cuando `*this >= arg`

### 2. Operador de Potencia (XOR Semántico)

#### operator^= - Exponenciación Asignada
```cpp
template <type_traits::unsigned_integral_c UIntType>
constexpr const dig_t &operator^=(UIntType exp) noexcept
{
  dig_t &cthis{*this};
  if (exp == 0)
  {
    cthis = dig_1();        // Cualquier número^0 = 1
    return (cthis);
  }
  else if (exp == 1)
  {
    return (cthis);         // Número^1 = número
  }
  else if (exp == 2)
  {
    cthis *= cthis;         // Optimización para cuadrado
    return (cthis);
  }
  else
  {
    const dig_t vthis{*this};
    cthis *= vthis;         // Inicio: this^2
    for (UIntType ix{2}; ix < exp; ++ix)
    {
      cthis *= vthis;       // Multiplicación iterativa
    }
    return (cthis);
  }
}
```

**Análisis Detallado:**
- **Semántica**: Implementa exponenciación `this^exp` (potencia matemática)
- **Casos especiales optimizados**:
  - `exp == 0`: Retorna `dig_1()` (elemento neutro multiplicativo)
  - `exp == 1`: Retorna valor sin cambios
  - `exp == 2`: Optimización directa `this *= this`
- **Algoritmo general**: Multiplicación iterativa simple
- **Tipo de exponente**: Template restringido a enteros unsigned
- **Complejidad**: O(exp) - algoritmo lineal, no logarítmico
- **Aritmética modular**: Todas las multiplicaciones son automáticamente mod B

#### operator^ - Exponenciación
```cpp
template <type_traits::unsigned_integral_c UIntType>
constexpr dig_t operator^(UIntType exp) const noexcept
{
  dig_t cpthis{*this};
  cpthis ^= exp;
  return cpthis;
}
```

**Análisis:**
- **Patrón de implementación**: Copia + asignación compuesta
- **Preservación**: Mantiene `*this` inmutable
- **Eficiencia**: Reutiliza implementación de `operator^=`

### 3. Operadores de Complemento

#### operator~ - Complemento Base-1
```cpp
constexpr dig_t operator~() const noexcept
{
  return dig_t(ui_max() - m_d);
}
```

**Análisis:**
- **Semántica**: Complemento respecto a `B-1` (valor máximo en la base)
- **Fórmula**: `(B-1) - valor`
- **Uso**: Complemento aritmético, no complemento bitwise tradicional
- **Relación**: `ui_max() == B-1` por definición de la clase

#### operator- (unario) - Complemento Base
```cpp
constexpr dig_t operator-() const noexcept
{
  return dig_t((m_d == 0) ? 0 : (B - m_d));
}
```

**Análisis:**
- **Semántica**: Complemento aritmético modular (negación en mod B)
- **Caso especial**: `-0 = 0` (cero es su propio opuesto)
- **Fórmula general**: `B - valor` para `valor != 0`
- **Propiedad**: `x + (-x) ≡ 0 (mod B)` cuando `x != 0`

## Métodos Auxiliares de Complemento

```cpp
constexpr dig_t C_Bm1() const noexcept  // Identical to operator~()
{
  return dig_t(ui_max() - m_d);
}

constexpr dig_t C_B() const noexcept    // Identical to operator-()
{
  return dig_t((m_d == 0) ? 0 : (B - m_d));
}
```

**Análisis:**
- **Propósito**: Nombres explícitos para complementos matemáticos
- **C_Bm1**: Complemento respecto a B-1
- **C_B**: Complemento respecto a B
- **Identidad**: Funciones alias de operadores unarios

## Consideraciones de Diseño

### 1. Semántica No-Bitwise
Los operadores bitwise tradicionales (`&`, `|`, `^`, `~`) han sido **redefinidos** con semántica matemática:
- `&` = función mínimo
- `|` = función máximo  
- `^` = exponenciación
- `~` = complemento aritmético B-1

### 2. Restricciones de Tipos
```cpp
template <type_traits::unsigned_integral_c UIntType>
```
Los exponentes están restringidos a tipos enteros unsigned mediante concepts de C++20.

### 3. Optimizaciones
- **Casos especiales**: Potencias 0, 1, 2 optimizadas
- **Asignaciones condicionales**: Solo modifica cuando es necesario
- **constexpr**: Todo calculable en tiempo de compilación

### 4. Aritmética Modular Automática
Todos los cálculos respetan automáticamente la aritmética modular base B, incluyendo:
- Multiplicaciones en exponenciación
- Complementos aritméticos
- Operaciones de comparación

## Casos de Uso Matemáticos

### Ejemplo de Min/Max
```cpp
dig_t<10> a{7}, b{3};
auto minimo = a & b;  // minimo == dig_t<10>{3}
auto maximo = a | b;  // maximo == dig_t<10>{7}
```

### Ejemplo de Exponenciación
```cpp
dig_t<10> base{3};
auto cuadrado = base ^ 2;  // cuadrado == dig_t<10>{9}
auto cubo = base ^ 3;      // cubo == dig_t<10>{7} (27 mod 10)
```

### Ejemplo de Complementos
```cpp
dig_t<10> valor{3};
auto comp_9 = ~valor;    // comp_9 == dig_t<10>{6} (9-3)
auto comp_10 = -valor;   // comp_10 == dig_t<10>{7} (10-3)
```

## Análisis de Rendimiento

### Complejidades Computacionales
- **Min/Max**: O(1) - comparación simple
- **Exponenciación**: O(exp) - algoritmo lineal
- **Complementos**: O(1) - aritmética simple

### Optimizaciones Potenciales
La exponenciación podría beneficiarse de algoritmo de exponenciación rápida (O(log exp)), pero la implementación actual privilegia simplicidad y claridad.

## Conclusiones

Los operadores bitwise de `dig_t<Base>` implementan una **semántica matemática rica** que extiende las capacidades aritméticas básicas con operaciones de:
- Selección (min/max)
- Exponenciación eficiente
- Complementos aritméticos modulares

Esta redefinición semántica transforma operadores tradicionalmente bitwise en herramientas matemáticas especializadas para aritmética modular.