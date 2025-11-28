# dig_t<Base> - Análisis de Operadores de Entrada/Salida (I/O)

## Resumen Ejecutivo

Los operadores de E/S de `dig_t<Base>` implementan un **protocolo de serialización estructurado** que garantiza la integridad de tipos durante la lectura/escritura. El formato de salida es autodocumentado y el parser de entrada utiliza una **máquina de estados finitos** para validación robusta.

## Formato de Serialización

### Formato Estándar
```
d[valor]Bbase
```

**Ejemplos:**
- `d[7]B10` - dígito 7 en base 10
- `d[15]B16` - dígito 15 en base 16  
- `d[0]B8` - dígito 0 en base 8

## Operador de Salida (operator<<)

```cpp
template <std::uint64_t Base>
  requires(Base > 1)
std::ostream &operator<<(std::ostream &os, dig_t<Base> arg)
{
  os << "d[" << static_cast<std::int64_t>(arg())
     << "]B" << static_cast<std::int64_t>(Base);
  return os;
}
```

### Análisis del Operador de Salida

**Componentes del formato:**
- **Prefijo**: `"d["` - Identificador de tipo dígito
- **Valor**: `arg()` - Valor interno del dígito (conversión a int64_t)
- **Separador**: `"]B"` - Delimitador valor/base
- **Base**: `Base` - Parámetro template (conversión a int64_t)

**Características técnicas:**
- **Template constraints**: `requires(Base > 1)` - Validación compile-time
- **Casting explícito**: `static_cast<std::int64_t>` para compatibilidad de streams
- **Autodocumentación**: Formato incluye tanto valor como base
- **Inmutabilidad**: Recibe `arg` por valor (no modifica original)

## Operador de Entrada (operator>>)

### Arquitectura de Parsing

El operador de entrada implementa una **máquina de estados finitos** (FSM) con 9 estados para parsing robusto:

```cpp
enum class estado_e {
  e0,  // Estado inicial/error
  e1,  // Encontrado 'd'
  e2,  // Encontrado "d#" (esperando dígitos)
  e3,  // Leyendo dígitos del valor
  e4,  // Encontrado "#" después del valor
  e5,  // Encontrado "B" 
  e6,  // Leyendo dígitos de la base
  e7,  // Validación final
  e8   // Estado de éxito/terminación
};
```

### Análisis Detallado del Parser

#### Variables de Estado
```cpp
std::string sds;                                    // Buffer de entrada
std::string num_dig;                               // Dígitos del valor
std::string num_base;                              // Dígitos de la base
estado_e est_act = estado_e::e0;                   // Estado actual
typename dig_t<Base>::sig_uint_t indice{0};        // Índice de carácter
typename dig_t<Base>::sig_uint_t numero_base_recogido{0}; // Base parseada
uint_t numero{0};                                  // Valor final
```

#### Máquina de Estados Detallada

**Estado e0 (Inicial/Error):**
```cpp
case estado_e::e0:
  if (c == 'd') {
    est_act = estado_e::e1;  // Avanza si encuentra 'd'
  }
  break;
```

**Estado e1 (Después de 'd'):**
```cpp
case estado_e::e1:
  if (c == '#') {
    est_act = estado_e::e2;  // Esperamos "d#"
  } else {
    est_act = estado_e::e0;  // Error: reset
  }
  break;
```

**Estados e2-e3 (Parsing del Valor):**
```cpp
case estado_e::e2:
  if ((c <= '9') and (c >= '0')) {
    est_act = estado_e::e3;
    num_dig.push_back(c - '0');  // Primer dígito del valor
  } else {
    est_act = estado_e::e0;      // Error: reset
    num_dig.clear();
  }
  break;

case estado_e::e3:
  if ((c <= '9') and (c >= '0'))
    num_dig.push_back(c - '0');  // Más dígitos del valor
  else if (c == '#')
    est_act = estado_e::e4;      // Fin del valor
  else {
    est_act = estado_e::e0;      // Error: reset
    num_dig.clear();
  }
  break;
```

**Estados e4-e6 (Parsing de la Base):**
```cpp
case estado_e::e4:
  if (c == 'B')
    est_act = estado_e::e5;      // Esperamos "#B"
  else {
    est_act = estado_e::e0;      // Error: reset
    num_dig.clear();
  }
  break;

case estado_e::e5:
  if ((c <= '9') and (c >= '0')) {
    est_act = estado_e::e6;
    num_base.push_back(c - '0'); // Primer dígito de base
  } else {
    est_act = estado_e::e0;      // Error: reset completo
    num_dig.clear();
    num_base.clear();
  }
  break;

case estado_e::e6:
  if ((c <= '9') and (c >= '0'))
    num_base.push_back(c - '0'); // Más dígitos de base
  else if ((c == ' ') || (c == '\0'))
    est_act = estado_e::e7;      // Fin de entrada
  break;
```

