# FASE 3: CONSTRUCTORES ESPECÍFICOS - int_reg_digs_t

## Análisis Teórico Completo de Constructores

### Configuración Base: int_reg_digs_t<2,3>
- **B = 2** (Base binaria)  
- **R = 3** (Dígitos magnitud)
- **L = 4** (Total dígitos con signo)
- **Herencia**: `nat_reg_digs_t<2,4>`
- **Rango**: -8 a +7 (complemento a 16)

---

## 1. CONSTRUCTOR POR DEFECTO

### Implementación:
```cpp
consteval inline int_reg_digs_t() noexcept : base_t{}
{
  for (size_t i = 0; i < L; ++i)
    (*this)[i] = dig_0();
}
```

### Análisis:
- **Especificador**: `consteval` → Evaluación obligatoria en compilación
- **Inicialización**: Manual de todos los dígitos a `dig_0()`
- **Resultado**: `0000₂` = +0
- **Herencia**: Llama constructor base `nat_reg_digs_t<2,4>{}`
- **Garantía**: Siempre produce cero positivo válido

### Uso:
```cpp
int_reg_digs_t<2,3> num;  // → 0000₂ = +0
```

---

## 2. CONSTRUCTOR DESDE LISTA DE INICIALIZACIÓN

### Implementación:
```cpp
constexpr inline int_reg_digs_t(const std::initializer_list<dig_t> &arg) noexcept
    : base_t{arg} {}
```

### Análisis:
- **Delegación**: Completa al constructor base
- **Sin normalización**: NO aplica `normalize_sign()`
- **Flexibilidad**: Acepta cualquier cantidad de dígitos
- **Riesgo**: Puede producir representaciones inconsistentes

### Uso:
```cpp
// Correcto:
int_reg_digs_t<2,3> num{dig_1(), dig_0(), dig_1(), dig_0()};  // 0101₂

// Potencialmente problemático (sin normalize_sign):
int_reg_digs_t<2,3> neg{dig_1(), dig_1(), dig_1(), dig_1()};  // ¿1111₂?
```

---

## 3. CONSTRUCTOR VARIÁDICO DESDE DÍGITOS

### Implementación:
```cpp
template <typename... Ts>
  requires(std::is_same_v<Ts, dig_t> && ...)
constexpr inline int_reg_digs_t(const Ts &...args) noexcept
    : base_t{(utilities::ugly_pack_details::pack2array<Ts...>{})(args...)}
{
  normalize_sign();
}
```

### Análisis:
- **Type Safety**: `requires` garantiza que todos los argumentos sean `dig_t`
- **Conversión**: Usa `pack2array` para expandir parameter pack
- **Normalización**: SÍ aplica `normalize_sign()` ✅
- **Flexibilidad**: Acepta cantidad variable de argumentos

### Uso:
```cpp
auto d0 = dig_0(), d1 = dig_1();
int_reg_digs_t<2,3> num(d1, d0, d1, d0);  // Con normalize_sign()
```

---

## 4. CONSTRUCTOR DESDE nat_reg_digs_t (COPIA)

### Implementación:
```cpp
template <std::size_t N>
  requires(N > 0)
constexpr inline int_reg_digs_t(const base_N_t<N> &arg) noexcept
    : base_t{copy_arg_N<N>(arg)} {}
```

### Función Auxiliar `copy_arg_N<N>()`:
```cpp
void copy_arg_N(const base_N_t<N> &arg)
{
  if constexpr (N < L)  // Arg más pequeño
  {
    for (std::size_t ix{N}; ix < L; ++ix)
      cthis[ix] = dig_0();        // Rellena con ceros
    for (std::size_t ix{0}; ix < N; ++ix)
      cthis[ix] = arg[ix];        // Copia disponibles
    normalize_sign();             // Normaliza signo
  }
  else  // N >= L, arg igual o mayor
  {
    for (std::size_t ix{0}; ix < L; ++ix)
      cthis[ix] = arg[ix];        // Copia primeros L
    normalize_sign();             // Normaliza signo
  }
}
```

### Análisis:
- **Conversión Universal**: Desde cualquier `nat_reg_digs_t<B,N>`
- **Manejo Inteligente**: Adapta según tamaño relativo
- **Normalización**: SIEMPRE aplica `normalize_sign()` ✅
- **Truncamiento**: Si N > L, toma solo primeros L dígitos

