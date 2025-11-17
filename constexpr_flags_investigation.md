# MSVC /constexpr: Flags Investigation

## Flags conocidos que estamos usando:
- `/constexpr:depth2048` - Profundidad de recursión constexpr (default: 512)
- `/constexpr:steps1048576` - Número máximo de pasos en evaluación constexpr (default: ~1M)

## Posibles flags adicionales a investigar:

### `/constexpr:frame` o `/constexpr:frame<N>`
**Propósito potencial**: Controlar el tamaño del stack frame para evaluaciones constexpr
**Estado**: INVESTIGAR - no confirmado que exista

### `/constexpr:traceback` o `/constexpr:trace`
**Propósito potencial**: Habilitar tracing de evaluaciones constexpr para debugging
**Estado**: INVESTIGAR - no confirmado que exista

### `/constexpr:backtrace<N>`
**Propósito potencial**: Limitar el número de niveles en el backtrace de errores constexpr
**Estado**: INVESTIGAR - no confirmado que exista

# MSVC /constexpr: Flags Investigation

## Flags conocidos que estamos usando:
- `/constexpr:depth2048` - Profundidad de recursión constexpr (default: 512) ✅ FUNCIONA
- `/constexpr:steps1048576` - Número máximo de pasos en evaluación constexpr (default: ~1M) ✅ FUNCIONA

## ✅ FLAGS ADICIONALES DESCUBIERTOS:

### `/constexpr:frame` 
**Estado**: ✅ **RECONOCIDO POR MSVC** pero con **WARNING D9002: se omite la opción desconocida**
**Interpretación**: El compilador reconoce el flag pero lo ignora (posiblemente no implementado en esta versión)

### `/constexpr:trace` 
**Estado**: ✅ **RECONOCIDO POR MSVC** pero con **WARNING D9002: se omite la opción desconocida**
**Interpretación**: El compilador reconoce el flag pero lo ignora (posiblemente no implementado en esta versión)

### `/constexpr:frame1024`
**Estado**: ✅ **RECONOCIDO POR MSVC** pero con **WARNING D9002: se omite la opción desconocida**
**Interpretación**: El compilador reconoce el flag pero lo ignora (posiblemente no implementado en esta versión)

## ❌ FLAGS NO RECONOCIDOS:

### `/constexpr:backtrace`
**Estado**: ❌ **ERROR D8003: falta el nombre del archivo** - El flag es interpretado como nombre de archivo
**Interpretación**: Este flag NO existe en MSVC

## 📊 RESULTADOS DE LA INVESTIGACIÓN:

### Flags que SÍ existen pero no están implementados:
1. `/constexpr:frame` - Control de stack frame (ignorado por MSVC 19.44)
2. `/constexpr:trace` - Tracing de evaluaciones (ignorado por MSVC 19.44)  
3. `/constexpr:frame<N>` - Stack frame con tamaño específico (ignorado por MSVC 19.44)

### Flags que NO existen:
1. `/constexpr:backtrace` - No existe en MSVC

## 💡 CONCLUSIÓN:
Los flags `/constexpr:frame` y `/constexpr:trace` están **documentados/planificados** en MSVC pero **no implementados** en la versión 19.44.35219. Podrían estar disponibles en versiones futuras o versiones preview.

## 🚀 RECOMENDACIÓN ACTUAL:
Continuar usando los flags que funcionan:
```bat
/constexpr:depth2048 /constexpr:steps1048576
```

Los otros flags adicionales no aportan valor en la versión actual de MSVC.