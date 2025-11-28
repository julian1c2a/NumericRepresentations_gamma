# MSVC /constexpr: Flags Investigation

## Flags conocidos que estamos usando:
- `/constexpr:depth8192` - Profundidad de recursión constexpr (default: 2048)
- `/constexpr:steps4194304` - Número máximo de pasos en evaluación constexpr (default: ~4M)
- `/bigobj` - Permite construir objetos binarios realmente grandes