### Casos de Uso:
```cpp
nat_reg_digs_t<2,3> nat_num = nat_reg_digs_t<2,3>::regd_1();
int_reg_digs_t<2,3> int_num(nat_num);  // Conversión automática

nat_reg_digs_t<2,6> big_nat;  // 6 dígitos
int_reg_digs_t<2,3> small_int(big_nat);  // Toma primeros 4
```

---

## 5. CONSTRUCTOR DE MOVIMIENTO DESDE nat_reg_digs_t

### Implementación:
```cpp
template <std::size_t N>
  requires(N > 0)
constexpr inline int_reg_digs_t(base_N_t<N> &&arg) noexcept
    : base_t{} 
{
  move_arg_N<N>(std::move(arg));
}
```

### Función Auxiliar `move_arg_N<N>()`:
```cpp
void move_arg_N(base_N_t<N> &&arg)
{
  if constexpr (N < L)
  {
    for (std::size_t ix{N}; ix < L; ++ix)
      cthis[ix] = std::move(dig_0());     // Mueve ceros
    for (std::size_t ix{0}; ix < N; ++ix)
      cthis[ix] = std::move(arg[ix]);     // Mueve disponibles
    normalize_sign();
  }
  else if constexpr (N > L)
  {
    for (std::size_t ix{0}; ix < L; ++ix)
      cthis[ix] = std::move(arg[ix]);     // Mueve primeros L
    normalize_sign();
  }
  else  // N == L
  {
    static_cast<base_t>(cthis) = std::move(arg);  // Movimiento completo
    normalize_sign();
  }
}
```

### Análisis:
- **Optimización**: Evita copias usando `std::move`
- **Casos Específicos**: Manejo diferenciado por tamaño
- **Move Perfecto**: Para N == L usa movimiento completo
- **Normalización**: Siempre garantizada ✅

---

## 6. CONSTRUCTOR DESDE TIPOS INTEGRALES

### Implementación:
```cpp
template <type_traits::integral_c... Ints_type>
  requires((sizeof...(Ints_type)) > 1)
constexpr inline int_reg_digs_t(Ints_type... dig_pow_i) noexcept
    : base_t(dig_pow_i...)
{
  normalize_sign();
}
```

### Análisis:
- **Flexibilidad**: Acepta `int`, `long`, `unsigned`, etc.
- **Restricción**: Mínimo 2 argumentos
- **Conversión Automática**: Los tipos integrales se convierten a `dig_t`
- **Normalización**: Aplicada tras construcción ✅

### Uso:
```cpp
int_reg_digs_t<2,3> num(1, 0, 1, 0);        // int → dig_t
int_reg_digs_t<2,3> mix(1u, 0L, 1LL, 0);    // Tipos mixtos
```

---

## 7. CONSTRUCTORES ENTRE int_reg_digs_t DIFERENTES

### Implementación:
```cpp
// Copia
template <std::size_t N>
  requires(N > 1)
constexpr inline int_reg_digs_t(const int_reg_N_digs_t<N> &arg) noexcept
    : base_t{copy_arg_N<N>(arg)} {}

// Movimiento  
template <std::size_t N>
  requires(N > 1)
constexpr inline int_reg_digs_t(int_reg_N_digs_t<N> &&arg) noexcept
    : base_t{std::move(move_arg_N(arg))} {}
```

### Análisis:
- **Interconversión**: Entre diferentes configuraciones `int_reg_digs_t<B,R>`
- **Reutilización**: Usa mismas funciones `copy_arg_N` / `move_arg_N`
- **Restricción**: N > 1 (no acepta tipos de 1 dígito)
- **Casos**:
  * `int_reg_digs_t<2,3>` ← `int_reg_digs_t<2,5>` (reducción)
  * `int_reg_digs_t<2,3>` ← `int_reg_digs_t<16,2>` (cambio base)

---

## 8. SISTEMA DE NORMALIZACIÓN DE SIGNO

### Funciones Clave:
```cpp
constexpr bool is_minus() const { return (!(((*this)[R]).is_0())); }
constexpr void set_minus() { (*this)[R] = dig_Bm1(); }
constexpr void normalize_sign() { is_minus() ? set_minus() : dig_t{}; }
```

