# Análisis de Operadores Aritméticos - dig_t.hpp

## Resumen de Operadores Implementados

### 1. **Operadores de Asignación Compuesta**

#### Suma (+=)
- `operator+=(dig_t arg)`: Suma modular entre dígitos
- `operator+=(Int_t arg)`: Suma con tipos integrales (template)

**Algoritmo Optimizado por Tamaño de Base:**
```cpp
if constexpr (B < middle_max<uint_t>()) {
    // Optimización para bases pequeñas - sin overflow
    m_d += arg.m_d;
    if (m_d >= B) m_d -= B;
} else {
    // Para bases grandes - usa tipo con signo superior
    sig_uint_t tmp{m_d};
    tmp += arg.m_d;
    if (tmp >= B) tmp -= B;
    m_d = static_cast<uint_t>(tmp);
}
```

#### Resta (-=)
- `operator-=(dig_t arg)`: Resta modular con manejo de underflow
- `operator-=(Int_t arg)`: Resta con tipos integrales

#### Multiplicación (*=)
- `operator*=(dig_t arg)`: Multiplicación modular
- `operator*=(Int_t arg)`: Multiplicación con tipos integrales

**Optimización por Tamaño de Base:**
```cpp
if constexpr (B < sqrt_max<uint_t>()) {
    // Base pequeña - no hay riesgo de overflow
    m_d *= arg.m_d;
    m_d %= B;
} else {
    // Base grande - usa tipo superior para evitar overflow
    sig_uint_t tmp{m_d};
    tmp *= arg.m_d;
    tmp %= static_cast<sig_uint_t>(B);
    m_d = static_cast<uint_t>(tmp);
}
```

#### División (/=) y Módulo (%=)
- `operator/=(dig_t arg)`: División entera con verificación de división por cero
- `operator/=(Int_t arg)`: División con tipos integrales
- `operator%=(dig_t arg)`: Módulo
- `operator%=(Int_t arg)`: Módulo con tipos integrales

### 2. **Operadores Binarios**
Implementados como wrappers sobre los operadores de asignación compuesta:

```cpp
constexpr dig_t operator+(dig_t arg) const noexcept {
    dig_t ret(*this);
    ret += arg;
    return ret;
}
```

- `operator+(dig_t/Int_type)`: Suma binaria
- `operator-(dig_t/Int_type)`: Resta binaria  
- `operator*(dig_t/Int_type)`: Multiplicación binaria
- `operator/(dig_t/Int_type)`: División binaria

### 3. **Operadores Unarios**

#### Incremento/Decremento
- `operator++()`: Pre-incremento (++x)
- `operator++(int)`: Post-incremento (x++)
- `operator--()`: Pre-decremento (--x)
- `operator--(int)`: Post-decremento (x--)

#### Negación Aritmética
- `operator-()`: Negación modular
```cpp
constexpr dig_t operator-() const noexcept {
    return dig_t((m_d == 0) ? 0 : (B - m_d));
}
```

### 4. **Características de Diseño**

#### Optimizaciones Compile-Time
1. **Detección de Tamaño de Base**: 
   - `B < middle_max<uint_t>()`: Operaciones directas sin overflow
   - `B >= middle_max<uint_t>()`: Usa tipos superiores

2. **Multiplicación Segura**:
   - `B < sqrt_max<uint_t>()`: Multiplicación directa
   - `B >= sqrt_max<uint_t>()`: Prevención de overflow

#### Normalización Automática
- Función `normaliza<Int_t>()` convierte cualquier entero al rango [0, B-1]
- Manejo de tipos con y sin signo
- Reducción modular automática

#### Aritmética Modular Correcta
- Todas las operaciones mantienen invariante: `0 <= resultado < B`
- Manejo correcto de underflow en resta
- Multiplicación sin pérdida de precisión

### 5. **Garantías de Rendimiento**
- **constexpr**: Todas las operaciones evaluables en compile-time
- **noexcept**: Garantía de no-excepción
- **Optimización por template**: Código específico para cada base B
- **Detección automática**: Algoritmos óptimos según tamaño de base

### 6. **Casos Especiales**
- **División por cero**: Comportamiento definido por implementación
- **Overflow**: Imposible por diseño (aritmética modular)
- **Underflow**: Manejado correctamente en resta (wrap-around modular)