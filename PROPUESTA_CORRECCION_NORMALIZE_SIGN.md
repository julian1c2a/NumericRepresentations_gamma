/**
 * @file PROPUESTA_CORRECCION_NORMALIZE_SIGN.md
 * @brief Propuesta de corrección para normalize_sign() basada en hallazgos
 */

# 🚨 PROBLEMA CONFIRMADO: normalize_sign() ES INCORRECTA

## ✅ EVIDENCIA ENCONTRADA

### CÓDIGO PROBLEMÁTICO ACTUAL:
```cpp
constexpr void normalize_sign() { is_minus() ? set_minus() : set_plus(); }
```

**PROBLEMA:** Es una función **no-op circular** que no normaliza nada real.

### EVIDENCIA DE NORMALIZACIÓN CORRECTA EN EL CÓDIGO:

**En `operator--()` líneas 943-953:**
```cpp
const bool is_0_filled{r_cthis.is_0()};

if (is_plus() && is_0_filled)
{
  r_cthis.set_fill_Bm1();
  set_minus(); 
}
else if (is_minus() && is_0_filled)
{
  r_cthis.set_fill_Bm1();
  set_plus();  // ✅ NORMALIZACIÓN REAL: -0 → +0
}
```

**ESTO DEMUESTRA:**
1. ✅ El sistema SÍ maneja casos de -0 vs +0
2. ✅ Existe `r_cthis.is_0()` para detectar magnitud cero
3. ✅ La lógica correcta convierte -0 a +0

## 🔧 PROPUESTA DE CORRECCIÓN

### OPCIÓN 1: CORRECCIÓN MÍNIMA (Recomendada)
```cpp
constexpr void normalize_sign() { 
    base_t &r_base_cthis{*static_cast<base_t *>(this)};
    // Normalizar -0 a +0 (caso más común de inconsistencia)
    if (is_minus() && r_base_cthis.is_0()) {
        set_plus();
    }
}
```

**JUSTIFICACIÓN:**
- Corrige el caso más importante: -0 → +0
- Mantiene compatibilidad con código existente
- Implementación simple y eficiente

### OPCIÓN 2: CORRECCIÓN COMPLETA (Si se necesita)
```cpp
constexpr void normalize_sign() { 
    base_t &r_base_cthis{*static_cast<base_t *>(this)};
    
    // Caso 1: -0 debe ser +0
    if (is_minus() && r_base_cthis.is_0()) {
        set_plus();
        return;
    }
    
    // Caso 2: Otros casos de normalización según semántica específica
    // (Requeriría más análisis del sistema)
}
```

## 🎯 RECOMENDACIÓN FINAL

### APLICAR LA CORRECCIÓN:

**SÍ, normalize_sign() DEBE ser corregida** porque:

1. **La implementación actual es defectuosa** (no-op circular)
2. **El sistema tiene casos reales de normalización** (evidenciado en operator--)
3. **La corrección es simple y segura**

### IMPLEMENTACIÓN INMEDIATA:

```cpp
// LÍNEA 72: include/int_reg_digs_t.hpp
// ANTES (INCORRECTO):
constexpr void normalize_sign() { is_minus() ? set_minus() : set_plus(); }

// DESPUÉS (CORREGIDO):
constexpr void normalize_sign() { 
    base_t &r_base_cthis{*static_cast<base_t *>(this)};
    if (is_minus() && r_base_cthis.is_0()) {
        set_plus();  // Normalizar -0 → +0
    }
}
```

**IMPACTO:**
- ✅ Corrige normalización de -0 → +0
- ✅ Mantiene compatibilidad hacia atrás
- ✅ Hace que normalize_sign() realmente normalice
- ✅ Consistente con lógica encontrada en operator--

¿Aplicamos esta corrección?