**Estado e7 (Validación Final):**
```cpp
case estado_e::e7:
  // Construcción de numero_base_recogido
  for (std::size_t k = 0; k < num_base.size(); ++k) {
    numero_base_recogido *= 10;
    numero_base_recogido += num_base[k];
  }
  
  if (numero_base_recogido != Base) {
    est_act = estado_e::e0;      // Error: base no coincide
    num_dig.clear();
    num_base.clear();
  } else {
    est_act = estado_e::e8;      // ¡Éxito!
  }
  break;
```

#### Construcción del Valor Final

```cpp
for (std::size_t k = 0; k < num_dig.size(); ++k) {
  numero *= 10;
  numero += num_dig[k];
}
numero %= Base;                    // Aplicación automática de aritmética modular
arg = dig_t<Base>(numero);         // Construcción del resultado
```

### Características Avanzadas del Parser

#### 1. Validación de Tipos en Tiempo de Ejecución
- **Verificación de base**: La base parseada debe coincidir exactamente con el parámetro template `Base`
- **Aritmética modular automática**: `numero %= Base` garantiza valores válidos
- **Robustez**: Estados de error con limpieza completa de buffers

#### 2. Manejo de Errores
- **Reset automático**: Cualquier error retorna al estado e0
- **Limpieza de buffers**: `num_dig.clear()` y `num_base.clear()` en errores
- **Parsing defensivo**: Validación estricta de cada carácter

#### 3. Optimizaciones
- **Conversión directa**: `c - '0'` para chars numéricos
- **Construcción incremental**: Números construidos dígito a dígito
- **Terminación temprana**: Estados e7/e8 para finalización eficiente

## Formato de Entrada Alternativo (d# vs d[)

**Diferencia importante**: El parser espera formato `d#valor#Bbase` pero el output genera `d[valor]Bbase`.

### Análisis de Inconsistencia

**Salida esperada según el parser:**
```
d#7#B10  (formato que el parser puede leer)
```

**Salida actual del operador<<:**
```
d[7]B10  (formato que el parser NO puede leer)
```

Esta inconsistencia sugiere que:
1. El formato de salida debería usar `#` en lugar de `[` y `]`
2. O el parser debería actualizarse para aceptar `[` y `]`

## Casos de Uso y Ejemplos

### Ejemplo de Salida
```cpp
dig_t<10> d{7};
std::cout << d;  // Imprime: d[7]B10
```

### Ejemplo de Entrada (formato correcto para el parser)
```cpp
dig_t<10> d;
// Para que funcione, necesitaríamos entrada: "d#7#B10"
std::cin >> d;   // Actualmente busca formato d#...#B...
```

## Consideraciones de Diseño

### 1. Seguridad de Tipos
- **Template constraints**: `requires(Base > 1)`
- **Validación runtime**: Base parseada debe coincidir con Base template
- **Aritmética automática**: Aplicación de mod Base

### 2. Robustez
- **Máquina de estados**: Parsing estructurado y determinista
- **Manejo de errores**: Estados de error con recuperación
- **Validación estricta**: Cada carácter es validado

### 3. Autodocumentación
- **Formato explícito**: Incluye tanto valor como base
- **Legibilidad**: Formato comprensible para humanos
- **Debugging**: Fácil identificación de valores y tipos

## Análisis de Rendimiento

### Complejidades
- **Salida**: O(log₁₀(valor) + log₁₀(Base)) - Conversión a string
- **Entrada**: O(n) donde n = longitud del string de entrada
- **Memoria**: O(log₁₀(valor) + log₁₀(Base)) - Buffers temporales

### Optimizaciones Potenciales
1. **Pool de strings**: Reutilización de buffers para múltiples operaciones
2. **Parsing directo**: Evitar almacenamiento intermedio de dígitos
3. **Formato consistente**: Resolver inconsistencia d[/] vs d#/#

## Conclusiones

Los operadores de E/S de `dig_t<Base>` implementan un **protocolo de serialización robusto** con:

**Fortalezas:**
- Validación completa de tipos
- Formato autodocumentado  
- Parser robusto con manejo de errores
- Aritmética modular automática

**Área de mejora identificada:**
- Inconsistencia entre formato de salida (`d[...]B`) y formato de entrada esperado (`d#...#B`)

Esta implementación demuestra un enfoque sofisticado para E/S type-safe en sistemas de aritmética modular.