### Análisis para int_reg_digs_t<2,3> (R=3):
- **`is_minus()`**: Verifica si `(*this)[3] ≠ 0`
- **`set_minus()`**: Establece `(*this)[3] = dig_Bm1() = 1₂`
- **`normalize_sign()`**: Si negativo detectado, fuerza bit de signo

### ⚠️ OBSERVACIÓN CRÍTICA:
```
Para int_reg_digs_t<2,3>:
  Dígitos: [0] [1] [2] [3]
           LSB      MSB(signo)

  is_minus() verifica dígito[3] ≠ 0
  set_minus() establece dígito[3] = 1

  Esto sugiere BIT DE SIGNO en lugar de complemento a la base puro.
```

### Comportamiento Esperado:
- **Complemento Puro**: -1 debería ser `1111₂` (todos los bits en 1)
- **Implementación Real**: Parece usar dígito[R] como flag independiente
- **Implicación**: Sistema híbrido entre bit de signo y complemento

---

## 9. OPERADOR ASIGNACIÓN DESDE dig_t

### Implementación:
```cpp
constexpr inline int_reg_digs_t &operator=(dig_t arg) noexcept
{
  base_t &cthis{*static_cast<base_t *>(this)};
  if (&(cthis[0]) != (&arg))
  {
    for (dig_t &dig : cthis) 
      dig = dig_0();          // Reset todos a 0
    cthis[0] = arg;           // Solo LSB = arg
  }
  return (*this);
}
```

### Análisis:
- **Reset Completo**: Todos los dígitos a 0 primero
- **Asignación Simple**: Solo LSB recibe el valor
- **⚠️ Sin Normalización**: NO aplica `normalize_sign()`
- **Uso**: `num = dig_1()` → `0001₂`

### Inconsistencia:
- Otros constructores SÍ aplican `normalize_sign()`
- Este operador NO lo aplica
- Puede causar estados inconsistentes

---

## 10. RESUMEN Y EVALUACIÓN

### ✅ FORTALEZAS:
1. **Cobertura Completa**: 9 tipos diferentes de constructores
2. **Type Safety**: Templates con `requires` apropiados
3. **Move Semantics**: Optimizaciones para tipos temporales
4. **Flexibilidad**: Conversiones automáticas entre tipos relacionados
5. **Manejo Inteligente**: Adaptación automática según tamaños

### ⚠️ INCONSISTENCIAS DETECTADAS:
1. **Normalización Desigual**: No todos aplican `normalize_sign()`
2. **Sistema Signo Híbrido**: Mezcla bit de signo con complemento
3. **Casos Edge**: Constructor lista omite normalización
4. **Asignación dig_t**: Sin normalización automática

### 🎯 PATRONES IDENTIFICADOS:

#### Aplicación de normalize_sign():
- ✅ Constructor variádico desde dígitos
- ✅ Constructor desde nat_reg_digs_t (copia/move)
- ✅ Constructor desde tipos integrales
- ✅ Constructores inter-int_reg_digs_t
- ❌ Constructor lista inicialización
- ❌ Operador asignación desde dig_t

#### Delegación al Constructor Base:
- Constructor por defecto → `base_t{}`
- Lista inicialización → `base_t{arg}`
- Tipos integrales → `base_t(dig_pow_i...)`

#### Funciones Template Auxiliares:
- `copy_arg_N<N>()` → Manejo copia con normalización
- `move_arg_N<N>()` → Manejo movimiento con normalización
- `pack2array<>` → Conversión parameter pack

---

## 11. CONCLUSIONES FASE 3

### ANÁLISIS COMPLETADO ✅:
- **9 tipos de constructores** analizados teóricamente
- **Patrones de implementación** documentados
- **Sistema normalización** caracterizado
- **Inconsistencias** identificadas y documentadas
- **Base sólida** para verificación empírica establecida

### PREPARACIÓN FASE 4:
- Constructores proporcionan base para **operadores aritméticos**
- Sistema normalización será clave para **operaciones con signo**
- Inconsistencias detectadas requerirán **atención especial**

### PRÓXIMO OBJETIVO:
**FASE 4: OPERADORES ARITMÉTICOS** (`+`, `-`, `*`, `/`, `%`)
- Análisis de implementación aritmética
- Verificación compatibilidad con complemento a la base
- Validación de normalización en resultados