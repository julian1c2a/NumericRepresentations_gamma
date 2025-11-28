# Análisis Completo de `dig_t.hpp`

## Resumen General
`dig_t<B>` es una clase template que implementa aritmética modular para dígitos en bases arbitrarias. Es la base fundamental del sistema de representación numérica.

## 1. Arquitectura General

### Template Parameters
- `B`: Base numérica (std::uint64_t, B > 1)
- Constraint: `requires(B > 1)`

### Objetivo Principal
Wrapper para tipos enteros sin signo que implementa:
- ✅ Aritmética modular (suma/multiplicación circular con módulo B)
- ✅ Operadores sobrecargados para transparencia sintáctica
- ✅ Optimizaciones compile-time mediante metaprogramación

## 2. Type System

### Tipos Fundamentales
```cpp
using uint_t = TypeFromIntNumber_t<B>;        // Tipo base para el dígito
using sig_uint_t = sig_UInt_for_UInt_t<uint_t>;  // Tipo con signo superior
using sig_sint_t = sig_SInt_for_UInt_t<uint_t>;  // Tipo con signo para operaciones
```

### Tipos de Contenedores
```cpp
using uintspair = std::array<uint_t, 2>;      // Par de enteros
using digspair = std::array<dig_t, 2>;        // Par de dígitos
using uintspairlist = std::array<uintspair, B>; // Lista de pares por base
using uintspairtbl = std::array<uintspairlist, B>; // Tabla bidimensional
```

### Data Members
```cpp
private:
    uint_t m_d;  // Valor del dígito (0 <= m_d < B)
```

## 3. Constructores y Conversiones

### Constructor por Defecto
- Inicializa m_d = 0

### Conversiones Explícitas
- `explicit operator uint_t()`: Extrae valor interno
- `explicit operator sig_uint_t()`: Conversión con signo
- `explicit operator sig_sint_t()`: Conversión con signo completo

### Accesores
- `get() const`: Obtiene valor sin conversión explícita
- `operator()()`: Operador función para acceso

## 4. Métodos de Manipulación

### Setters Constexpr
- `set_0()`, `set_1()`: Valores especiales
- `set_Bm1()`, `set_Bm2()`: B-1 y B-2
- `set_dig(dig_t)`, `set_dig(uint_t)`: Asignación con módulo

### Factory Methods (Consteval)
- `dig_0()`, `dig_1()`: Instancias constantes
- `dig_max()`, `dig_Bm1()`: Valor máximo (B-1)
- `dig_submax()`, `dig_Bm2()`: Segundo máximo (B-2)

## 5. Propiedades Matemáticas

### Verificaciones Compile-time
- `is_prime()`: Determina si B es primo
- Optimizaciones específicas para bases primas

### Límites y Constantes
- `ui_max()`: uint_t(B-1)
- `ui_Bm1()`, `ui_Bm2()`: Constantes útiles

## Estado de Implementación

### ✅ Completado
1. **Arquitectura de tipos**: Sistema robusto de type traits
2. **Constructores básicos**: Inicialización correcta
3. **Conversiones**: Operadores explícitos funcionando
4. **Setters**: Métodos de asignación con validación
5. **Factory methods**: Generadores constexpr/consteval
6. **Verificaciones matemáticas**: is_prime() implementado

### 🔄 Por Analizar
1. **Operadores aritméticos**: +, -, *, /, %
2. **Operadores de comparación**: ==, !=, <, >, <=, >=
3. **Operadores lógicos y bit**: &, |, ^, ~, <<, >>
4. **E/O**: operator<<, operator>>
5. **Tablas de multiplicar**: Optimizaciones estáticas
6. **Algoritmos especiales**: Potenciación, raíces, etc.

## Próximos Pasos
1. Documentar operadores aritméticos
2. Crear tests para cada categoría de funcionalidad
3. Verificar optimizaciones compile-time
4. Analizar performance vs implementación naive