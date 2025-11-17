/**
 * @file analisis_factory_methods_teorico.md
 * @brief FASE 2: Análisis Teórico Completo de Factory Methods para int_reg_digs_t
 * 
 * Análisis detallado del código fuente de int_reg_digs_t.hpp líneas 108-210
 * sin necesidad de compilación exitosa.
 */

# FASE 2: FACTORY METHODS - ANÁLISIS TEÓRICO COMPLETO

## 1. CONFIGURACIÓN TEMPLATE: int_reg_digs_t<2,3>

### Parámetros de Template:
- **B = 2** (Base binaria)
- **R = 3** (Dígitos de magnitud)
- **L = R + 1 = 4** (Dígitos totales incluido signo)

### Herencia:
```cpp
int_reg_digs_t<2,3> : public nat_reg_digs_t<2,4>
```

### Representación de Complemento a la Base:
- **Rango**: -8 a +7 (16 valores total)
- **Base^L = 2^4 = 16** (Complemento a 16)
- **Representación**:
  * Positivos: `0000₂` a `0111₂` (0 a +7)
  * Negativos: `1000₂` a `1111₂` (-8 a -1)

## 2. FACTORY METHODS FUNDAMENTALES

### 2.1. sregd_0() - Cero con Signo
```cpp
inline static consteval int_reg_digs_t sregd_0() noexcept
{
  return int_reg_digs_t{};  // Constructor por defecto = 0000₂
}
```
- **Valor**: +0
- **Representación**: `0000₂` 
- **Análisis**: Usa constructor por defecto, inicializa todos los dígitos a 0

### 2.2. sregd_1() - Uno Positivo
```cpp
inline static consteval int_reg_digs_t sregd_1() noexcept
{
  return int_reg_digs_t{regd_1()};  // Usar factory de clase base
}
```
- **Valor**: +1
- **Representación**: `0001₂`
- **Análisis**: Delega al factory method `regd_1()` de `nat_reg_digs_t<2,4>`

### 2.3. sregd_m1() - Menos Uno
```cpp
inline static consteval int_reg_digs_t sregd_m1() noexcept
{
  int_reg_digs_t result;
  result.fill(dig_Bm1());  // Llenar con B-1 = 1₂
  return result;
}
```
- **Valor**: -1
- **Representación**: `1111₂`
- **Cálculo Complemento**: 16 - 1 = 15 = `1111₂`
- **Análisis**: Llena todos los dígitos con `dig_Bm1()` (valor 1 en binario)

## 3. FACTORY METHODS AVANZADOS

### 3.1. sregd_Bm1() - Base Menos Uno (B-1)
```cpp
inline static consteval int_reg_digs_t sregd_Bm1() noexcept
{
  int_reg_digs_t ret{regd_0()};
  ret[0] = dig_Bm1();  // Primer dígito = B-1 = 1₂
  return ret;
}
```
- **Valor**: +1 (mismo que B-1 en base 2)
- **Representación**: `0001₂`
- **Análisis**: Inicializa en 0, luego establece dígito menos significativo a B-1

### 3.2. sregd_mBp1() - Menos Base Más Uno (-B+1)
```cpp
inline static consteval int_reg_digs_t sregd_mBp1() noexcept
{
  using base_Rm1_t = base_N_t<R - 1>;  // base_N_t<2>
  using base_1_t = base_N_t<1>;        // base_N_t<1>  
  using base_R_t = base_N_t<R>;        // base_N_t<3>
  base_R_t result{base_Rm1_t::regd_0().cat(base_1_t::regd_1())};
  return {result.cat(base_1_t::regd_1())};
}
```
- **Valor**: -2+1 = -1 (en int_reg_digs_t<2,3>)
- **Análisis**: Construcción compleja usando concatenación de tipos base menores

### 3.3. sregd_pow_n_B<n>() - Potencias de Base
```cpp
template <std::size_t n>
  requires((n >= 0) && (n < L - 1))  // n ∈ [0,2] para L=4
inline static consteval int_reg_digs_t sregd_pow_n_B() noexcept
{
  int_reg_digs_t ret{regd_0()};
  ret[n] = dig_1();  // Establecer bit n a 1
  return ret;
}
```
- **Valor**: +2^n
- **Casos**:
  * `sregd_pow_n_B<0>()`: +1 → `0001₂`
  * `sregd_pow_n_B<1>()`: +2 → `0010₂` 
  * `sregd_pow_n_B<2>()`: +4 → `0100₂`
- **Análisis**: Crea potencias positivas de la base

### 3.4. sregd_m_pow_n_B<n>() - Menos Potencias de Base
```cpp
template <std::size_t n>
  requires((n >= 0) && (n < L - 1))
inline static consteval int_reg_digs_t sregd_m_pow_n_B() noexcept
{
  int_reg_digs_t ret{regd_0()};
  ret[n] = dig_1();
  ret[L - 1] = dig_Bm1();  // Establecer bit de signo
  return ret;
}
```
- **Valor**: -2^n
- **Casos**:
  * `sregd_m_pow_n_B<0>()`: -1 → `1001₂` (16-1 = 15 = `1111₂` ❌)
  * `sregd_m_pow_n_B<1>()`: -2 → `1010₂` (16-2 = 14 = `1110₂` ❌)
  * `sregd_m_pow_n_B<2>()`: -4 → `1100₂` (16-4 = 12 = `1100₂` ✓)

**⚠️ ANÁLISIS CRÍTICO**: Esta implementación parece incorrecta para complemento a la base puro.

## 4. CONCEPTOS CLAVE DE COMPLEMENTO A LA BASE

### 4.1. Definición Matemática
Para un número negativo `-x` en complemento a la base:
```
Representación(-x) = B^L - x
```

### 4.2. Tabla de Valores para int_reg_digs_t<2,3>
| Valor | Binario | Decimal | Cálculo Complemento |
| ----- | ------- | ------- | ------------------- |
| +7    | 0111₂   | 7       | N/A                 |
| +6    | 0110₂   | 6       | N/A                 |
| +5    | 0101₂   | 5       | N/A                 |
| +4    | 0100₂   | 4       | N/A                 |
| +3    | 0011₂   | 3       | N/A                 |
| +2    | 0010₂   | 2       | N/A                 |
| +1    | 0001₂   | 1       | N/A                 |
| +0    | 0000₂   | 0       | N/A                 |
| -1    | 1111₂   | 15      | 16 - 1 = 15         |
| -2    | 1110₂   | 14      | 16 - 2 = 14         |
| -3    | 1101₂   | 13      | 16 - 3 = 13         |
| -4    | 1100₂   | 12      | 16 - 4 = 12         |
| -5    | 1011₂   | 11      | 16 - 5 = 11         |
| -6    | 1010₂   | 10      | 16 - 6 = 10         |
| -7    | 1001₂   | 9       | 16 - 7 = 9          |
| -8    | 1000₂   | 8       | 16 - 8 = 8          |

## 5. ANÁLISIS DE CORRECTNESS

### 5.1. Factory Methods Correctos ✅
- **sregd_0()**: `0000₂` = +0 ✅
- **sregd_1()**: `0001₂` = +1 ✅
- **sregd_m1()**: `1111₂` = -1 ✅ (16-1=15=`1111₂`)

### 5.2. Factory Methods Cuestionables ❓
- **sregd_mBp1()**: Implementación compleja, requiere verificación
- **sregd_m_pow_n_B<n>()**: Parece usar lógica de bit de signo en lugar de complemento puro

### 5.3. Patrón de Implementación
La clase usa una estrategia **híbrida**:
1. **Factory methods básicos**: Complemento a la base puro
2. **Factory methods avanzados**: Posible mezcla de bit de signo + magnitud

## 6. HERENCIA Y REUTILIZACIÓN

### 6.1. Métodos Heredados de nat_reg_digs_t<2,4>
```cpp
// Disponibles a través de herencia:
- regd_0()     // Cero natural
- regd_1()     // Uno natural  
- regd_Bm1()   // Base menos uno natural
- operator[]   // Acceso a dígitos
- fill()       // Llenar con valor
```

### 6.2. Extensiones para Números con Signo
```cpp
// Nuevos en int_reg_digs_t:
- sregd_*()    // Versiones con signo
- Template factories con restricciones
- Métodos específicos para negativos
```

## 7. CARACTERÍSTICAS TÉCNICAS

### 7.1. Uso de consteval
- **Propósito**: Evaluación en tiempo de compilación
- **Beneficio**: Zero runtime overhead
- **Restricción**: Todos los valores deben ser constantes de compilación

### 7.2. Template Requirements
```cpp
template <std::size_t n>
  requires((n >= 0) && (n < L - 1))
```
- **Validación**: Índices válidos en tiempo de compilación
- **Rango**: n ∈ [0, L-2] = [0, 2] para int_reg_digs_t<2,3>

### 7.3. Uso de using Aliases
```cpp
using base_Rm1_t = base_N_t<R - 1>;
using base_1_t = base_N_t<1>;
using base_R_t = base_N_t<R>;
```
- **Propósito**: Construcción incremental usando tipos de menor tamaño
- **Patrón**: Concatenación `.cat()` para construir tipos mayores

## 8. CONCLUSIONES FASE 2

### 8.1. Fortalezas ✅
1. **Diseño coherente**: Factory methods siguen nomenclatura clara
2. **Performance**: `consteval` garantiza evaluación en compilación
3. **Type safety**: Template requirements validan parámetros
4. **Herencia efectiva**: Reutiliza funcionalidad de nat_reg_digs_t

### 8.2. Áreas de Atención ⚠️
1. **Consistencia**: Mezcla de complemento a la base con bit de signo
2. **Complejidad**: Algunos factory methods son muy elaborados
3. **Documentación**: Faltan comentarios explicativos en código
4. **Testing**: Requiere pruebas exhaustivas para validar correctness

### 8.3. Verificación Teórica ✅
- **sregd_0()**: Implementación correcta para cero
- **sregd_1()**: Delegación apropiada a clase base  
- **sregd_m1()**: Implementación correcta de complemento a la base para -1
- **Template factories**: Diseño sólido con validación de tipos

## 9. TRANSICIÓN A FASE 3

### 9.1. Preparación para Constructores
- Factory methods proporcionan base sólida para constructores específicos
- Patrones de inicialización establecidos
- Herencia de nat_reg_digs_t validada

### 9.2. Próximos Pasos
1. **Fase 3**: Análisis de constructores específicos
2. **Verificación**: Pruebas de factory methods (cuando compilación funcione)
3. **Validación**: Comportamiento de complemento a la base

---

## RESUMEN EJECUTIVO FASE 2 ✅

**COMPLETADA**: Análisis teórico completo de factory methods para int_reg_digs_t<2,3>

**DESCUBRIMIENTOS CLAVE**:
- 10+ factory methods estáticos con diferentes patrones de inicialización
- Implementación correcta de complemento a la base para casos básicos
- Uso avanzado de templates con `consteval` y `requires`
- Herencia efectiva de nat_reg_digs_t<2,4>

**VALIDACIÓN TEÓRICA**:
- ✅ sregd_0(): Cero → `0000₂`
- ✅ sregd_1(): Uno → `0001₂`  
- ✅ sregd_m1(): Menos uno → `1111₂`
- ❓ Factory methods avanzados requieren validación empírica

**PRÓXIMO OBJETIVO**: Fase 3 - Constructores específicos