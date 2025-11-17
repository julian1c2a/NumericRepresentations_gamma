/**
 * @file core/dig_t.hpp
 * @brief Copia adaptada de include/dig_t.hpp para el nuevo layout
 */

#ifndef CORE_DIG_T_HPP_INCLUDED
#define CORE_DIG_T_HPP_INCLUDED

#include <array>
#include <expected>
#include <string>

#include "internal/basic_types.hpp"
#include "internal/auxiliary_functions.hpp"

namespace NumRepr {

  using type_traits::maxbase;
  using type_traits::sig_SInt_for_UInt_t;
  using type_traits::sig_UInt_for_UInt_t;
  using type_traits::sqrt_max;
  using type_traits::suitable_base;
  using type_traits::uint_type_for_radix_c;

  /**
   * @brief Códigos de error para parsing de dígitos desde cadenas
   * @details Usados por parse_impl_ct para retornar errores detallados
   *          mediante std::expected sin lanzar excepciones
   */
  enum class parse_error_t : int {
    empty_or_null,    ///< String vacío, nullptr o demasiado corto (< 4 chars)
    invalid_prefix,   ///< Prefijo inválido (esperado: "d[", "dig#", "d#", "dig[")
    invalid_digit,    ///< Carácter no numérico en el valor del dígito
    no_digits,        ///< No se encontraron dígitos en el valor
    missing_delimiter,///< Falta delimitador de cierre (']' o '#')
    missing_B,        ///< Falta carácter 'B' antes de la base
    invalid_base_digit, ///< Carácter no numérico en la base
    no_base_digits,   ///< No se encontraron dígitos en la base
    base_mismatch,    ///< Base leída no coincide con la base template B
    unknown           ///< Error desconocido (no usado actualmente)
  };

  template <std::uint64_t B>
    requires(B > 1)
  struct dig_t {
    // Validación explícita de la restricción de base máxima
    static_assert(B <= std::numeric_limits<std::uint32_t>::max(),
                  "dig_t: Base B must be <= UINT32_MAX (4294967296). "
                  "This restriction ensures that digit multiplications (B-1)×(B-1) "
                  "fit within uint64_t without overflow.");
    
    /**
     * @brief Tipo entero sin signo para almacenar un dígito en base B
     * @details Selecciona automáticamente el tipo entero sin signo más pequeño 
     *          capaz de representar valores de 0 a B-1:
     *          - B ∈ [2, 256]: uint8_t
     *          - B ∈ [257, 65536]: uint16_t
     *          - B ∈ [65537, 4294967296]: uint32_t (máximo soportado)
     * 
     * @note RESTRICCIÓN: La base B debe ser <= UINT32_MAX (4294967296)
     * @note Esta restricción existe porque las multiplicaciones de dígitos
     *       (B-1) × (B-1) deben caber en el tipo superior sin overflow.
     *       Con B <= UINT32_MAX, el producto cabe en uint64_t (sig_uint_t).
     * 
     * @warning Bases mayores a UINT32_MAX causarán error de compilación con
     *          mensaje claro gracias al static_assert anterior.
     */
    using uint_t = 
        typename type_traits::TypeFromIntNumber_t<static_cast<uint64_t>(B)>;

  private:
    /// @brief Valor del dígito: único dato/estado de la clase
    /// @details Siempre normalizado en el rango [0, B-1]
    uint_t m_d;

  public:
    // =========================================================================
    // TIPOS AUXILIARES PÚBLICOS
    // =========================================================================
    
    /**
     * @brief Tipo entero sin signo superior - para prevenir overflow
     * @details Tipo con el doble de bits que uint_t:
     *          - uint8_t → uint16_t
     *          - uint16_t → uint32_t  
     *          - uint32_t → uint64_t
     * @note Usado en operaciones aritméticas donde (B-1)×(B-1) debe caber
     * @example typename dig_t<255>::sig_uint_t es uint16_t
     */
    using sig_uint_t = typename type_traits::sig_UInt_for_UInt_t<uint_t>;
    
    /**
     * @brief Tipo entero con signo superior - para aritmética con signo
     * @details Tipo con signo del doble de bits que uint_t:
     *          - uint8_t → int16_t
     *          - uint16_t → int32_t
     *          - uint32_t → int64_t
     * @note Usado en normaliza() para manejar valores negativos correctamente
     * @example typename dig_t<10>::sig_sint_t es int16_t
     */
    using sig_sint_t = typename type_traits::sig_SInt_for_UInt_t<uint_t>;
    
    /**
     * @brief Par de valores uint_t - actualmente usado en multiplicaciones
     * @details Formato: [cociente, resto] para operaciones que producen dos valores
     * @note Usado extensivamente en mult() para devolver carry y resultado
     */
    using uintspair = std::array<uint_t, 2>;
    
    /**
     * @brief Par de dígitos dig_t - actualmente NO usado
     * @details Definido para simetría con uintspair
     * @todo Considerar uso futuro en operaciones que devuelvan múltiples dígitos
     */
    using digspair = std::array<dig_t, 2>;
    
    /**
     * @brief Lista de pares indexed por un dígito - actualmente NO usado
     * @details Array de B elementos, cada uno es un uintspair
     * @note Podría usarse para tablas de multiplicación precalculadas
     */
    using uintspairlist = std::array<uintspair, B>;
    
    /**
     * @brief Tabla 2D de pares - actualmente NO usado
     * @details Matriz B×B de uintspair
     * @note Definido para futura tabla completa de multiplicación
     */
    using uintspairtbl = std::array<uintspairlist, B>;

    // =========================================================================
    // MULTIPLICACIÓN EN TIEMPO DE COMPILACIÓN CON GESTIÓN DE OVERFLOW
    // =========================================================================
    
    /**
     * @brief Multiplicación compile-time de dos valores < B con carry explícito
     * @tparam n Primer factor (debe ser < B)
     * @tparam m Segundo factor (debe ser < B)
     * @return Array [cociente, resto] donde n×m = cociente×B + resto
     * 
     * @details Calcula n×m en tiempo de compilación, devolviendo:
     *          - ret[0] = carry = (n×m) / B
     *          - ret[1] = dígito = (n×m) % B
     * 
     * @note ESTRATEGIA ANTI-OVERFLOW:
     *       - Si B > √(MAX_uint_t): promociona a sig_uint_t antes de multiplicar
     *       - Si B ≤ √(MAX_uint_t): multiplica directamente en uint_t
     * 
     * @note Evaluada completamente en tiempo de compilación (consteval)
     * 
     * @example dig_t<10>::mult<7, 8>() devuelve {5, 6} porque 7×8 = 56 = 5×10 + 6
     */
    template <uint_t n, uint_t m>
      requires((n < B) && (m < B))
    static consteval uintspair mult() noexcept {
      if constexpr (B > type_traits::sqrt_max<uint_t>()) {
        constexpr sig_uint_t sup_n{n};
        constexpr sig_uint_t sup_m{m};
        constexpr sig_uint_t result{sup_n * sup_m};
        constexpr uint_t ret_1{result / B};
        constexpr uint_t ret_0{result % B};
        constexpr uintspair ret{ret_1, ret_0};
        return ret;
      } else {
        constexpr uint_t result{n * m};
        constexpr uint_t ret_1{result / B};
        constexpr uint_t ret_0{result % B};
        constexpr uintspair ret{ret_1, ret_0};
        return ret;
      }
    }

  public:
    // =========================================================================
    // CONVERSIONES Y ACCESO AL VALOR
    // =========================================================================
    
    /**
     * @brief Conversión explícita a uint_t (tipo nativo del dígito)
     * @return Valor del dígito como uint_t
     * @note Conversión explícita - requiere static_cast<uint_t>(digit)
     */
    constexpr explicit operator uint_t() const noexcept { return m_d; }
    
    /**
     * @brief Obtiene el valor del dígito
     * @return Valor del dígito en el rango [0, B-1]
     * @note FORMA PREFERIDA para acceder al valor. Usado ampliamente en el código.
     * @see operator()() para sintaxis alternativa (deprecated)
     */
    constexpr uint_t get() const noexcept { return m_d; }
    
    /**
     * @brief Conversión explícita a sig_uint_t (tipo superior sin signo)
     * @return Valor del dígito convertido a tipo superior
     * @note Útil para operaciones que requieren más bits (evitar overflow)
     */
    constexpr explicit operator sig_uint_t() const noexcept { 
        return static_cast<sig_uint_t>(m_d); 
    }
    
    /**
     * @brief Conversión explícita a sig_sint_t (tipo superior con signo)
     * @return Valor del dígito convertido a tipo con signo superior
     * @note Útil para operaciones aritméticas que requieren signo
     */
    constexpr explicit operator sig_sint_t() const noexcept { 
        return static_cast<sig_sint_t>(m_d); 
    }
    
    /**
     * @brief Sintaxis funcional para obtener el valor del dígito
     * @return Valor del dígito en el rango [0, B-1]
     * @deprecated Use get() en su lugar. Mantenido por compatibilidad.
     * @note Mismo comportamiento que get() pero con sintaxis digit()
     */
    [[deprecated("Use get() instead")]]
    constexpr uint_t operator()() const noexcept { return m_d; }
    
    // =========================================================================
    // MODIFICADORES DEL VALOR
    // =========================================================================
    
    /// @brief Establece el valor del dígito a 0
    constexpr void set_0() noexcept { m_d = 0; }
    
    /// @brief Establece el valor del dígito a 1
    constexpr void set_1() noexcept { m_d = 1; }
    
    /// @brief Establece el valor del dígito a B-1 (máximo valor)
    constexpr void set_Bm1() noexcept { m_d = B - 1; }
    
    /// @brief Establece el valor del dígito a B-2
    constexpr void set_Bm2() noexcept { m_d = B - 2; }
    
    /// @brief Copia el valor desde otro dígito
    /// @param d Dígito fuente
    constexpr void set_dig(dig_t d) noexcept { m_d = d.m_d; }
    
    /// @brief Establece el valor desde un entero (normalizado módulo B)
    /// @param di Valor entero a asignar (se normaliza automáticamente)
    constexpr void set_dig(uint_t d) noexcept { m_d = d % B; }

  public:
    // =========================================================================
    // PROPIEDADES ALGEBRAICAS Y TEORÍA DE ANILLOS
    // =========================================================================
    
    /**
     * @brief Determina si la base B es un número primo
     * @return true si B es primo, false en caso contrario
     * 
     * @details TEORÍA DE ANILLOS: Cuando B es primo, ℤ/Bℤ forma un cuerpo 
     *          conmutativo finito (campo de Galois GF(B)). En este caso:
     *          - Todo elemento no nulo tiene inverso multiplicativo
     *          - No existen divisores de cero (excepto el 0)
     *          - Se pueden realizar divisiones (excepto por 0)
     * 
     * @note Evaluada en tiempo de compilación (consteval)
     * @see is_unit() - verifica si un dígito específico tiene inverso
     * @see mult_inv() - calcula el inverso multiplicativo
     */
    consteval static bool is_prime() noexcept {
      return auxiliary_functions::is_prime(static_cast<std::size_t>(B));
    }

    // =========================================================================
    // CONSTANTES ESTÁTICAS - Dígitos especiales
    // =========================================================================

    consteval static dig_t dig_max() noexcept { return dig_t(B - 1u); };
    consteval static dig_t dig_submax() noexcept { return dig_t(B - 2u); }
    consteval static dig_t dig_Bm1() noexcept { return dig_t(B - 1u); }
    consteval static dig_t dig_Bm2() noexcept { return dig_t(B - 2u); }
    consteval static dig_t dig_0() noexcept { return dig_t(); }
    consteval static dig_t dig_1() noexcept { return dig_t(1u); }

    consteval static uint_t ui_max() noexcept { return uint_t(B - 1u); }
    consteval static uint_t ui_submax() noexcept { return uint_t(B - 2u); }
    consteval static uint_t ui_Bm1() noexcept { return uint_t(B - 1u); }
    consteval static uint_t ui_Bm2() noexcept { return uint_t(B - 2u); }
    consteval static uint_t ui_0() noexcept { return uint_t(0u); }
    consteval static uint_t ui_1() noexcept { return uint_t(1u); }

    consteval static sig_uint_t sui_B() noexcept { 
        return static_cast<sig_uint_t>(B); }
    consteval static sig_uint_t sui_max() noexcept { 
        return static_cast<sig_uint_t>(B - 1u); }
    consteval static sig_uint_t sui_submax() noexcept { 
        return static_cast<sig_uint_t>(B - 2u); }
    consteval static sig_uint_t sui_0() noexcept { 
        return static_cast<sig_uint_t>(0u); }
    consteval static sig_uint_t sui_1() noexcept { 
        return static_cast<sig_uint_t>(1u); }

    consteval static sig_sint_t ssi_B() noexcept { 
        return static_cast<sig_sint_t>(B); }
    consteval static sig_sint_t ssi_max() noexcept { 
        return static_cast<sig_sint_t>(B - 1u); }
    consteval static sig_sint_t ssi_submax() noexcept { 
        return static_cast<sig_sint_t>(B - 2u); }
    consteval static sig_sint_t ssi_0() noexcept { 
        return static_cast<sig_sint_t>(0u); }
    consteval static sig_sint_t ssi_1() noexcept { 
        return static_cast<sig_sint_t>(1u); }

  public:
    // =========================================================================
    // CONSTRUCTOR POR DEFECTO
    // =========================================================================
    
    /**
     * @brief Constructor por defecto - inicializa a 0 (identidad aditiva)
     * @note Evaluado en tiempo de compilación (consteval)
     * @note Adelantado aquí por necesidad (usado en otras partes de la clase)
     * @details En teoría de anillos, 0 es la identidad aditiva de ℤ/Bℤ
     */
    consteval inline dig_t() noexcept : m_d(0u) {}

  private:
    // =========================================================================
    // FUNCIÓN NORMALIZA - Reducción módulo B desde cualquier entero
    // =========================================================================
    
    /**
     * @brief Normaliza cualquier entero (con/sin signo) al rango [0, B-1]
     * @tparam Int_t Tipo entero (deducido automáticamente)
     * @param arg Valor a normalizar
     * @return Valor normalizado en [0, B-1]
     * 
     * @details ESTRATEGIA DE NORMALIZACIÓN:
     *          1. Si arg ya es uint_t: simple módulo
     *          2. Si arg es con signo: maneja negativos correctamente
     *             - Usa tipo superior (sig_sint_t) si necesario
     *             - Suma múltiplos de B hasta hacer positivo
     *             - Finalmente aplica módulo
     *          3. Si arg es sin signo más grande: módulo en tipo superior
     * 
     * @note CORRECCIÓN PARA NEGATIVOS: Si x < 0, suma ⌈|x|/B⌉×B hasta x ≥ 0
     * @note Método estático fundamental para toda operación aritmética
     * 
     * @example normaliza(-7) en base 10 → 3 (porque -7 ≡ 3 (mod 10))
     * @example normaliza(23) en base 10 → 3
     */
    template <type_traits::integral_c Int_t>
    constexpr static inline uint_t normaliza(Int_t arg) noexcept {
      if constexpr (std::is_same_v<Int_t, uint_t>) { return (arg % B); }
      else if constexpr (std::is_signed_v<Int_t>) {
        if constexpr (
            type_traits::maxbase<Int_t>() >= type_traits::maxbase<sig_sint_t>()
        ) {
          constexpr Int_t sint_0{0};
          constexpr Int_t sint_B{B};
          Int_t cparg{arg};
          if (arg < sint_0) {
            Int_t coc{(-arg) / sint_B};
            coc *= sint_B;
            cparg += coc;
            if (cparg < 0)
              cparg += sint_B;
            if (cparg >= sint_B)
              cparg -= sint_B;
          }
          else { cparg %= sint_B; }
          return static_cast<uint_t>(cparg);
        } else {
          constexpr sig_sint_t sint_0{0};
          constexpr sig_sint_t sint_B{B};
          sig_sint_t cparg{arg};
          if (arg < sint_0) {
            sig_sint_t coc{(-arg) / sint_B};
            coc *= sint_B;
            cparg += coc;
            if (cparg < 0)
              cparg += sint_B;
            if (cparg >= sint_B)
              cparg -= sint_B;
          } else { cparg %= sint_B; }
          
          return static_cast<uint_t>(cparg);
        }
      } else {
        if constexpr (maxbase<Int_t>() < maxbase<uint_t>()) {
          constexpr sig_uint_t uint_B{B};
          sig_uint_t cparg{arg};
          if (arg >= uint_B) { cparg %= uint_B; }
          return static_cast<uint_t>(cparg);
        } else {
          constexpr Int_t uint_B{B};
          Int_t cparg{arg};
          if (arg >= uint_B) { cparg %= uint_B; }
          return static_cast<uint_t>(cparg);
        }
      }
    }

  public:
    // =========================================================================
    // CONSTRUCTORES Y ASIGNACIÓN
    // =========================================================================
    
    /**
     * @brief Constructor desde entero (con/sin signo, cualquier tamaño)
     * @tparam Int_t Tipo entero (deducido automáticamente)
     * @param arg Valor entero a convertir en dígito
     * @note Normaliza automáticamente: arg módulo B
     * @example dig_t<10>(23) → valor 3
     * @example dig_t<10>(-7) → valor 3 (porque -7 ≡ 3 (mod 10))
     */
    template <type_traits::integral_c Int_t>
    constexpr dig_t(Int_t arg) noexcept : m_d(normaliza<Int_t>(arg)) {}
    
    /**
     * @brief Constructor de copia (generado por defecto)
     * @param other Dígito a copiar
     * @note Trivial: simplemente copia m_d
     */
    constexpr dig_t(const dig_t &) noexcept = default;
    
    /**
     * @brief Constructor de movimiento (generado por defecto)
     * @param other Dígito a mover
     * @note Trivial: igual que copia (dig_t es trivialmente copiable)
     */
    constexpr dig_t(dig_t &&) noexcept = default;

    /**
     * @brief Constructor desde std::string con parsing de formato específico
     * @param str Cadena a parsear (formatos: "d[N]BM", "dig#N#BM", etc.)
     * @note Si el formato es inválido, construye dig_t(0) (noexcept)
     * @note Explicit: no se permite conversión implícita desde string
     * @see from_string() para versión que retorna std::expected con errores
     */
    explicit dig_t(const std::string &str) noexcept;
    
    /**
     * @brief Constructor desde C-string con parsing de formato específico
     * @param str Cadena C a parsear (formatos: "d[N]BM", "dig#N#BM", etc.)
     * @note Si el formato es inválido o str==nullptr, construye dig_t(0) (noexcept)
     * @note Explicit: no se permite conversión implícita desde const char*
     * @note constexpr noexcept para uso seguro en tiempo de compilación
     * @see from_cstr() para versión que retorna std::expected con errores
     */
    explicit constexpr dig_t(const char *str) noexcept;

    /**
     * @brief Parsing desde std::string (versión static factory) SIN excepciones
     * @param str Cadena a parsear
     * @return std::expected<dig_t, parse_error_t> con dígito o error detallado
     * @note No lanza excepciones, usa std::expected para manejo de errores
     * @note Usa parse_impl internamente
     */
    static std::expected<dig_t, parse_error_t> from_string(const std::string &str) noexcept;
    
    /**
     * @brief Parsing desde C-string (versión static factory) SIN excepciones
     * @param str Cadena C a parsear
     * @return std::expected<dig_t, parse_error_t> con dígito o error detallado
     * @note constexpr y noexcept, no lanza excepciones
     * @note Retorna parse_error_t::null_input si str == nullptr
     * @note Usa parse_impl internamente
     */
    [[nodiscard]] static constexpr std::expected<dig_t, parse_error_t> from_cstr(const char *str) noexcept;

    /**
     * @brief Parsing desde std::array SIN excepciones (versión static factory)
     * @tparam Arr Array de caracteres (deducido automáticamente)
     * @return std::expected<dig_t, parse_error_t> con dígito o error detallado
     * 
     * @note Array pasado como argumento template (compile-time)
     * @note Usa parse_impl_ct internamente
     * @note Ideal para metaprogramación y contextos sin excepciones
     * @note MSVC limitation: std::expected no puede evaluarse en consteval con errores,
     *       así que la función es inline (runtime) a pesar del array compile-time
     * 
     * @example
     * ```cpp
     * auto result = dig_t<10>::from_array_ct<std::array{'d', '[', '5', ']', 'B', '1', '0'}>();
     * assert(result.has_value() && result->get() == 5);
     * ```
     */
    template <auto Arr>
    [[nodiscard]] static inline std::expected<dig_t, parse_error_t>
    from_array_ct() noexcept;

    // =========================================================================
    // OPERADORES DE ASIGNACIÓN
    // =========================================================================
    
    /**
     * @brief Asignación desde entero con normalización automática
     * @tparam Int_t Tipo entero (deducido automáticamente)
     * @param a Valor a asignar
     * @return Referencia a *this
     * @note ANTI-AUTOASIGNACIÓN: Si Int_t es uint_t, verifica &a != &m_d
     * @note Normaliza automáticamente mediante normaliza<Int_t>(a)
     */
    template <type_traits::integral_c Int_t>
    constexpr const dig_t &operator=(const Int_t &a) noexcept {
      if constexpr (std::is_same_v<Int_t, uint_t>) {
        if (&a != &m_d) { m_d = normaliza<Int_t>(a); }
      } else { m_d = normaliza<Int_t>(a); }
      return (*this);
    }
    
    /**
     * @brief Operador de asignación por copia (generado por defecto)
     * @param other Dígito a copiar
     * @return Referencia a *this
     * @note Trivial: simplemente copia m_d
     */
    constexpr dig_t &operator=(const dig_t &) noexcept = default;
    
    /**
     * @brief Operador de asignación por movimiento (generado por defecto)
     * @param other Dígito a mover
     * @return Referencia a *this
     * @note Trivial: igual que copia (dig_t es trivialmente copiable)
     */
    constexpr dig_t &operator=(dig_t &&) noexcept = default;

    // =========================================================================
    // PROPIEDADES ALGEBRAICAS DE ELEMENTOS INDIVIDUALES
    // =========================================================================
    // 
    // ADVERTENCIA: Las funciones is_unit() y mult_inv() tienen ineficiencias
    // algorítmicas significativas en sus implementaciones actuales:
    //
    // - is_unit(): O(log B) en caso general (usa std::gcd)
    // - mult_inv(): O(B × log B) en caso general (búsqueda exhaustiva)
    //
    // Para bases grandes (B > 10⁶), mult_inv() es IMPRACTICABLE.
    // Ver documentación detallada de cada función para mejoras propuestas.
    // =========================================================================
    
    /**
     * @brief Determina si el dígito es una unidad del anillo ℤ/Bℤ
     * @return true si existe inverso multiplicativo, false en caso contrario
     * 
     * @details TEORÍA DE ANILLOS: Un elemento a ∈ ℤ/Bℤ es unidad si existe
     *          b ∈ ℤ/Bℤ tal que a×b ≡ 1 (mod B).
     * 
     *          CRITERIO:
     *          - Si B es primo: todo elemento ≠ 0 es unidad (cuerpo GF(B))
     *          - Si B compuesto: a es unidad ⟺ gcd(a, B) = 1
     * 
     * @note CASOS ESPECIALES:
     *       - 1 siempre es unidad (identidad multiplicativa)
     *       - 0 nunca es unidad
     * 
     * @warning INEFICIENCIA ALGORÍTMICA (caso B compuesto, m_d ≠ 1):
     *          Usa std::gcd(B, m_d) que tiene complejidad O(log min(B, m_d)).
     *          Para bases grandes (B > 10⁶), puede ser lento.
     * 
     * @warning MEJORA PROPUESTA:
     *          - Precalcular tabla de unidades en tiempo de compilación
     *          - Usar bitset<B> o array<bool, B> para lookup O(1)
     *          - Para B primo, el check es O(1) (optimizado en tiempo de compilación)
     * 
     * @warning COMPILER WARNING:
     *          MSVC puede generar C4127 "conditional expression is constant"
     *          en el if constexpr. Puede ignorarse o suprimir con pragma.
     * 
     * @see mult_inv() para calcular el inverso multiplicativo
     * @see is_prime() para verificar si ℤ/Bℤ es un cuerpo
     */
    constexpr bool is_unit() const noexcept {
      if constexpr (is_prime()) {
        if (!is_0()) { return true; } 
        else { return false; }
      } else {
        if (is_1()) return true;
        else if (std::gcd(B, m_d) != ui_1()) return false;
        else return true;
      }
    }

    /**
     * @brief Determina si el dígito es divisor de cero en el anillo ℤ/Bℤ
     * @return true si existe elemento no nulo que multiplicado da 0
     * 
     * @details TEORÍA DE ANILLOS: Un elemento a ∈ ℤ/Bℤ es divisor de cero
     *          si existe b ≠ 0 en ℤ/Bℤ tal que a×b ≡ 0 (mod B).
     * 
     *          CRITERIO:
     *          - Si B es primo: solo 0 es divisor de cero (cuerpo GF(B))
     *          - Si B compuesto: a es divisor de 0 ⟺ gcd(a, B) > 1 o a = 0
     * 
     * @note PROPIEDAD: En ℤ/Bℤ, is_0_divisor() ≡ !is_unit() (excepto 0)
     * @note 0 es siempre divisor de cero (0×b = 0 para todo b)
     * 
     * @example En ℤ/6ℤ: 2 es divisor de 0 porque 2×3 ≡ 0 (mod 6)
     */
    constexpr bool is_0_divisor() const noexcept {
      if constexpr (is_prime()) {
        if (is_0()) { return true; } 
        else { return false; }
      } else {
        if (is_0()) return true;
        else if (std::gcd(B, m_d) != ui_1()) return true;
        else return false;
      }
    }

    /**
     * @brief Calcula el inverso multiplicativo del dígito si es una unidad
     * @return Dígito b tal que (*this) × b ≡ 1 (mod B)
     * @note Si no es unidad, devuelve dig_0() (comportamiento por defecto)
     * 
     * @details ALGORITMO ACTUAL (INEFICIENTE): Búsqueda exhaustiva entre
     *          todos los candidatos que son unidades, hasta encontrar uno
     *          que multiplicado por (*this) dé 1 módulo B.
     * 
     * @note OPTIMIZACIONES PRESENTES:
     *       - Si m_d == 1: inverso es 1 → O(1)
     *       - Si m_d == B-1: inverso es B-1 (porque (B-1)² ≡ 1 (mod B)) → O(1)
     *       - Caso general: itera sobre [2, B-1] comprobando is_unit() → O(B × log B)
     * 
     * @warning INEFICIENCIA CRÍTICA (caso general):
     *          Complejidad O(B × log B) donde:
     *          - Itera hasta B-1 candidatos (bucle for)
     *          - Cada iteración llama is_unit() → std::gcd() → O(log B)
     *          - Cada iteración hace multiplicación y comparación
     *          
     *          IMPACTO:
     *          - B = 256: ~2000 operaciones
     *          - B = 65536: ~1 millón de operaciones
     *          - B = 2³²: IMPRACTICABLE (billones de operaciones)
     * 
     * @warning BUG POTENCIAL en línea "for (dig_t index(2); !is_Bm1(); ++index)":
     *          La condición !is_Bm1() NO verifica index, verifica *this.
     *          Debería ser "!index.is_Bm1()" → bucle infinito si m_d == B-1.
     *          Actualmente funciona por casualidad gracias al check previo.
     * 
     * @warning MEJORAS PROPUESTAS (urgentes para bases grandes):
     *          1. ALGORITMO EXTENDIDO DE EUCLIDES: O(log B)
     *             Calcula inverso directamente sin búsqueda:
     *             ```cpp
     *             // Extended GCD: ax + By = gcd(a,B) = 1
     *             // Devuelve x tal que ax ≡ 1 (mod B)
     *             ```
     * 
     *          2. TABLA PRECALCULADA (compile-time):
     *             ```cpp
     *             static constexpr std::array<uint_t, B> inv_table = 
     *                 generate_inverse_table<B>();
     *             return dig_t(inv_table[m_d]);
     *             ```
     *             Complejidad: O(1) runtime, O(B log B) compile-time
     * 
     *          3. EXPONENCIACIÓN MODULAR (si B es primo):
     *             Por Pequeño Teorema de Fermat: a^(B-2) ≡ a⁻¹ (mod B)
     *             Complejidad: O(log B) usando exponenciación rápida
     * 
     * @note PRECONDICIÓN IMPLÍCITA: is_unit() debería ser true
     * @note POSTCONDICIÓN: Si is_unit(), entonces (*this) * mult_inv() == 1
     * 
     * @example En ℤ/7ℤ: mult_inv() de 3 es 5 porque 3×5 = 15 ≡ 1 (mod 7)
     * @example En ℤ/10ℤ: 2 no tiene inverso → devuelve 0
     * @example En ℤ/256ℤ: Puede tardar ~2000 operaciones en caso general
     * 
     * @see is_unit() para verificar si existe inverso válido
     */
    constexpr dig_t mult_inv() const noexcept {
      if (is_unit()) {
        if (is_1()) return dig_1();
        else if (is_Bm1()) return dig_max();
        else {
          // OPTIMIZACIÓN: Comprobar gcd inline en lugar de llamar index.is_unit()
          // Evita repetir todo el ciclo for de is_unit() en cada iteración
          for (dig_t index(2); !index.is_Bm1(); ++index) {
            // index es unidad si gcd(B, index.m_d) == 1
            if constexpr (is_prime()) {
              // Si B es primo, todos los índices [2, B-1] son unidades
              if (((*this) * index).is_1()) return index;
            } else {
              // Si B es compuesto, verificar gcd inline
              if (std::gcd(B, index.m_d) == ui_1()) {
                if (((*this) * index).is_1()) return index;
              }
            }
          }
          return dig_0();
        }
      } else return dig_0();
    }

    /**
     * @brief Calcula el acarreo (carry) de la suma de dos dígitos sin overflow
     * @param arg_1 Primer sumando (dígito en base B)
     * @param arg_2 Segundo sumando (dígito en base B)
     * @return dig_0() si arg_1 + arg_2 < B, dig_1() si arg_1 + arg_2 >= B
     * 
     * @details ALGORITMO DE PREVENCIÓN DE OVERFLOW:
     *          Determina si arg_1 + arg_2 >= B SIN calcular la suma directamente,
     *          evitando overflow cuando arg_1 + arg_2 > MAX(uint_t).
     * 
     *          ESTRATEGIA POR CASOS (compile-time):
     *          1. Si B <= middle_max<uint_t>() (~√MAX(uint_t)):
     *             - Suma directa segura, no hay riesgo de overflow
     *             - Optimización por paridad de B (par/impar)
     *          
     *          2. Si B > middle_max<uint_t>():
     *             - Usa tipo superior (sig_uint_t) para cálculos
     *             - Optimización por paridad de B (par/impar)
     * 
     *          CASOS SEGÚN PARIDAD DE B:
     *          
     *          **B PAR** (B = 2k):
     *          - Si ambos args < B/2: suma < B → carry = 0
     *          - Si ambos args >= B/2: suma >= B → carry = 1
     *          - Caso límite: verificar arg_1 >= B - arg_2
     *          
     *          **B IMPAR** (B = 2k+1):
     *          - Bdiv2_1 = B/2 (truncado)
     *          - Bdiv2_2 = B/2 + 1
     *          - Condiciones simétricas para ambos argumentos:
     *            * (arg_1 < Bdiv2_1 && arg_2 < Bdiv2_2) o viceversa → carry = 0
     *            * (arg_1 >= Bdiv2_1 && arg_2 >= Bdiv2_2) o viceversa → carry = 1
     *          - Caso límite: verificar arg_1 >= B - arg_2
     * 
     * @note COMPLEJIDAD: O(1) - todas las ramas resueltas en compile-time
     * @note SEGURIDAD: Garantiza que NUNCA ocurre overflow en uint_t
     * @note OPTIMIZACIÓN: if constexpr elimina ramas no usadas en compile-time
     * 
     * @example sum_carry(dig_t<10>(6), dig_t<10>(7)) → dig_1() (6+7=13 >= 10)
     * @example sum_carry(dig_t<10>(3), dig_t<10>(5)) → dig_0() (3+5=8 < 10)
     * @example sum_carry(dig_t<256>(200), dig_t<256>(100)) → dig_1() (300 >= 256)
     * 
     * @see sum_digs() para obtener el dígito de la suma (sin carry)
     */
    constexpr static dig_t sum_carry(dig_t arg_1, dig_t arg_2) noexcept {
      if constexpr (B <= type_traits::middle_max<uint_t>()) { // B <= MID
        if constexpr ((B % 2) == 0) { /// B PAR
          constexpr uint_t Bdiv2{B / 2};
          /// NOS ASEGURAMOS QUE ENTRE LOS DOS ARGUMENTOS 
          /// NO SUPERAMOS EL MÁXIMO DEL TIPO UINT_T
          if ((arg_1() < Bdiv2) && (arg_2() < Bdiv2)) { return dig_0(); } /// NO HAY ACARREO
          else if ((arg_1() >= Bdiv2) && (arg_2() >= Bdiv2)) { return dig_1(); } /// HAY ACARREO
          else if (arg_1() >= B - arg_2()) { return dig_1(); } /// HAY ACARREO (CONDICIÓN LÍMITE)
          else { return dig_0(); } /// NO HAY ACARREO (CONDICIÓN LÍMITE)
        } else { // B IMPAR
          constexpr uint_t Bdiv2_1{B / 2};
          constexpr uint_t Bdiv2_2{(B / 2) + 1};
          /// NOS ASEGURAMOS QUE ENTRE LOS DOS ARGUMENTOS 
          /// NO SUPERAMOS EL MÁXIMO DEL TIPO UINT_T
          /// SON DOS CONDICIONES SIMÉTRICAS
          if (((arg_1() < Bdiv2_1) && (arg_2() < Bdiv2_2)) ||
              ((arg_1() < Bdiv2_2) && (arg_2() < Bdiv2_1)))  /// NO HAY ACARREO 
            { return dig_0(); }
          else if (((arg_1() >= Bdiv2_1) && (arg_2() >= Bdiv2_2)) ||
                   ((arg_1() >= Bdiv2_2) && (arg_2() >= Bdiv2_1)))  /// HAY ACARREO 
            { return dig_1(); }
          else if (arg_1() >= B - arg_2()) { return dig_1(); } /// HAY ACARREO (CONDICIÓN LÍMITE)
          else { return dig_0(); } /// NO HAY ACARREO (CONDICIÓN LÍMITE)
        }
      } else { // B > MID
        if constexpr ((B % 2) == 0) { // B PAR
          constexpr sig_uint_t Bdiv2{B / 2};
          /// NOS ASEGURAMOS QUE ENTRE LOS DOS ARGUMENTOS 
          /// NO SUPERAMOS EL MÁXIMO DEL TIPO UINT_T
          if ((arg_1() < Bdiv2) && (arg_2() < Bdiv2)) { return dig_0(); } /// NO HAY ACARREO
          else if ((arg_1() >= Bdiv2) && (arg_2() >= Bdiv2)) { return dig_1(); } /// HAY ACARREO
          else if (arg_1() >= B - arg_2()) { return dig_1(); } /// HAY ACARREO (CONDICIÓN LÍMITE)
          else { return dig_0(); } /// NO HAY ACARREO (CONDICIÓN LÍMITE)
        }
        else { // B IMPAR
          constexpr sig_uint_t Bdiv2_1{B / 2};
          constexpr sig_uint_t Bdiv2_2{(B / 2) + 1};
            /// NOS ASEGURAMOS QUE ENTRE LOS DOS ARGUMENTOS
            /// NO SUPERAMOS EL MÁXIMO DEL TIPO UINT_T
            /// SON DOS CONDICIONES SIMÉTRICAS
          if (((arg_1() < Bdiv2_1) && (arg_2() < Bdiv2_2)) ||
              ((arg_1() < Bdiv2_2) && (arg_2() < Bdiv2_1)))  /// NO HAY ACARREO
            { return dig_0(); }
          else if (((arg_1() >= Bdiv2_1) && (arg_2() >= Bdiv2_2)) ||
                   ((arg_1() >= Bdiv2_2) && (arg_2() >= Bdiv2_1)))  /// HAY ACARREO
            { return dig_1(); }
          else if (arg_1() >= B - arg_2()) { return dig_1(); } /// HAY ACARREO (CONDICIÓN LÍMITE)
          else { return dig_0(); } /// NO HAY ACARREO (CONDICIÓN LÍMITE)
        }
      }
    }

    // =========================================================================
    // OPERADORES LÓGICOS BITWISE - Interpretación como MIN/MAX
    // =========================================================================
    // NOTA: Como dig_t es un tipo atómico (no tiene bits individuales),
    // los operadores & y | se interpretan como MIN y MAX respectivamente.
    // 
    // EQUIVALENCIA CON LÓGICA BOOLEANA (cuando dígitos ∈ {0,1}):
    // - a & b ≡ min(a,b) ≡ a AND b
    // - a | b ≡ max(a,b) ≡ a OR b
    // 
    // Para dígitos generales en [0, B-1], min/max es la generalización natural.
    // =========================================================================

    /**
     * @brief Operador AND bitwise - devuelve el mínimo de dos dígitos
     * @param arg Segundo operando
     * @return min(*this, arg)
     * 
     * @details INTERPRETACIÓN: Como dig_t es atómico (sin bits individuales),
     *          el AND bitwise se interpreta como la operación MIN.
     * 
     *          JUSTIFICACIÓN: Para dígitos booleanos {0,1}:
     *          - 0 & 0 = 0 = min(0,0)
     *          - 0 & 1 = 0 = min(0,1)
     *          - 1 & 0 = 0 = min(1,0)
     *          - 1 & 1 = 1 = min(1,1)
     * 
     *          Para dígitos generales [0, B-1], min es la generalización natural.
     * 
     * @note EQUIVALENCIA: & y && son idénticos (dig_t es atómico, no tiene bits)
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<10>(3) & dig_t<10>(5) → dig_t<10>(3)
     * @example dig_t<10>(3) & dig_t<10>(5) → dig_t<10>(3)
     * @example dig_t<2>(1) & dig_t<2>(1) → dig_t<2>(1) (AND booleano)
     */
    constexpr 
    dig_t operator&(const dig_t &arg) const noexcept { 
        return (((*this) <= arg) ? (*this) : arg); 
    }
    
    /**
     * @brief Operador AND lógico - idéntico a operator& para dig_t
     * @param arg Segundo operando
     * @return min(*this, arg)
     * 
     * @note EQUIVALENCIA: && y & son idénticos porque dig_t es atómico
     * @note En tipos con bits, && evaluaría como booleano y tendría short-circuit
     * @note Para dig_t, ambos son simplemente la operación MIN
     * 
     * @see operator& para documentación completa
     */
    constexpr 
    dig_t operator&&(const dig_t &arg) const noexcept { 
        return (((*this) <= arg) ? (*this) : arg); 
    }
    
    /**
     * @brief Operador AND bitwise con asignación - asigna el mínimo
     * @param arg Segundo operando
     * @return Referencia a *this después de asignar min(*this, arg)
     * 
     * @details EFECTO: *this = min(*this, arg)
     * 
     * @note OPTIMIZACIÓN: Solo asigna si arg < *this (evita escritura redundante)
     * @note EQUIVALENCIA: *this &= arg es equivalente a *this = *this & arg
     * 
     * @example dig_t<10> d(7); d &= dig_t<10>(5); // d = 5
     */
    constexpr 
    const dig_t &operator&=(dig_t arg) noexcept {
      if (arg < (*this)) (*this) = arg;
      return (*this);
    }
    
    /**
     * @brief Operador OR bitwise - devuelve el máximo de dos dígitos
     * @param arg Segundo operando
     * @return max(*this, arg)
     * 
     * @details INTERPRETACIÓN: Como dig_t es atómico (sin bits individuales),
     *          el OR bitwise se interpreta como la operación MAX.
     * 
     *          JUSTIFICACIÓN: Para dígitos booleanos {0,1}:
     *          - 0 | 0 = 0 = max(0,0)
     *          - 0 | 1 = 1 = max(0,1)
     *          - 1 | 0 = 1 = max(1,0)
     *          - 1 | 1 = 1 = max(1,1)
     * 
     *          Para dígitos generales [0, B-1], max es la generalización natural.
     * 
     * @note EQUIVALENCIA: | y || son idénticos (dig_t es atómico, no tiene bits)
     * @note COMPLEJIDAD: O(1)
     * @note DUALIDAD: OR es dual de AND bajo la transformación min ↔ max
     * 
     * @example dig_t<10>(3) | dig_t<10>(5) → dig_t<10>(5)
     * @example dig_t<2>(0) | dig_t<2>(1) → dig_t<2>(1) (OR booleano)
     */
    constexpr 
    dig_t operator|(const dig_t &arg) const noexcept { 
        return (((*this) >= arg) ? (*this) : arg); 
    }
    
    /**
     * @brief Operador OR lógico - idéntico a operator| para dig_t
     * @param arg Segundo operando
     * @return max(*this, arg)
     * 
     * @note EQUIVALENCIA: || y | son idénticos porque dig_t es atómico
     * @note En tipos con bits, || evaluaría como booleano y tendría short-circuit
     * @note Para dig_t, ambos son simplemente la operación MAX
     * 
     * @see operator| para documentación completa
     */
    constexpr 
    dig_t operator||(const dig_t &arg) const noexcept { 
        return (((*this) >= arg) ? (*this) : arg); 
    }

    /**
     * @brief Operador OR bitwise con asignación - asigna el máximo
     * @param arg Segundo operando
     * @return Referencia a *this después de asignar max(*this, arg)
     * 
     * @details EFECTO: *this = max(*this, arg)
     * 
     * @note OPTIMIZACIÓN: Solo asigna si arg > *this (evita escritura redundante)
     * @note EQUIVALENCIA: *this |= arg es equivalente a *this = *this | arg
     * @note DUALIDAD: |= es dual de &= (max vs min)
     * 
     * @example dig_t<10> d(3); d |= dig_t<10>(5); // d = 5
     */
    constexpr 
    const dig_t &operator|=(dig_t arg) noexcept {
      if (arg > (*this)) (*this) = arg;
      return (*this);
    }

    /**
     * @brief Operador de exponenciación con asignación (*this ^= exp)
     * @tparam UIntType Tipo entero sin signo del exponente
     * @param exp Exponente (≥ 0)
     * @return Referencia a *this después de elevar a la potencia exp
     * 
     * @details ALGORITMO: Exponenciación binaria rápida (exponentiación por cuadrados)
     *          Basado en la representación binaria del exponente:
     *          exp = ∑ bᵢ·2ⁱ  donde bᵢ ∈ {0,1}
     * 
     *          PSEUDOCÓDIGO:
     *          resultado = 1
     *          base = *this
     *          mientras (exp > 0):
     *              si (exp es impar):  resultado *= base
     *              base *= base       // elevar base al cuadrado
     *              exp /= 2            // dividir exponente por 2
     * 
     *          EJEMPLO: 3^13 en representación binaria
     *          13₁₀ = 1101₂ = 2³ + 2² + 2⁰
     *          3^13 = 3^(8+4+1) = 3^8 · 3^4 · 3^1
     *          Iteraciones:
     *          - exp=13 (impar): resultado = 3,      base = 3² = 9,    exp = 6
     *          - exp=6  (par):   resultado = 3,      base = 9² = 81,   exp = 3
     *          - exp=3  (impar): resultado = 3·81,   base = 81² = ..., exp = 1
     *          - exp=1  (impar): resultado = 243·..., base = ...,      exp = 0
     * 
     * @note CONVENCIÓN MATEMÁTICA: 0^0 = 1
     *       Esta es la convención adoptada en combinatoria, análisis y álgebra.
     *       Justificación: x^0 = 1 para todo x (incluyendo x=0) por continuidad
     *       del producto vacío y coherencia con reglas de exponentes.
     * 
     * @note COMPLEJIDAD: O(log exp) multiplicaciones módulo B
     *       - Número de iteraciones: ⌊log₂(exp)⌋ + 1
     *       - Ejemplo: exp=1024 = 2^10 → solo 10 iteraciones
     *       - Comparado con algoritmo ingenuo O(exp): 1024 iteraciones
     *       - Mejora: 100× más rápido para exp=1024
     * 
     * @note OPTIMIZACIÓN: Casos especiales exp=0,1,2 tratados directamente
     * 
     * @example dig_t<10>(2) ^= 3   // 2^3 = 8 (3 iteraciones)
     * @example dig_t<10>(0) ^= 0   // 0^0 = 1 (convención)
     * @example dig_t<7>(5) ^= 13   // 5^13 ≡ ? (mod 7), 4 iteraciones
     * @example dig_t<10>(2) ^= 10  // 2^10 = 1024, 4 iteraciones vs 10 ingenuo
     */
    template <type_traits::unsigned_integral_c UIntType> constexpr 
    const dig_t &operator^=(UIntType exp) noexcept {
      dig_t &cthis{*this};
      
      // Casos especiales optimizados
      if (exp == 0) {
        cthis = dig_1();
        return (cthis);
      } else if (exp == 1) {
        return (cthis);
      } else if (exp == 2) {
        cthis *= cthis;
        return (cthis);
      }
      
      // Exponenciación binaria rápida para exp >= 3
      dig_t resultado{dig_1()};  // Acumulador de resultado
      dig_t base{*this};          // Base que se eleva al cuadrado iterativamente
      
      while (exp > 0) {
        if (exp & 1) {            // Si exp es impar (bit menos significativo = 1)
          resultado *= base;      // Multiplicar resultado por base actual
        }
        base *= base;             // Elevar base al cuadrado
        exp >>= 1;                // Dividir exp por 2 (desplazar bits a derecha)
      }
      
      cthis = resultado;
      return (cthis);
    }

    /**
     * @brief Operador de exponenciación (*this ^ exp)
     * @tparam UIntType Tipo entero sin signo del exponente
     * @param exp Exponente (≥ 0)
     * @return Nuevo dig_t con el resultado de elevar *this a la potencia exp
     * 
     * @details Delega en operator^= que usa exponenciación binaria rápida.
     *          Aplica la convención 0^0 = 1 (ver operator^=).
     * 
     * @note CONVENCIÓN: 0^0 = 1
     * @note COMPLEJIDAD: O(log exp) multiplicaciones + copia O(1)
     * @note ALGORITMO: Exponenciación por cuadrados (ver operator^=)
     * 
     * @example dig_t<10>(3) ^ 2   // 3^2 = 9
     * @example dig_t<10>(0) ^ 0   // 0^0 = 1
     * @example dig_t<13>(5) ^ 3   // 5^3 = 125 ≡ 8 (mod 13)
     * @example dig_t<10>(2) ^ 100 // 2^100 en solo ⌊log₂(100)⌋+1 = 8 iteraciones
     */
    template <type_traits::unsigned_integral_c UIntType>
    constexpr dig_t operator^(UIntType exp) const noexcept {
      dig_t cpthis{*this};
      cpthis ^= exp;
      return cpthis;
    }

    /**
     * @section OPERADORES_COMPARACION Operadores de Comparación
     * 
     * @brief Conjunto de operadores para comparar elementos dig_t
     * 
     * @details Los operadores de comparación se dividen en dos categorías:
     * 
     * **1. COMPARACIONES BOOLEANAS (dig_t @ dig_t):**
     *    - operator== : Igualdad
     *    - operator!= : Desigualdad
     *    - operator<  : Menor que
     *    - operator<= : Menor o igual
     *    - operator>  : Mayor que
     *    - operator>= : Mayor o igual
     * 
     * **2. COMPARACIÓN DE TRES VÍAS (C++20):**
     *    - operator<=> : Operador "nave espacial" (spaceship)
     *      * dig_t @ dig_t  → std::strong_ordering
     *      * dig_t @ Int_t  → std::weak_ordering
     * 
     * @note INVARIANTE CRÍTICO: m_d siempre está normalizado (0 ≤ m_d < B)
     *       Por tanto, la comparación se reduce a comparar m_d directamente.
     * 
     * @note SEMÁNTICA: Las comparaciones reflejan el orden en ℤ/Bℤ donde
     *       los representantes canónicos son {0, 1, ..., B-1}.
     * 
     * @note ORDEN: d[a]Bₙ < d[b]Bₙ si y solo si a < b como enteros naturales.
     *       Ejemplo en base 10: d[3]B10 < d[7]B10 porque 3 < 7.
     * 
     * @note COMPLEJIDAD: Todas las comparaciones son O(1) (una sola comparación entera).
     */

    /**
     * @brief Operador de igualdad (dig_t == dig_t)
     * @param a Elemento a comparar
     * @return true si *this y a representan el mismo elemento en ℤ/Bℤ
     * 
     * @details Compara los valores normalizados directamente.
     * 
     * @note PROPIEDAD: Es una relación de equivalencia (reflexiva, simétrica, transitiva).
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<10>{3} == dig_t<10>{3}  // true
     * @example dig_t<10>{3} == dig_t<10>{7}  // false
     */
    constexpr bool operator==(dig_t a) const noexcept { return (a.m_d == m_d); }

    /**
     * @brief Operador de desigualdad (dig_t != dig_t)
     * @param a Elemento a comparar
     * @return true si *this y a representan elementos distintos en ℤ/Bℤ
     * 
     * @note Complemento de operator==: (x != y) ≡ !(x == y)
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<10>{3} != dig_t<10>{7}  // true
     * @example dig_t<10>{5} != dig_t<10>{5}  // false
     */
    constexpr bool operator!=(dig_t a) const noexcept { return (a.m_d != m_d); }

    /**
     * @brief Operador mayor o igual (dig_t >= dig_t)
     * @param a Elemento a comparar
     * @return true si *this ≥ a en el orden de ℤ/Bℤ
     * 
     * @details Compara m_d >= a.m_d (equivalente a a.m_d <= m_d).
     * 
     * @note ORDEN: Orden natural de los representantes canónicos {0,...,B-1}.
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<10>{7} >= dig_t<10>{3}  // true
     * @example dig_t<10>{5} >= dig_t<10>{5}  // true
     * @example dig_t<10>{2} >= dig_t<10>{8}  // false
     */
    constexpr bool operator>=(dig_t a) const noexcept { return (a.m_d <= m_d); }

    /**
     * @brief Operador mayor que (dig_t > dig_t)
     * @param a Elemento a comparar
     * @return true si *this > a en el orden de ℤ/Bℤ
     * 
     * @details Compara m_d > a.m_d (equivalente a a.m_d < m_d).
     * 
     * @note ORDEN ESTRICTO: Antisimétrico y transitivo.
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<10>{7} > dig_t<10>{3}  // true
     * @example dig_t<10>{5} > dig_t<10>{5}  // false
     * @example dig_t<10>{2} > dig_t<10>{8}  // false
     */
    constexpr bool operator>(dig_t a) const noexcept { return (a.m_d < m_d); }

    /**
     * @brief Operador menor o igual (dig_t <= dig_t)
     * @param a Elemento a comparar
     * @return true si *this ≤ a en el orden de ℤ/Bℤ
     * 
     * @details Compara m_d <= a.m_d (equivalente a a.m_d >= m_d).
     * 
     * @note DUAL: Dual de operator>=
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<10>{3} <= dig_t<10>{7}  // true
     * @example dig_t<10>{5} <= dig_t<10>{5}  // true
     * @example dig_t<10>{8} <= dig_t<10>{2}  // false
     */
    constexpr bool operator<=(dig_t a) const noexcept { return (a.m_d >= m_d); }

    /**
     * @brief Operador menor que (dig_t < dig_t)
     * @param a Elemento a comparar
     * @return true si *this < a en el orden de ℤ/Bℤ
     * 
     * @details Compara m_d < a.m_d (equivalente a a.m_d > m_d).
     * 
     * @note DUAL: Dual de operator>
     * @note ORDEN TOTAL: Define un orden total en ℤ/Bℤ.
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<10>{3} < dig_t<10>{7}  // true
     * @example dig_t<10>{5} < dig_t<10>{5}  // false
     * @example dig_t<10>{8} < dig_t<10>{2}  // false
     */
    constexpr bool operator<(dig_t a) const noexcept { return (a.m_d > m_d); }

    /**
     * @brief Operador de comparación de tres vías "nave espacial" (dig_t <=> dig_t)
     * @param rhs Elemento derecho de la comparación
     * @return std::strong_ordering::less, equal, o greater
     * 
     * @details Operador C++20 que reemplaza las 6 comparaciones booleanas.
     *          Retorna std::strong_ordering porque el orden en ℤ/Bℤ es total.
     * 
     * **SEMÁNTICA:**
     * - std::strong_ordering::less    : *this < rhs
     * - std::strong_ordering::equal   : *this == rhs
     * - std::strong_ordering::greater : *this > rhs
     * 
     * @note STRONG ORDERING: Garantiza que (a <=> b == 0) ⇒ (a == b)
     *       A diferencia de weak_ordering, igual implica indistinguible.
     * 
     * @note COMPLEJIDAD: O(1) - dos comparaciones en el peor caso
     * 
     * @note GENERACIÓN AUTOMÁTICA: C++20 sintetiza operator==, !=, <, <=, >, >=
     *       a partir de operator<=> y operator==.
     * 
     * @example auto cmp = dig_t<10>{3} <=> dig_t<10>{7};  // less
     * @example auto cmp = dig_t<10>{5} <=> dig_t<10>{5};  // equal
     * @example auto cmp = dig_t<10>{9} <=> dig_t<10>{2};  // greater
     */
    constexpr 
    std::strong_ordering operator<=>(dig_t rhs) const noexcept {
      const auto lhs_d{m_d};
      const auto rhs_d{rhs.m_d};
      return ((lhs_d < rhs_d) ? std::strong_ordering::less
                              : ((lhs_d > rhs_d) ? std::strong_ordering::greater
                                                 : std::strong_ordering::equal));
    }

    /**
     * @brief Operador de igualdad con entero (dig_t == Int_t)
     * @tparam Int_t Tipo entero (signed o unsigned)
     * @param rhs Entero a comparar (se normaliza módulo B)
     * @return true si *this ≡ rhs (mod B)
     * 
     * @details El entero rhs se normaliza primero a ℤ/Bℤ antes de comparar.
     * 
     * @note NORMALIZACIÓN: rhs se reduce módulo B antes de comparar.
     * @note COMPLEJIDAD: O(1) - normalización + comparación
     * 
     * @example dig_t<10>{3} == 13   // true (13 ≡ 3 mod 10)
     * @example dig_t<10>{7} == 7    // true
     * @example dig_t<10>{5} == 15   // true (15 ≡ 5 mod 10)
     * @example dig_t<7>{2} == -5    // true (-5 ≡ 2 mod 7)
     */
    template <type_traits::integral_c Int_t> constexpr 
    bool operator==(Int_t rhs) noexcept {
      const dig_t &lhs{*this};
      return (lhs.m_d == normaliza<Int_t>(rhs));
    }

    /**
     * @brief Operador de comparación de tres vías con entero (dig_t <=> Int_t)
     * @tparam Int_t Tipo entero (signed o unsigned)
     * @param rhs Entero a comparar (se normaliza módulo B)
     * @return std::weak_ordering::less, equivalent, o greater
     * 
     * @details Retorna std::weak_ordering en lugar de strong_ordering porque
     *          múltiples enteros pueden ser equivalentes módulo B sin ser iguales.
     * 
     * **EJEMPLO DE WEAK ORDERING:**
     * En base 10: 3, 13, 23, -7, etc. son todos equivalentes a dig_t<10>{3}
     * pero no son el mismo entero → weak_ordering::equivalent
     * 
     * **DIFERENCIA STRONG vs WEAK:**
     * - Strong: (a <=> b == 0) ⇒ a y b son indistinguibles
     * - Weak:   (a <=> b == 0) ⇒ a y b son equivalentes (pero pueden ser distintos)
     * 
     * @note WEAK ORDERING: 13 <=> dig_t<10>{3} == equivalent (no equal)
     * @note COMPLEJIDAD: O(1) - normalización + dos comparaciones
     * 
     * @example dig_t<10>{3} <=> 13   // equivalent (13 ≡ 3 mod 10)
     * @example dig_t<10>{2} <=> 8    // less (2 < 8)
     * @example dig_t<10>{9} <=> 5    // greater (9 > 5)
     */
    template <type_traits::integral_c Int_t> constexpr 
    std::weak_ordering operator<=>(Int_t rhs) const noexcept {
      const dig_t &lhs{*this};
      const uint_t rhs_B{normaliza<Int_t>(rhs)};
      return ((lhs() < rhs_B)   ? std::weak_ordering::less
                                : (lhs() > rhs_B) ? std::weak_ordering::greater
                                                  : std::weak_ordering::equivalent);
    }

    /**
     * @section OPERADORES_ARITMETICOS_ASIGNACION Operadores Aritméticos con Asignación
     * 
     * @brief Operadores compuestos que modifican *this con operación aritmética
     * 
     * @details Todos implementan aritmética modular en ℤ/Bℤ:
     * - +=, -=, *=, /=, %=
     * - Versiones para dig_t y tipos enteros (Int_t)
     * - Optimizaciones específicas según tamaño de B
     * - Prevención de overflow con tipos mayores cuando es necesario
     */

    /**
     * @brief Operador de suma con asignación (dig_t += dig_t)
     * @param arg Sumando (elemento de ℤ/Bℤ)
     * @return Referencia a *this después de la suma modular
     * 
     * @details Realiza la suma modular: (*this + arg) mod B
     * 
     * **ALGORITMO:**
     * - Si B < middle_max: suma directa con chequeo de overflow
     * - Si B ≥ middle_max: usa tipo mayor (sig_uint_t) para evitar overflow
     * 
     * **OPTIMIZACIÓN:**
     * - Evita operación módulo (%) costosa
     * - Usa resta condicional: if (sum >= B) sum -= B
     * - Complejidad: O(1) - dos sumas y una comparación
     * 
     * @note NEUTRO ADITIVO: a += dig_t(0) no modifica a
     * @note OVERFLOW: La suma nunca desborda gracias al chequeo
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<256>{200} += dig_t<256>{100}
     *          → 200 + 100 = 300 ≥ 256
     *          → 300 - 256 = 44
     * 
     * @see operator+(dig_t) que delega en este operador
     */
    constexpr 
    const dig_t &operator+=(dig_t arg) noexcept {
      dig_t &cthis{*this};
      if constexpr (B < type_traits::middle_max<uint_t>()) {
        m_d += arg.m_d;
        if (m_d >= B) m_d -= B;
        return (cthis);
      } else {
        sig_uint_t tmp{m_d};
        tmp += (arg.m_d);
        if (tmp >= B) tmp -= B;
        m_d = static_cast<uint_t>(tmp);
        return (cthis);
      }
    }

    /**
     * @brief Operador de suma con asignación desde entero (dig_t += Int_t)
     * @tparam Int_t Tipo entero (signed o unsigned)
     * @param arg Entero a sumar (se normaliza módulo B)
     * @return Referencia a *this después de la suma modular
     * 
     * @details Normaliza arg módulo B y aplica suma modular.
     * 
     * @note El entero se normaliza primero: arg' = arg mod B
     * @note Luego aplica: *this = (*this + arg') mod B
     * @note Usa tipo mayor para prevenir overflow según tamaño de B
     * 
     * @example dig_t<256>{200} += 356
     *          → 356 ≡ 100 (mod 256)
     *          → 200 + 100 = 300 → 44 (mod 256)
     */
    template <type_traits::integral_c Int_t> constexpr 
    const dig_t &operator+=(Int_t arg) noexcept {
      if constexpr (B >= type_traits::middle_max<uint_t>()) {
        const sig_uint_t arg1{normaliza<Int_t>(arg)};
        sig_uint_t arg2{m_d};
        arg2 += arg1;
        if (arg2 >= static_cast<Int_t>(B)) arg2 -= static_cast<Int_t>(B);
        m_d = static_cast<uint_t>(arg2);
        return (*this);
      } else {
        const uint_t arg1{normaliza<Int_t>(arg)};
        uint_t arg2{m_d};
        arg2 += arg1;
        if (arg2 >= static_cast<Int_t>(B)) arg2 -= static_cast<Int_t>(B);
        m_d = static_cast<uint_t>(arg2);
        return (*this);
      }
    }

    /**
     * @brief Operador de resta con asignación (dig_t -= dig_t)
     * @param arg Sustraendo (elemento de ℤ/Bℤ)
     * @return Referencia a *this después de la resta modular
     * 
     * @details Realiza la resta modular: (*this - arg) mod B
     * 
     * **ALGORITMO:**
     * - Usa tipo con signo (sig_sint_t) para manejar diferencias negativas
     * - Si resultado < 0: suma B para obtener el representante positivo
     * - Complejidad: O(1)
     * 
     * **MANEJO DE NEGATIVOS:**
     * - -1 mod B = B - 1
     * - -k mod B = B - k (para 0 < k < B)
     * 
     * @note NEUTRO: a -= dig_t(0) no modifica a
     * @note INVERSO: a -= a resulta en 0
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<256>{50} -= dig_t<256>{100}
     *          → 50 - 100 = -50
     *          → -50 + 256 = 206
     * 
     * @example dig_t<10>{3} -= dig_t<10>{3}
     *          → 3 - 3 = 0
     * 
     * @see operator-(dig_t) que delega en este operador
     */
    constexpr 
    const dig_t &operator-=(dig_t arg) noexcept {
      sig_sint_t cp_dm{m_d};
      cp_dm -= arg.m_d;
      if (cp_dm < 0) cp_dm += ssi_B();
      m_d = cp_dm;
      return (*this);
    }

    /**
     * @brief Operador de resta con asignación desde entero (dig_t -= Int_t)
     * @tparam Int_t Tipo entero (signed o unsigned)
     * @param arg Entero a restar (se normaliza módulo B)
     * @return Referencia a *this después de la resta modular
     * 
     * @details Normaliza arg módulo B y aplica resta modular.
     * 
     * @note El entero se normaliza primero: arg' = arg mod B
     * @note Luego aplica: *this = (*this - arg') mod B
     * @note Maneja correctamente enteros negativos
     * 
     * @example dig_t<256>{50} -= 350
     *          → 350 ≡ 94 (mod 256)
     *          → 50 - 94 = -44 + 256 = 212
     */
    template <type_traits::integral_c Int_t> constexpr
    const dig_t &operator-=(Int_t arg) noexcept {
      sig_sint_t tmp{normaliza<Int_t>(arg)};
      sig_sint_t este{m_d};
      este -= tmp;
      if (este < static_cast<sig_sint_t>(0)) este += ssi_B();
      m_d = este;
      return (*this);
    }

    /**
     * @brief Operador de multiplicación con asignación (dig_t *= dig_t)
     * @param arg Factor (elemento de ℤ/Bℤ)
     * @return Referencia a *this después de la multiplicación modular
     * 
     * @details Realiza la multiplicación modular: (*this × arg) mod B
     * 
     * **ALGORITMO CON PREVENCIÓN DE OVERFLOW:**
     * - Si B < √(MAX_UINT_T): multiplicación directa sin riesgo de overflow
     * - Si B ≥ √(MAX_UINT_T): usa tipo mayor (sig_uint_t) para evitar overflow
     * 
     * **OPTIMIZACIÓN:**
     * - Determina en compile-time si B es "pequeña" para evitar cálculos costosos
     * - Para B pequeña: m_d *= arg.m_d; m_d %= B (dos operaciones)
     * - Para B grande: usa tipo doble de tamaño
     * 
     * @note NEUTRO MULTIPLICATIVO: a *= dig_t(1) no modifica a
     * @note ABSORCIÓN: a *= dig_t(0) resulta en 0
     * @note COMPLEJIDAD: O(1) - multiplicación y módulo son O(1) en hardware
     * 
     * @example dig_t<256>{100} *= dig_t<256>{50}
     *          → 100 × 50 = 5000
     *          → 5000 mod 256 = 136
     * 
     * @example dig_t<10>{7} *= dig_t<10>{8}
     *          → 7 × 8 = 56 mod 10 = 6
     * 
     * @see operator*(dig_t) que delega en este operador
     */
    constexpr 
    const dig_t &operator*=(dig_t arg) noexcept {
      if constexpr (B < type_traits::sqrt_max<uint_t>()) { /// NO OVERFLOW (B SMALL)
        m_d *= arg.m_d;
        m_d %= B;
        return (*this);
      } else {  /// WOULD BE OVERFLOW (B BIGGER THAN SQRT(MAX_UINT_T))
        sig_uint_t tmp{m_d};
        tmp *= arg.m_d;
        tmp %= static_cast<sig_uint_t>(B);
        m_d = static_cast<uint_t>(tmp);
        return (*this);
      }
    }

    /**
     * @brief Operador de multiplicación con asignación desde entero (dig_t *= Int_t)
     * @tparam Int_t Tipo entero (signed o unsigned)
     * @param arg Entero factor (se normaliza módulo B)
     * @return Referencia a *this después de la multiplicación modular
     * 
     * @details Normaliza arg módulo B y aplica multiplicación modular.
     *          Maneja correctamente tipos signed/unsigned de diferentes tamaños.
     * 
     * **CASOS MANEJADOS:**
     * - Int_t signed y sizeof(Int_t) > sizeof(uint_t): usa sig_SInt_for_SInt_t
     * - Int_t signed y sizeof(Int_t) ≤ sizeof(uint_t): usa sig_sint_t
     * - Int_t unsigned y sizeof(Int_t) > sizeof(uint_t): usa sig_UInt_for_UInt_t
     * - Int_t unsigned y sizeof(Int_t) ≤ sizeof(uint_t): usa sig_uint_t
     * 
     * @note Prevención de overflow según tamaño de tipos
     * @note Maneja correctamente enteros negativos
     * 
     * @example dig_t<256>{100} *= 500
     *          → 500 ≡ 244 (mod 256)
     *          → 100 × 244 = 24400 mod 256 = 80
     */
    template <type_traits::integral_c Int_t> constexpr
    const dig_t &operator*=(Int_t arg) noexcept {
      const Int_t tmp{normaliza<Int_t>(arg)};
      if constexpr (std::is_signed_v<Int_t>) { /// TIPO CON SIGNO
        if constexpr (sizeof(Int_t) > sizeof(uint_t)) { /// TIPO ENTERO MAYOR QUE UINT_T
          using SIG2_SINT_T = type_traits::sig_SInt_for_SInt_t<Int_t>;
          const SIG2_SINT_T norm_arg{tmp};
          SIG2_SINT_T este{m_d};
          este *= norm_arg;
          este %= static_cast<SIG2_SINT_T>(B);
          m_d = static_cast<uint_t>(este);
          return (*this);
        } else { /// TIPO ENTERO MENOR O IGUAL QUE UINT_T
          const sig_sint_t norm_arg{tmp};
          sig_sint_t este{m_d};
          este *= norm_arg;
          este %= static_cast<sig_sint_t>(B);
          m_d = static_cast<uint_t>(este);
          return (*this);
        }
      } else { /// TIPO SIN SIGNO
        if constexpr (sizeof(Int_t) > sizeof(uint_t)) { /// TIPO ENTERO MAYOR QUE UINT_T
          using SIG2_UINT_T = type_traits::sig_UInt_for_UInt_t<Int_t>;
          const SIG2_UINT_T norm_arg{tmp};
          SIG2_UINT_T este{m_d};
          este *= norm_arg;
          este %= static_cast<SIG2_UINT_T>(B);
          m_d = static_cast<uint_t>(este);
          return (*this);
        } else { /// TIPO ENTERO MENOR O IGUAL QUE UINT_T
          const sig_uint_t norm_arg{tmp};
          sig_uint_t este{m_d};
          este *= norm_arg;
          este %= static_cast<sig_uint_t>(B);
          m_d = static_cast<uint_t>(este);
          return (*this);
        }
      }
    }

    /**
     * @brief Operador de división con asignación (dig_t /= dig_t)
     * @param arg Divisor (debe ser unidad en ℤ/Bℤ)
     * @return Referencia a *this después de la división modular
     * 
     * @details En aritmética modular, la división se define como:
     * 
     *    a / b (mod B) ≡ a × b⁻¹ (mod B)
     * 
     * donde b⁻¹ es el inverso multiplicativo de b módulo B.
     * 
     * **SEMÁNTICA:**
     * - Si b es unidad (gcd(b, B) = 1): calcula a × b⁻¹
     * - Si b no es unidad: operación no definida en teoría de anillos
     * - Si b = 0: operación indefinida (no se modifica *this)
     * 
     * **ALGORITMO:**
     * 1. Verificar si arg es unidad (gcd(arg, B) = 1)
     * 2. Si es unidad: calcular inverso b⁻¹ y hacer *this *= b⁻¹
     * 3. Si no es unidad: no hacer nada (división no válida)
     * 
     * @note PRECONDICIÓN: arg debería ser unidad para división válida
     * @note DIVISIÓN POR 0: Si arg = 0, no se modifica *this
     * @note COMPLEJIDAD: O(B × log B) por el cálculo de mult_inv()
     * 
     * @warning DIVISIÓN NO ESTÁNDAR: En bases no primas, no todos los
     *          elementos tienen inverso. Por ejemplo en ℤ/256ℤ, solo
     *          los impares son unidades.
     * 
     * @warning MEJORA RECOMENDADA: Precalcular tabla de inversos O(B)
     *          una vez, luego acceso O(1) en cada división.
     * 
     * @example Base 257 (primo): dig_t<257>{10} /= dig_t<257>{3}
     *          → 10 × 3⁻¹ ≡ 10 × 86 ≡ 860 ≡ 89 (mod 257)
     *          Verificación: 89 × 3 = 267 ≡ 10 (mod 257) ✓
     * 
     * @example Base 256: dig_t<256>{10} /= dig_t<256>{5}
     *          → 10 × 5⁻¹ ≡ 10 × 205 ≡ 2050 ≡ 2 (mod 256)
     *          Verificación: 2 × 5 = 10 ✓
     * 
     * @example Base 256 (no unidad): dig_t<256>{10} /= dig_t<256>{4}
     *          → 4 no es unidad (par), división no válida
     *          → *this permanece sin cambios
     * 
     * @see mult_inv() para el algoritmo de inversión
     * @see is_unit() para verificar si un elemento es unidad
     */
    constexpr 
    const dig_t &operator/=(dig_t arg) noexcept {
      // División modular: a / b ≡ a × b⁻¹ (mod B)
      if (arg.is_unit()) {
        dig_t inv = arg.mult_inv();
        (*this) *= inv;
      }
      // Si arg no es unidad o es 0, no modificamos *this
      return (*this);
    }

    /**
     * @brief Operador de división con asignación desde entero (dig_t /= Int_t)
     * @tparam Int_t Tipo entero (signed o unsigned)
     * @param arg Divisor entero (se normaliza módulo B, debe ser unidad)
     * @return Referencia a *this después de la división modular
     * 
     * @details Normaliza arg módulo B y delega en operator/=(dig_t).
     * 
     * @note El entero se normaliza primero: arg' = arg mod B
     * @note Luego aplica: *this = (*this) × (arg')⁻¹
     * 
     * @example dig_t<257>{10} /= 260
     *          → 260 ≡ 3 (mod 257)
     *          → 10 / 3 ≡ 10 × 86 ≡ 89 (mod 257)
     */
    template <type_traits::integral_c Int_t> constexpr
    const dig_t &operator/=(Int_t arg) noexcept {
      uint_t cparg{normaliza<Int_t>(arg)};
      dig_t tmp{cparg};
      (*this) /= tmp;
      return (*this);
    }

    /**
     * @brief Operador de módulo con asignación (dig_t %= dig_t)
     * @param arg Divisor (módulo)
     * @return Referencia a *this después del cálculo del resto
     * 
     * @details Calcula el resto de la división entera: *this mod arg
     * 
     * **SEMÁNTICA:**
     * - a % b devuelve el resto cuando a se divide por b
     * - Resultado siempre en rango [0, b-1]
     * - Si b = 0: no se modifica *this (división por 0 evitada)
     * 
     * **DIFERENCIA CON DIVISIÓN MODULAR:**
     * - operator%= : resto de división entera (a mod b dentro de ℤ/Bℤ)
     * - operator/= : división modular (a × b⁻¹ mod B)
     * 
     * @note DIVISIÓN POR 0: Si arg = 0, no se modifica *this
     * @note COMPLEJIDAD: O(1) - módulo es operación de hardware
     * 
     * @example dig_t<256>{100} %= dig_t<256>{30}
     *          → 100 mod 30 = 10
     * 
     * @example dig_t<256>{15} %= dig_t<256>{7}
     *          → 15 mod 7 = 1
     * 
     * @see operator%(dig_t) que delega en este operador
     */
    constexpr 
    const dig_t &operator%=(dig_t arg) noexcept {
      if (arg.m_d != ui_0()) m_d %= arg.m_d;
      return (*this);
    }

    /**
     * @brief Operador de módulo con asignación desde entero (dig_t %= Int_t)
     * @tparam Int_t Tipo entero (signed o unsigned)
     * @param arg Entero divisor (se normaliza módulo B)
     * @return Referencia a *this después del cálculo del resto
     * 
     * @details Normaliza arg módulo B y aplica operación módulo.
     * 
     * @note El entero se normaliza primero: arg' = arg mod B
     * @note Luego aplica: *this = *this mod arg'
     * 
     * @example dig_t<256>{100} %= 286
     *          → 286 ≡ 30 (mod 256)
     *          → 100 mod 30 = 10
     */
    template <type_traits::integral_c Int_t> constexpr
    const dig_t &operator%=(Int_t arg) noexcept {
      dig_t cparg{normaliza<Int_t>(arg)};
      if (cparg != dig_0()) (*this) %= cparg;
      return (*this);
    }

    /**
     * @section INCREMENTO_DECREMENTO Operadores de Incremento y Decremento
     * 
     * @brief Operadores para modificar un dig_t en ±1 con aritmética modular
     * 
     * @details Estos operadores implementan incremento/decremento circular:
     * - Incremento en B-1 da vuelta a 0
     * - Decremento en 0 da vuelta a B-1
     * 
     * **VERSIONES:**
     * - Pre-incremento/decremento: ++x, --x (retornan referencia modificada)
     * - Post-incremento/decremento: x++, x-- (retornan copia antes de modificar)
     * 
     * @note EFICIENCIA: Pre-incremento es más eficiente (no crea copia)
     * @note CIRCULARIDAD: ℤ/Bℤ es un grupo cíclico bajo adición
     */

    /**
     * @brief Operador de pre-incremento (++dig_t)
     * @return Referencia a *this después del incremento
     * 
     * @details Incrementa en 1 módulo B: (*this + 1) mod B
     * 
     * **COMPORTAMIENTO:**
     * - Si m_d < B-1: simplemente incrementa m_d
     * - Si m_d = B-1: da vuelta a 0 (circularidad)
     * 
     * @note MÁS EFICIENTE que post-incremento (no crea copia)
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<256>{5}: ++x → x = 6
     * @example dig_t<256>{255}: ++x → x = 0 (wraparound)
     */
    constexpr 
    const dig_t &operator++() noexcept {
      (m_d < ui_max()) ? (++m_d) : (m_d = ui_0());
      return (*this);
    }

    /**
     * @brief Operador de post-incremento (dig_t++)
     * @param int Parámetro ficticio para distinguir de pre-incremento
     * @return Copia del valor antes del incremento
     * 
     * @details Retorna copia del valor original y luego incrementa.
     * 
     * **ALGORITMO:**
     * 1. Guarda copia del valor actual
     * 2. Llama a operator++() (pre-incremento)
     * 3. Retorna la copia guardada
     * 
     * @note MENOS EFICIENTE que pre-incremento (crea copia temporal)
     * @note COMPLEJIDAD: O(1) pero con overhead de copia
     * 
     * @example dig_t<256> x{5}, y = x++;
     *          → y = 5, x = 6
     */
    constexpr 
    dig_t operator++(int) noexcept {
      dig_t ret(*this);
      ++(*this);
      return ret;
    }

    /**
     * @brief Operador de pre-decremento (--dig_t)
     * @return Referencia a *this después del decremento
     * 
     * @details Decrementa en 1 módulo B: (*this - 1) mod B
     * 
     * **COMPORTAMIENTO:**
     * - Si m_d > 0: simplemente decrementa m_d
     * - Si m_d = 0: da vuelta a B-1 (circularidad)
     * 
     * @note MÁS EFICIENTE que post-decremento (no crea copia)
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<256>{5}: --x → x = 4
     * @example dig_t<256>{0}: --x → x = 255 (wraparound)
     */
    constexpr 
    const dig_t &operator--() noexcept {
      if (m_d > ui_0()) { --m_d; }
      else { m_d = ui_max(); }
      return (*this);
    }

    /**
     * @brief Operador de post-decremento (dig_t--)
     * @param int Parámetro ficticio para distinguir de pre-decremento
     * @return Copia del valor antes del decremento
     * 
     * @details Retorna copia del valor original y luego decrementa.
     * 
     * **ALGORITMO:**
     * 1. Guarda copia del valor actual
     * 2. Llama a operator--() (pre-decremento)
     * 3. Retorna la copia guardada
     * 
     * @note MENOS EFICIENTE que pre-decremento (crea copia temporal)
     * @note COMPLEJIDAD: O(1) pero con overhead de copia
     * 
     * @example dig_t<256> x{5}, y = x--;
     *          → y = 5, x = 4
     * 
     * @example dig_t<256> x{0}, y = x--;
     *          → y = 0, x = 255
     */
    constexpr 
    dig_t operator--(int) noexcept {
      dig_t ret(*this);
      --(*this);
      return ret;
    }

    // =========================================================================
    // @section OPERADORES_ARITMETICOS_SIMPLES
    //          Operadores aritméticos sin asignación (+, -, *, /, %)
    // =========================================================================
    
    /**
     * @brief Operador de suma sin asignación (a + b)
     * @param arg Sumando derecho
     * @return Nuevo dig_t con el resultado de la suma módulo B
     * 
     * @details Implementa (a + b) mod B creando una copia y delegando en operator+=.
     * 
     * **ALGORITMO:**
     * 1. Crea copia de *this
     * 2. Aplica operator+= a la copia
     * 3. Retorna la copia modificada
     * 
     * **OPTIMIZACIÓN DEL COMPILADOR:**
     * - El compilador suele optimizar la copia temporal (RVO/NRVO)
     * - En contextos de asignación: x = a + b optimiza eficientemente
     * - Para modificar in-place, usar operator+= directamente
     * 
     * @note Este operador es constexpr y noexcept
     * @note Para cadenas de operaciones: a+b+c crea copias intermedias
     * @note COMPLEJIDAD: O(1) + overhead de copia
     * 
     * @example dig_t<256>{100} + dig_t<256>{200} = dig_t<256>{44}
     * @example dig_t<10>{7} + dig_t<10>{8} = dig_t<10>{5}
     * 
     * @see operator+= para versión con asignación (más eficiente para modificar)
     */
    constexpr 
    dig_t operator+(dig_t arg) const noexcept { 
      dig_t ret{*this}; 
      ret += arg; 
      return ret; 
    }

    /**
     * @brief Operador de resta sin asignación (a - b)
     * @param arg Sustraendo
     * @return Nuevo dig_t con el resultado de la resta módulo B
     * 
     * @details Implementa (a - b) mod B creando una copia y delegando en operator-=.
     * 
     * **COMPORTAMIENTO:**
     * - Si a ≥ b: resultado directo a - b
     * - Si a < b: resultado con wraparound (a - b + B)
     * 
     * **ALGORITMO:**
     * 1. Crea copia de *this
     * 2. Aplica operator-= a la copia
     * 3. Retorna la copia modificada
     * 
     * @note COMPLEJIDAD: O(1) + overhead de copia
     * 
     * @example dig_t<256>{100} - dig_t<256>{50} = dig_t<256>{50}
     * @example dig_t<256>{50} - dig_t<256>{100} = dig_t<256>{206} (wraparound)
     * 
     * @see operator-= para versión con asignación
     */
    constexpr 
    dig_t operator-(dig_t arg) const noexcept { 
      dig_t ret{*this}; 
      ret -= arg; 
      return ret; 
    }

    /**
     * @brief Operador de multiplicación sin asignación (a × b)
     * @param arg Factor derecho
     * @return Nuevo dig_t con el resultado de la multiplicación módulo B
     * 
     * @details Implementa (a × b) mod B creando una copia y delegando en operator*=.
     * 
     * **ALGORITMO:**
     * 1. Crea copia de *this
     * 2. Aplica operator*= a la copia (con prevención de overflow)
     * 3. Retorna la copia modificada
     * 
     * **PROPIEDADES MATEMÁTICAS:**
     * - Conmutativo: a×b = b×a
     * - Asociativo: (a×b)×c = a×(b×c)
     * - Distributivo: a×(b+c) = a×b + a×c
     * - Neutro multiplicativo: a×1 = a
     * - Absorción: a×0 = 0
     * 
     * @note COMPLEJIDAD: O(1) + overhead de copia
     * 
     * @example dig_t<256>{100} * dig_t<256>{50} = dig_t<256>{136}
     * @example dig_t<257>{7} * dig_t<257>{8} = dig_t<257>{56}
     * 
     * @see operator*= para versión con asignación
     */
    constexpr 
    dig_t operator*(dig_t arg) const noexcept { 
      dig_t ret{*this}; 
      ret *= arg; 
      return ret; 
    }

    /**
     * @brief Operador de división modular sin asignación (a / b)
     * @param arg Divisor (debe ser unidad en ℤ/Bℤ)
     * @return Nuevo dig_t con el resultado de a × b⁻¹ mod B
     * 
     * @details Implementa división modular: a/b ≡ a×b⁻¹ (mod B)
     * 
     * **ALGORITMO:**
     * 1. Crea copia de *this
     * 2. Aplica operator/= que calcula a × mult_inv(b)
     * 3. Retorna la copia modificada
     * 
     * **PRECONDICIÓN:**
     * - arg debe ser unidad: gcd(arg, B) = 1
     * - En bases primas (B=257), todos los valores ≠ 0 son unidades
     * - En bases compuestas (B=256), solo impares son unidades
     * 
     * @warning Si arg no es unidad, el comportamiento es indefinido
     * @note COMPLEJIDAD: O(B × log B) debido a mult_inv() + overhead de copia
     * 
     * @example Base 257 (prima): dig_t<257>{7} / dig_t<257>{3} = dig_t<257>{88}
     *          Verificación: 88 × 3 = 264 ≡ 7 (mod 257) ✓
     * 
     * @see operator/= para versión con asignación
     * @see mult_inv() para el cálculo del inverso multiplicativo
     */
    constexpr 
    dig_t operator/(dig_t arg) const noexcept { 
      dig_t ret{*this}; 
      ret /= arg; 
      return ret; 
    }

    /**
     * @brief Operador de módulo sin asignación (a mod b)
     * @param arg Módulo (debe ser ≠ 0)
     * @return Nuevo dig_t con el resto de la división entera
     * 
     * @details Calcula el resto de dividir a entre b: r = a mod b donde 0 ≤ r < b
     * 
     * **DISTINCIÓN IMPORTANTE:**
     * - operator%  → módulo: resto de división entera (a mod b)
     * - operator/  → división modular: a × b⁻¹ mod B
     * 
     * Son operaciones DIFERENTES en aritmética modular.
     * 
     * **ALGORITMO:**
     * 1. Crea copia de *this
     * 2. Aplica operator%= a la copia
     * 3. Retorna la copia modificada
     * 
     * @warning Si arg = 0, no se modifica el valor (protección división por 0)
     * @note COMPLEJIDAD: O(1) + overhead de copia
     * 
     * @example dig_t<256>{100} % dig_t<256>{30} = dig_t<256>{10}
     * @example dig_t<256>{15} % dig_t<256>{7} = dig_t<256>{1}
     * 
     * @see operator%= para versión con asignación
     * @see operator/ para división modular (operación diferente)
     */
    constexpr 
    dig_t operator%(dig_t arg) const noexcept { 
      dig_t ret{*this}; 
      ret %= arg; 
      return ret; 
    }


    /// OPERADORES ARITMÉTICOS SIMPLES CON ENTEROS

    /// OPERADOR DE SUMA CON ENTERO
    template <type_traits::integral_c Int_type> constexpr
    dig_t operator+(Int_type arg) const noexcept { 
        dig_t ret(*this); 
        ret += normaliza<Int_type>(arg); 
        return ret; 
    }

    /// OPERADOR DE RESTA CON ENTERO
    template <type_traits::integral_c Int_type> constexpr
    dig_t operator-(Int_type arg) const noexcept { 
        dig_t ret(*this); 
        const dig_t tmp(normaliza<Int_type>(arg)); 
        ret -= tmp; 
        return ret; 
    }
    
    /// OPERADOR DE MULTIPLICACIÓN CON ENTERO
    template <type_traits::integral_c Int_type> constexpr
    dig_t operator*(Int_type arg) const noexcept { 
        dig_t ret(*this); 
        const dig_t tmp(normaliza<Int_type>(arg)); 
        ret *= tmp; return ret; 
    }
    
    /**
     * @brief Operador de división modular con entero (dig_t / Int_t)
     * @tparam Int_type Tipo entero (signed/unsigned)
     * @param arg Divisor entero (se normaliza, debe resultar en unidad)
     * @return Nuevo dig_t con el resultado de a × (arg mod B)⁻¹
     * 
     * @details Normaliza arg módulo B y aplica división modular.
     * 
     * **ALGORITMO:**
     * 1. Normaliza arg: arg' = arg mod B
     * 2. Verifica que arg' ≠ 0
     * 3. Si arg' es unidad: calcula *this × (arg')⁻¹ mod B
     * 4. Si arg' no es unidad: comportamiento indefinido
     * 
     * **PRECONDICIÓN:**
     * - El valor normalizado de arg debe ser unidad en ℤ/Bℤ
     * - En base prima (B=257): cualquier valor normalizado ≠ 0 vale
     * - En base compuesta (B=256): solo valores impares normalizados
     * 
     * **PROTECCIÓN DIVISIÓN POR CERO:**
     * - Si arg normaliza a 0, no se modifica el valor original
     * - Evita cálculo de mult_inv(0) que sería inválido
     * 
     * @warning Si arg normaliza a no-unidad, resultado indefinido
     * @note COMPLEJIDAD: O(B × log B) por mult_inv() + normalización
     * 
     * @example Base 257: dig_t<257>{21} / 3 = dig_t<257>{7}
     * @example dig_t<256>{42} / 257 = dig_t<256>{42} / 1 = dig_t<256>{42}
     * 
     * @see operator/= para versión con asignación
     * @see mult_inv() para el cálculo del inverso
     */
    template <type_traits::integral_c Int_type> 
    constexpr
    dig_t operator/(Int_type arg) const noexcept { 
      dig_t ret(*this); 
      const dig_t cparg(normaliza<Int_type>(arg)); 
      if (cparg != dig_0()) ret /= cparg; 
      return ret; 
    }

    /**
     * @brief Operador de módulo con entero (dig_t mod Int_t)
     * @tparam Int_type Tipo entero (signed/unsigned)
     * @param arg Módulo entero (se normaliza, debe ser ≠ 0)
     * @return Nuevo dig_t con el resto: *this mod (arg mod B)
     * 
     * @details Normaliza arg módulo B y calcula el resto de la división.
     * 
     * **ALGORITMO:**
     * 1. Normaliza arg: arg' = arg mod B
     * 2. Si arg' ≠ 0: calcula *this mod arg'
     * 3. Si arg' = 0: no modifica (protección división por 0)
     * 
     * **NORMALIZACIÓN DEL DIVISOR:**
     * - 100 % 286: normaliza 286 → 30, luego 100 mod 30 = 10
     * - 15 % 263: normaliza 263 → 7, luego 15 mod 7 = 1
     * 
     * **DISTINCIÓN:**
     * - operator% → módulo (resto de división entera)
     * - operator/ → división modular (multiplicación por inverso)
     * 
     * @note Protege contra división por 0 (si arg ≡ 0 mod B)
     * @note COMPLEJIDAD: O(1) + overhead de normalización
     * 
     * @example dig_t<256>{100} % 30 = dig_t<256>{10}
     * @example dig_t<256>{100} % 286 = dig_t<256>{10} (286 mod 256 = 30)
     * 
     * @see operator%= para versión con asignación
     * @see operator/ para división modular (operación diferente)
     */
    template <type_traits::integral_c Int_type> 
    constexpr
    dig_t operator%(Int_type arg) const noexcept { 
      dig_t ret(*this); 
      const dig_t cparg(normaliza<Int_type>(arg)); 
      if (cparg != dig_0()) ret %= cparg; 
      return ret; 
    }

    // =========================================================================
    // @section OPERADORES_UNARIOS
    //          Complementos y negación en aritmética modular
    // =========================================================================

    /**
     * @brief Operador de complemento a B-1 (NOT bitwise interpretado)
     * @return Nuevo dig_t con el complemento (B-1) - m_d
     * 
     * @details Calcula el complemento respecto a B-1: ~a = (B-1) - a
     * 
     * **INTERPRETACIÓN:**
     * - En aritmética modular: complemento respecto al máximo valor
     * - Equivale a "invertir" el dígito respecto a B-1
     * - NO es negación (para eso usar operator-)
     * 
     * **PROPIEDADES:**
     * - ~(~a) = a (involución: aplicar dos veces regresa al original)
     * - ~0 = B-1 (máximo)
     * - ~(B-1) = 0 (mínimo)
     * - a + ~a = B-1 para todo a
     * 
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<256>{100}: ~100 = 155 (porque 255-100=155)
     * @example dig_t<10>{3}: ~3 = 6 (porque 9-3=6)
     * @example dig_t<256>{0}: ~0 = 255
     * 
     * @see operator! para versión lógica (idéntico comportamiento)
     * @see C_Bm1() para versión con nombre explícito
     * @see operator- para negación aritmética
     */
    constexpr 
    dig_t operator~() const noexcept { 
      return dig_t(ui_max() - m_d); 
    }

    /**
     * @brief Operador lógico NOT (interpretado como complemento a B-1)
     * @return Nuevo dig_t con el complemento (B-1) - m_d
     * 
     * @details En ℤ/Bℤ, el NOT lógico se interpreta como complemento a B-1.
     *          Idéntico comportamiento a operator~().
     * 
     * **USO:**
     * - En contextos lógicos: !a representa "invertir" el valor
     * - NO es negación booleana (0/1), sino complemento aritmético
     * 
     * @note Comportamiento idéntico a operator~()
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<256>{100}: !100 = 155
     * @example dig_t<10>{7}: !7 = 2
     * 
     * @see operator~() para versión bitwise (comportamiento idéntico)
     */
    constexpr 
    dig_t operator!() const noexcept { 
      return dig_t(ui_max() - m_d); 
    }

    /**
     * @brief Operador de negación aritmética (complemento a B)
     * @return Nuevo dig_t con -a = (B - a) mod B
     * 
     * @details Calcula el inverso aditivo en ℤ/Bℤ: a + (-a) ≡ 0 (mod B)
     * 
     * **ALGORITMO:**
     * - Si a = 0: -a = 0 (caso especial)
     * - Si a ≠ 0: -a = B - a
     * 
     * **PROPIEDADES MATEMÁTICAS:**
     * - a + (-a) = 0 (inverso aditivo)
     * - -(-a) = a (involución)
     * - -0 = 0 (el cero es su propio inverso)
     * - -(B-1) = 1
     * - -1 = B-1
     * 
     * **DISTINCIÓN IMPORTANTE:**
     * - operator-() → negación: -a = B - a (inverso aditivo)
     * - operator~() → complemento: ~a = (B-1) - a (complemento a B-1)
     * - Diferencia: operator- deja 0 fijo, operator~ no
     * 
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<256>{100}: -100 = 156 (porque 256-100=156)
     * @example dig_t<256>{0}: -0 = 0 (caso especial)
     * @example dig_t<10>{3}: -3 = 7 (porque 10-3=7)
     * @example Verificación: 100 + 156 = 256 ≡ 0 (mod 256) ✓
     * 
     * @see C_B() para versión con nombre explícito
     * @see operator~() para complemento a B-1 (diferente)
     */
    constexpr 
    dig_t operator-() const noexcept { 
      return dig_t((m_d == 0) ? 0 : (B - m_d)); 
    }

    /**
     * @brief Complemento a B-1 (versión con nombre explícito)
     * @return Nuevo dig_t con (B-1) - m_d
     * 
     * @details Versión explícitamente nombrada de operator~().
     *          Útil cuando se prefiere claridad sobre sintaxis de operador.
     * 
     * **NOMBRE:** C_Bm1 = "Complemento a (B menos 1)"
     * 
     * @note Comportamiento idéntico a operator~() y operator!()
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<256>{100}.C_Bm1() = 155
     * 
     * @see operator~() para versión con sintaxis de operador
     * @see mC_Bm1() para versión con asignación
     */
    constexpr 
    dig_t C_Bm1() const noexcept { 
      return dig_t(ui_max() - m_d); 
    }

    /**
     * @brief Complemento a B / Negación (versión con nombre explícito)
     * @return Nuevo dig_t con B - m_d (o 0 si m_d = 0)
     * 
     * @details Versión explícitamente nombrada de operator-().
     *          Calcula el inverso aditivo en ℤ/Bℤ.
     * 
     * **NOMBRE:** C_B = "Complemento a B"
     * 
     * @note Comportamiento idéntico a operator-()
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<256>{100}.C_B() = 156
     * @example dig_t<256>{0}.C_B() = 0
     * 
     * @see operator-() para versión con sintaxis de operador
     * @see mC_B() para versión con asignación
     */
    constexpr 
    dig_t C_B() const noexcept { 
      return dig_t((m_d == 0) ? 0 : (B - m_d)); 
    }

    /**
     * @brief Complemento a B-1 con asignación (modifica *this)
     * @return Referencia a *this modificado
     * 
     * @details Versión in-place de C_Bm1(): modifica el objeto actual.
     * 
     * **ALGORITMO:**
     * - m_d = (B-1) - m_d
     * - Retorna *this
     * 
     * **USO:**
     * - Más eficiente que x = x.C_Bm1() (evita copia)
     * - Útil para modificación en lugar
     * 
     * @note Modifica el objeto (no es const)
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<256> x{100}; x.mC_Bm1(); // ahora x = 155
     * 
     * @see C_Bm1() para versión sin asignación
     * @see operator~() para versión con sintaxis de operador
     */
    constexpr 
    const dig_t &mC_Bm1() noexcept { 
      m_d = (ui_max() - m_d); 
      return (*this); 
    }

    /**
     * @brief Complemento a B / Negación con asignación (modifica *this)
     * @return Referencia a *this modificado
     * 
     * @details Versión in-place de C_B(): calcula el inverso aditivo.
     * 
     * **ALGORITMO:**
     * - Si m_d = 0: no cambia
     * - Si m_d ≠ 0: m_d = B - m_d
     * - Retorna *this
     * 
     * **USO:**
     * - Más eficiente que x = -x (evita copia)
     * - Para negar el valor en lugar
     * 
     * @note Modifica el objeto (no es const)
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<256> x{100}; x.mC_B(); // ahora x = 156
     * @example dig_t<256> y{0}; y.mC_B(); // y sigue siendo 0
     * 
     * @see C_B() para versión sin asignación
     * @see operator-() para versión con sintaxis de operador
     */
    constexpr 
    const dig_t &mC_B() noexcept { 
      (m_d == 0) ? (m_d) : (m_d = (B - m_d)); 
      return (*this); 
    }

    // =========================================================================
    // @section FUNCIONES_CONSULTA_BOOLEANAS
    //          Predicados sobre el valor del dígito
    // =========================================================================

    /**
     * @brief Verifica si el dígito es cero
     * @return true si m_d = 0, false en caso contrario
     * 
     * @details Consulta de valor mínimo en ℤ/Bℤ.
     * 
     * **USO COMÚN:**
     * - Validación antes de divisiones
     * - Detección de neutro aditivo
     * - Condiciones de terminación
     * 
     * @note COMPLEJIDAD: O(1)
     * @note constexpr: evaluable en tiempo de compilación
     * 
     * @example dig_t<256>{0}.is_0() = true
     * @example dig_t<256>{1}.is_0() = false
     */
    constexpr bool is_0() const noexcept { return (m_d == ui_0()); }

    /**
     * @brief Verifica si el dígito es uno
     * @return true si m_d = 1, false en caso contrario
     * 
     * @details Consulta de neutro multiplicativo.
     * 
     * **PROPIEDADES:**
     * - 1 es unidad en cualquier base B > 1
     * - 1 es neutro multiplicativo: a × 1 = a
     * - 1 es su propio inverso: 1⁻¹ = 1
     * 
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<256>{1}.is_1() = true
     * @example dig_t<256>{257}.is_1() = true (257 % 256 = 1)
     */
    constexpr bool is_1() const noexcept { return (m_d == ui_1()); }

    /**
     * @brief Verifica si el dígito es 0 o 1
     * @return true si m_d ∈ {0, 1}, false en caso contrario
     * 
     * @details Útil para optimizaciones y casos especiales.
     * 
     * **CASOS DE USO:**
     * - Simplificación de operaciones
     * - Detección de valores booleanos
     * - Optimizaciones en exponenciación
     * 
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<256>{0}.is_0or1() = true
     * @example dig_t<256>{1}.is_0or1() = true
     * @example dig_t<256>{2}.is_0or1() = false
     */
    constexpr bool is_0or1() const noexcept { return ((m_d == ui_0()) || (m_d == ui_1())); }

    /**
     * @brief Verifica si el dígito NO es uno
     * @return true si m_d ≠ 1, false si m_d = 1
     * 
     * @details Negación lógica de is_1().
     * 
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<256>{0}.is_not_1() = true
     * @example dig_t<256>{1}.is_not_1() = false
     */
    constexpr bool is_not_1() const noexcept { return (m_d != ui_1()); }

    /**
     * @brief Verifica si el dígito NO es cero
     * @return true si m_d ≠ 0, false si m_d = 0
     * 
     * @details Negación lógica de is_0().
     * 
     * **USO COMÚN:**
     * - Validación de divisores no nulos
     * - Detección de elementos no nulos
     * - Condiciones de continuación
     * 
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<256>{5}.is_not_0() = true
     * @example dig_t<256>{0}.is_not_0() = false
     */
    constexpr bool is_not_0() const noexcept { return (m_d != ui_0()); }

    /**
     * @brief Verifica si el dígito NO es 0 ni 1
     * @return true si m_d ∉ {0, 1}, false si m_d ∈ {0, 1}
     * 
     * @details Negación lógica de is_0or1().
     * 
     * **USO:**
     * - Filtrado de casos especiales
     * - Validación de valores "normales"
     * 
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<256>{2}.is_not_0or1() = true
     * @example dig_t<256>{0}.is_not_0or1() = false
     */
    constexpr bool is_not_0or1() const noexcept { return (!is_0or1()); }

    /**
     * @brief Verifica si el dígito es B-1 (valor máximo)
     * @return true si m_d = B-1, false en caso contrario
     * 
     * @details Consulta de valor máximo en ℤ/Bℤ.
     * 
     * **PROPIEDADES DE B-1:**
     * - Es el máximo valor representable
     * - (B-1) + 1 ≡ 0 (mod B) (wraparound)
     * - -(B-1) ≡ 1 (mod B)
     * - En base 2: B-1 = 1 (único caso donde coincide con 1)
     * 
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<256>{255}.is_Bm1() = true
     * @example dig_t<10>{9}.is_Bm1() = true
     * @example dig_t<256>{254}.is_Bm1() = false
     */
    constexpr bool is_Bm1() const noexcept { return (m_d == ui_Bm1()); }

    /**
     * @brief Verifica si el dígito NO es B-1
     * @return true si m_d ≠ B-1, false si m_d = B-1
     * 
     * @details Negación lógica de is_Bm1().
     * 
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<256>{254}.is_not_Bm1() = true
     * @example dig_t<256>{255}.is_not_Bm1() = false
     */
    constexpr bool is_not_Bm1() const noexcept { return (m_d != ui_Bm1()); }

    /**
     * @brief Verifica si el dígito es B-2
     * @return true si m_d = B-2, false en caso contrario
     * 
     * @details Segundo valor máximo en ℤ/Bℤ.
     * 
     * **PROPIEDADES DE B-2:**
     * - Segundo máximo: solo B-1 es mayor
     * - (B-2) + 1 = B-1
     * - (B-2) + 2 ≡ 0 (mod B)
     * 
     * @note COMPLEJIDAD: O(1)
     * @note Útil en algoritmos que necesitan detectar "casi máximo"
     * 
     * @example dig_t<256>{254}.is_Bm2() = true
     * @example dig_t<10>{8}.is_Bm2() = true
     * @example dig_t<256>{255}.is_Bm2() = false
     */
    constexpr bool is_Bm2() const noexcept { return (m_d == ui_Bm2()); }

    /**
     * @brief Verifica si el dígito NO es B-2
     * @return true si m_d ≠ B-2, false si m_d = B-2
     * 
     * @details Negación lógica de is_Bm2().
     * 
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<256>{253}.is_not_Bm2() = true
     * @example dig_t<256>{254}.is_not_Bm2() = false
     */
    constexpr bool is_not_Bm2() const noexcept { return (m_d != ui_Bm2()); }

    /**
     * @brief Verifica si el dígito es B-1 o B-2
     * @return true si m_d ∈ {B-1, B-2}, false en caso contrario
     * 
     * @details Detecta si el valor está en los dos máximos.
     * 
     * **USO:**
     * - Detección de valores "casi máximos"
     * - Optimizaciones con valores grandes
     * - Análisis de proximidad al límite superior
     * 
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<256>{255}.is_Bm1orBm2() = true
     * @example dig_t<256>{254}.is_Bm1orBm2() = true
     * @example dig_t<256>{253}.is_Bm1orBm2() = false
     */
    constexpr bool is_Bm1orBm2() const noexcept { return (is_Bm1() || is_Bm2()); }

    /**
     * @brief Verifica si el dígito NO es B-1 ni B-2
     * @return true si m_d ∉ {B-1, B-2}, false si m_d ∈ {B-1, B-2}
     * 
     * @details Negación lógica de is_Bm1orBm2().
     * 
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<256>{253}.is_not_Bm1orBm2() = true
     * @example dig_t<256>{255}.is_not_Bm1orBm2() = false
     */
    constexpr bool is_not_Bm1orBm2() const noexcept { return (is_not_Bm1() && is_not_Bm2()); }

    /**
     * @brief Verifica si el dígito es extremo (0 o B-1)
     * @return true si m_d ∈ {0, B-1}, false en caso contrario
     * 
     * @details Detecta valores mínimo o máximo.
     * 
     * **INTERPRETACIÓN:**
     * - Valores "extremos" del rango [0, B-1]
     * - Útil para detección de límites
     * 
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<256>{0}.is_maxormin() = true
     * @example dig_t<256>{255}.is_maxormin() = true
     * @example dig_t<256>{100}.is_maxormin() = false
     */
    constexpr bool is_maxormin() const noexcept { return (is_0() || is_Bm1()); }

    /**
     * @brief Verifica si el dígito NO es extremo (no es 0 ni B-1)
     * @return true si m_d ∉ {0, B-1}, false si m_d ∈ {0, B-1}
     * 
     * @details Negación lógica de is_maxormin().
     * 
     * **USO:**
     * - Filtrado de valores "internos"
     * - Validación de rango medio
     * 
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<256>{100}.is_not_maxormin() = true
     * @example dig_t<256>{0}.is_not_maxormin() = false
     */
    constexpr bool is_not_maxormin() const noexcept { return (is_not_0() && is_not_Bm1()); }

    // =========================================================================
    // @subsection FUNCIONES_PROXIMIDAD
    //             Detección de cercanía a extremos
    // =========================================================================

    /**
     * @brief Verifica si el dígito está cerca de extremos
     * @return true si m_d ∈ {0, 1, B-2, B-1}, false en caso contrario
     * 
     * @details Detecta valores en el "borde" del rango [0, B-1].
     * 
     * **DEFINICIÓN "CERCA":**
     * - Distancia ≤ 1 desde los extremos 0 o B-1
     * - Valores: {0, 1, B-2, B-1}
     * 
     * **CASO ESPECIAL BASE 2:**
     * - En B=2: rango = {0, 1}
     * - TODOS los valores están "cerca"
     * - Retorna siempre true
     * 
     * **USO:**
     * - Detección de posibles overflows/underflows
     * - Optimizaciones específicas para valores pequeños/grandes
     * - Análisis de distribución de valores
     * 
     * @note En base 2, siempre retorna true (no tiene sentido la distinción)
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<256>{0}.is_near_maxormin() = true
     * @example dig_t<256>{1}.is_near_maxormin() = true
     * @example dig_t<256>{254}.is_near_maxormin() = true
     * @example dig_t<256>{255}.is_near_maxormin() = true
     * @example dig_t<256>{100}.is_near_maxormin() = false
     * @example dig_t<2>{0}.is_near_maxormin() = true (siempre en base 2)
     * @example dig_t<2>{1}.is_near_maxormin() = true (siempre en base 2)
     */
    constexpr bool is_near_maxormin() const noexcept { 
      if constexpr (B == 2u) { return true; } 
      else { return (is_0() || is_Bm1() || is_1() || is_Bm2()); } 
    }

    /**
     * @brief Verifica si el dígito está lejos de extremos
     * @return true si m_d ∉ {0, 1, B-2, B-1}, false en caso contrario
     * 
     * @details Detecta valores "internos" alejados de los límites.
     * 
     * **DEFINICIÓN "LEJOS":**
     * - Distancia > 1 desde ambos extremos 0 y B-1
     * - Valores en el rango [2, B-3]
     * - No incluye {0, 1, B-2, B-1}
     * 
     * **CASO ESPECIAL BASE 2:**
     * - En B=2: rango = {0, 1}
     * - NO existen valores "lejos"
     * - Retorna siempre false
     * 
     * **USO:**
     * - Filtrado de valores "seguros" (sin riesgo de wraparound inmediato)
     * - Análisis de valores intermedios
     * - Optimizaciones para rango medio
     * 
     * @note En base 2, siempre retorna false (todos están cerca)
     * @note Para B=3, también retorna siempre false (solo hay {0,1,2})
     * @note Tiene sentido solo para B ≥ 4
     * @note COMPLEJIDAD: O(1)
     * 
     * @example dig_t<256>{100}.is_far_maxormin() = true
     * @example dig_t<256>{2}.is_far_maxormin() = true
     * @example dig_t<256>{253}.is_far_maxormin() = true
     * @example dig_t<256>{0}.is_far_maxormin() = false
     * @example dig_t<256>{1}.is_far_maxormin() = false
     * @example dig_t<2>{0}.is_far_maxormin() = false (siempre en base 2)
     * @example dig_t<10>{5}.is_far_maxormin() = true
     */
    constexpr bool is_far_maxormin() const noexcept { 
      if constexpr (B == 2u) { return false; } 
      else { return (is_not_0() && is_not_Bm1() && is_not_1() && is_not_Bm2()); } 
    }

  private:

    /**
     * @brief Convierte el valor del dígito a string decimal
     * @return String con representación decimal del valor (ej: "42", "255")
     * @note Helper interno para to_string()
     */
    std::string num_to_string() const noexcept {
      const std::int64_t data_member{static_cast<std::int64_t>(this->m_d)};
      std::ostringstream fmtr_obj;
      fmtr_obj << data_member;
      const std::string ret{fmtr_obj.str()};
      return ret;
    }

    /**
     * @brief Genera string con formato "B<base>" para serialización
     * @return String con la base (ej: "B10", "B256")
     * @note Helper interno para to_string()
     * @note static porque solo depende de la constante template B
     */
    static std::string radix_str() noexcept {
      constexpr std::int64_t radix{static_cast<std::int64_t>(B)};
      std::ostringstream fmtr_obj;
      fmtr_obj << radix;
      const std::string ret{static_cast<std::string>("B") + static_cast<std::string>(fmtr_obj.str())};
      return ret;
    }

    // =========================================================================
    // PARSING HELPERS - Máquinas de Estado Finitas (FSM)
    // =========================================================================
    
    /**
     * @brief Resultado del parsing de prefijo (FSM 1)
     */
    struct PrefixResult {
      char delimiter_open;   ///< '[' o '#'
      char delimiter_close;  ///< ']' o '#'
      std::size_t next_pos;  ///< Posición después del prefijo y delim_open
    };
    
    /**
     * @brief FSM 1: Parsear prefijo ("d" o "dig") y detectar delimitadores
     * @tparam Container Tipo contenedor (const char* o std::array)
     * @param container Contenedor con los datos
     * @param size Tamaño del contenedor
     * @return std::expected con PrefixResult o parse_error_t
     * 
     * @details Formatos detectados:
     *          - "d["  → Short prefix, brackets
     *          - "d#"  → Short prefix, hashes
     *          - "dig#" → Long prefix, hashes
     *          - "dig[" → Long prefix, brackets
     */
    template<typename Container>
    static constexpr std::expected<PrefixResult, parse_error_t>
    parse_prefix_fsm(const Container& container, std::size_t size) noexcept {
      if (size < 4) {
        return std::unexpected(parse_error_t::empty_or_null);
      }
      
      // Acceso unificado a caracteres (funciona con const char* y std::array)
      auto char_at = [&](std::size_t i) -> char {
        if constexpr (std::is_pointer_v<std::decay_t<Container>>) {
          return container[i];
        } else {
          return container[i];
        }
      };
      
      PrefixResult result;
      
      // Detectar "d[" o "d#"
      if (size >= 4 && char_at(0) == 'd' && char_at(1) == '[') {
        result.delimiter_open = '[';
        result.delimiter_close = ']';
        result.next_pos = 2;
        return result;
      } else if (size >= 4 && char_at(0) == 'd' && char_at(1) == '#') {
        result.delimiter_open = '#';
        result.delimiter_close = '#';
        result.next_pos = 2;
        return result;
      }
      // Detectar "dig#" o "dig["
      else if (size >= 6 && char_at(0) == 'd' && char_at(1) == 'i' && 
               char_at(2) == 'g' && char_at(3) == '#') {
        result.delimiter_open = '#';
        result.delimiter_close = '#';
        result.next_pos = 4;
        return result;
      } else if (size >= 6 && char_at(0) == 'd' && 
               char_at(1) == 'i' && char_at(2) == 'g' && 
               char_at(3) == '['
        ) {
        result.delimiter_open = '[';
        result.delimiter_close = ']';
        result.next_pos = 4;
        return result;
      }
      
      return std::unexpected(parse_error_t::invalid_prefix);
    }
    
    /**
     * @brief Resultado del parsing de número (FSM 2)
     */
    struct NumberResult {
      sig_uint_t value;      ///< Valor parseado (sin normalizar)
      std::size_t next_pos;  ///< Posición después del delimitador de cierre
    };
    
    /**
     * @brief FSM 2: Parsear número decimal entre delimitadores
     * @tparam Container Tipo contenedor (const char* o std::array)
     * @param container Contenedor con los datos
     * @param size Tamaño del contenedor
     * @param pos Posición donde empieza el número
     * @param delim_close Delimitador de cierre esperado (']' o '#')
     * @return std::expected con NumberResult o parse_error_t
     * 
     * @details Lee dígitos decimales hasta encontrar delim_close.
     *          No normaliza el valor (se hace después).
     */
    template<typename Container>
    static constexpr std::expected<NumberResult, parse_error_t>
    parse_number_fsm(const Container& container, std::size_t size, 
                     std::size_t pos, char delim_close) noexcept {
      auto char_at = [&](std::size_t i) -> char {
        if constexpr (std::is_pointer_v<std::decay_t<Container>>) {
          return container[i];
        } else {
          return container[i];
        }
      };
      
      sig_uint_t numero = 0;
      std::size_t digit_count = 0;
      
      // Parsear dígitos hasta encontrar delimitador de cierre
      while (pos < size && char_at(pos) != delim_close) {
        if (char_at(pos) >= '0' && char_at(pos) <= '9') {
          numero = numero * 10 + (char_at(pos) - '0');
          digit_count++;
        } else {
          return std::unexpected(parse_error_t::invalid_digit);
        }
        pos++;
      }
      
      // Verificar delimitador de cierre
      if (pos >= size || char_at(pos) != delim_close) {
        return std::unexpected(parse_error_t::missing_delimiter);
      }
      pos++;  // Consumir delimitador de cierre
      
      if (digit_count == 0) {
        return std::unexpected(parse_error_t::no_digits);
      }
      
      return NumberResult{numero, pos};
    }
    
    /**
     * @brief Resultado del parsing de base (FSM 3)
     */
    struct BaseResult {
      sig_uint_t base_value;  ///< Base leída
      std::size_t next_pos;   ///< Posición final
    };
    
    /**
     * @brief FSM 3: Parsear y validar base ("B" seguido de dígitos)
     * @tparam Container Tipo contenedor (const char* o std::array)
     * @param container Contenedor con los datos
     * @param size Tamaño del contenedor
     * @param pos Posición donde debe estar 'B'
     * @param expected_base Base esperada (debe coincidir con la leída)
     * @return std::expected con BaseResult o parse_error_t
     * 
     * @details Lee "B" seguido de dígitos decimales y valida
     *          que coincida con expected_base.
     */
    template<typename Container>
    static constexpr std::expected<BaseResult, parse_error_t>
    parse_base_fsm(const Container& container, std::size_t size, 
                   std::size_t pos, std::uint64_t expected_base) noexcept {
      auto char_at = [&](std::size_t i) -> char {
        if constexpr (std::is_pointer_v<std::decay_t<Container>>) {
          return container[i];
        } else {
          return container[i];
        }
      };
      
      // Verificar 'B'
      if (pos >= size || char_at(pos) != 'B') {
        return std::unexpected(parse_error_t::missing_B);
      }
      pos++;
      
      // Parsear dígitos de la base
      sig_uint_t base_leida = 0;
      std::size_t base_digits = 0;
      
      while (pos < size && char_at(pos) >= '0' && char_at(pos) <= '9') {
        base_leida = base_leida * 10 + (char_at(pos) - '0');
        base_digits++;
        pos++;
      }
      
      if (base_digits == 0) {
        return std::unexpected(parse_error_t::no_base_digits);
      }
      
      if (base_leida != expected_base) {
        return std::unexpected(parse_error_t::base_mismatch);
      }
      
      return BaseResult{base_leida, pos};
    }

  public:
    // =========================================================================
    // VERSIONES CONSTEVAL PURAS (con recursión) - EXPERIMENTAL
    // =========================================================================
    
    /**
     * @brief Versión consteval pura de parse_prefix_fsm
     * @tparam Container Tipo contenedor (std::array)
     * @param container Contenedor con los datos
     * @param size Tamaño del contenedor
     * @return std::expected con PrefixResult o parse_error_t
     * 
     * @note Esta versión es completamente consteval y trabaja con std::array
     */
    template<typename Container>
    static consteval std::expected<PrefixResult, parse_error_t>
    parse_prefix_fsm_ct(const Container& container, std::size_t size) noexcept {
      if (size < 4) {
        return std::unexpected(parse_error_t::empty_or_null);
      }
      
      PrefixResult result;
      
      // Detectar "d[" o "d#"
      if (size >= 4 && container[0] == 'd' && container[1] == '[') {
        result.delimiter_open = '[';
        result.delimiter_close = ']';
        result.next_pos = 2;
        return result;
      } else if (size >= 4 && container[0] == 'd' && container[1] == '#') {
        result.delimiter_open = '#';
        result.delimiter_close = '#';
        result.next_pos = 2;
        return result;
      }
      // Detectar "dig#" o "dig["
      else if (size >= 6 && container[0] == 'd' && container[1] == 'i' && 
               container[2] == 'g' && container[3] == '#') {
        result.delimiter_open = '#';
        result.delimiter_close = '#';
        result.next_pos = 4;
        return result;
      } else if (size >= 6 && container[0] == 'd' && 
               container[1] == 'i' && container[2] == 'g' && 
               container[3] == '['
        ) {
        result.delimiter_open = '[';
        result.delimiter_close = ']';
        result.next_pos = 4;
        return result;
      }
      
      return std::unexpected(parse_error_t::invalid_prefix);
    }
    
    /**
     * @brief Helper recursivo para parsear número (compile-time)
     * @tparam Container Tipo contenedor (std::array)
     * @param container Contenedor con los datos
     * @param size Tamaño del contenedor
     * @param pos Posición actual
     * @param delim_close Delimitador de cierre
     * @param accumulator Acumulador del valor
     * @param digit_count Contador de dígitos parseados
     * @return std::expected con NumberResult o parse_error_t
     */
    template<typename Container>
    static consteval std::expected<NumberResult, parse_error_t>
    parse_number_fsm_ct_impl(const Container& container, std::size_t size,
                             std::size_t pos, char delim_close,
                             sig_uint_t accumulator, std::size_t digit_count) noexcept {
      // Caso base: llegamos al delimitador de cierre
      if (pos >= size) {
        return std::unexpected(parse_error_t::missing_delimiter);
      }
      
      if (container[pos] == delim_close) {
        if (digit_count == 0) {
          return std::unexpected(parse_error_t::no_digits);
        }
        return NumberResult{accumulator, pos + 1};
      }
      
      // Validar dígito
      if (container[pos] < '0' || container[pos] > '9') {
        return std::unexpected(parse_error_t::invalid_digit);
      }
      
      // Caso recursivo: procesar dígito y continuar
      sig_uint_t new_acc = accumulator * 10 + (container[pos] - '0');
      return parse_number_fsm_ct_impl(container, size, pos + 1, delim_close,
                                      new_acc, digit_count + 1);
    }
    
    /**
     * @brief Versión consteval pura de parse_number_fsm
     * @tparam Container Tipo contenedor (std::array)
     * @param container Contenedor con los datos
     * @param size Tamaño del contenedor
     * @param pos Posición donde empieza el número
     * @param delim_close Delimitador de cierre esperado
     * @return std::expected con NumberResult o parse_error_t
     */
    template<typename Container>
    static consteval std::expected<NumberResult, parse_error_t>
    parse_number_fsm_ct(const Container& container, std::size_t size,
                        std::size_t pos, char delim_close) noexcept {
      return parse_number_fsm_ct_impl(container, size, pos, delim_close, 0, 0);
    }
    
    /**
     * @brief Helper recursivo para parsear base (compile-time)
     * @tparam Container Tipo contenedor (std::array)
     * @param container Contenedor con los datos
     * @param size Tamaño del contenedor
     * @param pos Posición actual
     * @param accumulator Acumulador de la base
     * @param digit_count Contador de dígitos
     * @return std::expected con sig_uint_t o parse_error_t
     */
    template<typename Container>
    static consteval std::expected<sig_uint_t, parse_error_t>
    parse_base_fsm_ct_impl(const Container& container, std::size_t size,
                           std::size_t pos, sig_uint_t accumulator,
                           std::size_t digit_count) noexcept {
      // Caso base: no hay más caracteres o no es dígito
      if (pos >= size || container[pos] < '0' || container[pos] > '9') {
        if (digit_count == 0) {
          return std::unexpected(parse_error_t::no_base_digits);
        }
        return accumulator;
      }
      
      // Caso recursivo: procesar dígito y continuar
      sig_uint_t new_acc = accumulator * 10 + (container[pos] - '0');
      return parse_base_fsm_ct_impl(container, size, pos + 1, new_acc, digit_count + 1);
    }
    
    /**
     * @brief Versión consteval pura de parse_base_fsm
     * @tparam Container Tipo contenedor (std::array)
     * @param container Contenedor con los datos
     * @param size Tamaño del contenedor
     * @param pos Posición donde debe estar 'B'
     * @param expected_base Base esperada
     * @return std::expected con BaseResult o parse_error_t
     */
    template<typename Container>
    static consteval std::expected<BaseResult, parse_error_t>
    parse_base_fsm_ct(const Container& container, std::size_t size,
                      std::size_t pos, std::uint64_t expected_base) noexcept {
      // Verificar 'B'
      if (pos >= size || container[pos] != 'B') {
        return std::unexpected(parse_error_t::missing_B);
      }
      
      // Parsear dígitos de la base recursivamente
      auto base_result = parse_base_fsm_ct_impl(container, size, pos + 1, 0, 0);
      if (!base_result) {
        return std::unexpected(base_result.error());
      }
      
      sig_uint_t base_leida = *base_result;
      
      if (base_leida != expected_base) {
        return std::unexpected(parse_error_t::base_mismatch);
      }
      
      return BaseResult{base_leida, size};  // Posición final aproximada
    }
    
    /**
     * @brief Parser compile-time completamente consteval
     * @tparam N Tamaño del array
     * @param arr Array de caracteres con el string a parsear
     * @param base_template Base esperada (debe coincidir con B)
     * @return std::expected con uint_t parseado o parse_error_t
     * 
     * @details Usa versiones consteval puras con recursión.
     *          Soporta los 4 formatos: "d[N]BM", "d#N#BM", "dig#N#BM", "dig[N]BM"
     * 
     * @note Esta versión es completamente consteval y puede usarse
     *       en contextos que requieren evaluación en tiempo de compilación.
     */
    template<std::size_t N>
    static consteval std::expected<uint_t, parse_error_t>
    parse_impl_pure_ct(const std::array<char, N>& arr, 
                       std::uint64_t base_template) noexcept {
      constexpr std::size_t size = N;
      
      if (size < 4) {
        return std::unexpected(parse_error_t::empty_or_null);
      }
      
      // FSM 1: Parsear prefijo
      auto prefix = parse_prefix_fsm_ct(arr, size);
      if (!prefix) {
        return std::unexpected(prefix.error());
      }
      
      // FSM 2: Parsear número
      auto number = parse_number_fsm_ct(arr, size, prefix->next_pos, 
                                         prefix->delimiter_close);
      if (!number) {
        return std::unexpected(number.error());
      }
      
      // FSM 3: Parsear y validar base
      auto base = parse_base_fsm_ct(arr, size, number->next_pos, base_template);
      if (!base) {
        return std::unexpected(base.error());
      }
      
      // Normalizar el valor
      sig_uint_t numero_final = number->value % base_template;
      
      return static_cast<uint_t>(numero_final);
    }

  private:
    // =========================================================================
    // PARSING DESDE CADENAS - Implementación interna
    // =========================================================================
    
    /**
     * @brief Implementación interna del parser de cadenas
     * @param str Puntero a cadena C (no null)
     * @param size Longitud de la cadena
     * @param base_template Base esperada (debe coincidir con B)
     * @return Par {valor_parseado, éxito}
     * 
     * @details FORMATOS SOPORTADOS:
     *          ✅ "d[N]BM" - número N entre corchetes, base M
     *          ✅ "dig#N#BM" - número N entre almohadillas, base M
     *          ✅ "d#N#BM" - número N entre almohadillas (variante corta), base M
     *          ✅ "dig[N]BM" - número N entre corchetes (variante larga), base M
     * 
     * @note VALIDACIONES:
     *       - La base leída debe coincidir exactamente con base_template (B)
     *       - N se normaliza automáticamente módulo B durante el parsing
     *       - Devuelve {0, false} si el formato es inválido
     * 
     * @note Todos los formatos son simétricos:
     *       - Cortos: "d[...]" y "d#...#"
     *       - Largos: "dig#...#" y "dig[...]"
     * 
     * @todo Considerar consteval para uso en literales compile-time
     * 
     * @see from_string() para versión que lanza excepciones
     * @see from_cstr() para wrapper constexpr
     */
    static constexpr 
    std::pair<uint_t, bool> parse_impl(
        const char *str, std::size_t size, 
        std::uint64_t base_template) noexcept {
      if (!str || size < 4) return {0, false};

      // FSM 1: Parsear prefijo y detectar delimitadores
      auto prefix = parse_prefix_fsm(str, size);
      if (!prefix) return {0, false};

      // FSM 2: Parsear número entre delimitadores
      auto number = parse_number_fsm(str, size, prefix->next_pos, prefix->delimiter_close);
      if (!number) return {0, false};

      // FSM 3: Parsear y validar base
      auto base = parse_base_fsm(str, size, number->next_pos, base_template);
      if (!base) return {0, false};

      // NO normalizamos aquí - el constructor de dig_t ya lo hará
      // Convertir a uint_t para el return
      uint_t numero_parseado = static_cast<uint_t>(number->value);

      return {numero_parseado, true};
    }

  public:
    // =========================================================================
    // PARSING COMPILE-TIME desde array constexpr - SIN EXCEPCIONES
    // =========================================================================
    
    /**
     * @brief Parser constexpr que trabaja con std::array y std::expected
     * @tparam N Tamaño del array de caracteres
     * @param arr Array de caracteres con el string a parsear
     * @return std::expected<uint_t, parse_error_t> con valor o error detallado
     * 
     * @details FORMATOS SOPORTADOS (igual que parse_impl):
     *          ✅ "d[N]BM" - número N entre corchetes, base M
     *          ✅ "dig#N#BM" - número N entre almohadillas, base M
     *          ✅ "d#N#BM" - número N entre almohadillas (variante corta), base M
     *          ✅ "dig[N]BM" - número N entre corchetes (variante larga), base M
     * 
     * @note DIFERENCIAS CON parse_impl:
     *       - Trabaja con std::array<char, N> en lugar de const char*
     *       - Usa std::expected en lugar de std::pair<uint_t, bool>
     *       - Errores detallados (parse_error_t) en lugar de solo false
     *       - Completamente constexpr - apto para literales compile-time
     * 
     * @note USO RECOMENDADO: Para contextos donde NO se permiten excepciones
     *       (inicialización constexpr, metaprogramación, etc.)
     * 
     * @example
     * ```cpp
     * constexpr std::array<char, 7> str = {'d', '[', '5', ']', 'B', '1', '0'};
     * constexpr auto result = dig_t<10>::parse_impl_ct(str);
     * static_assert(result.has_value());
     * static_assert(*result == 5);
     * ```
     * 
     * @see parse_impl() para versión runtime con const char*
     * @see from_string() para versión que lanza excepciones
     */
    template <std::size_t N>
    static constexpr std::expected<uint_t, parse_error_t> 
    parse_impl_ct(const std::array<char, N>& arr) noexcept {
      constexpr std::size_t size = N;
      
      // FSM 1: Parsear prefijo y detectar delimitadores
      auto prefix = parse_prefix_fsm(arr, size);
      if (!prefix) return std::unexpected(prefix.error());

      // FSM 2: Parsear número entre delimitadores
      auto number = parse_number_fsm(arr, size, prefix->next_pos, prefix->delimiter_close);
      if (!number) return std::unexpected(number.error());

      // FSM 3: Parsear y validar base
      auto base = parse_base_fsm(arr, size, number->next_pos, B);
      if (!base) return std::unexpected(base.error());

      // Normalización final (el constructor NO normaliza desde parse_impl_ct)
      uint_t numero_final = static_cast<uint_t>(number->value % static_cast<sig_uint_t>(B));

      return numero_final;
    }

  public:

    /// FUNCIÓN RADIX QUE DEVUELVE LA BASE DEL DÍGITO
    /// UTILIDAD PARA PLANTILLAS GENÉRICAS
    consteval std::uint64_t radix() const { return B; }

    /// FUNCIÓN TO_STRING QUE DEVUELVE LA REPRESENTACIÓN EN CADENA DE CARACTERES
    /// TIPO std::string DE LA FORMA "d[number]Bbase"
    constexpr std::string to_string() const noexcept {
      const std::string num{this->num_to_string()};
      const std::string ret{"d[" + num + "]" + radix_str()};
      return ret;
    }
    
    // =========================================================================
    // CONVERSIÓN A C-STRING COMPILE-TIME
    // =========================================================================
    /**
     * @brief Conversión a C-string en tiempo de compilación
     * @return std::array<char, 32> con formato "d[N]BM" terminado en '\0'
     * 
     * @details Genera representación textual del dígito en formato corto:
     *          - "d[" + valor + "]B" + base + '\0'
     *          - Ejemplo: dig_t<10>(5) → "d[5]B10"
     * 
     * @note CONSTEXPR: Puede ejecutar en tiempo de compilación o runtime
     * @note ARRAY FIJO: Tamaño 32 es suficiente para cualquier base válida
     *       (máximo: "d[4294967295]B4294967296\0" = 25 chars)
     * 
     * @usage
     * ```cpp
     * constexpr auto d = dig_t<10>(7);
     * constexpr auto str = d.to_cstr();
     * static_assert(str[0] == 'd');
     * static_assert(str[1] == '[');
     * // str contiene "d[7]B10\0..."
     * ```
     */
    constexpr std::array<char, 32> to_cstr() const noexcept {
      std::array<char, 32> result{};
      std::size_t pos = 0;
      
      // Prefijo "d["
      result[pos++] = 'd';
      result[pos++] = '[';
      
      // Convertir m_d a string
      if (m_d == 0) {
        result[pos++] = '0';
      } else {
        // Convertir número a dígitos
        char temp[11]; // Máximo 10 dígitos para uint32_t + '\0'
        int temp_pos = 0;
        uint_t num = m_d;
        
        while (num > 0) {
          temp[temp_pos++] = '0' + (num % 10);
          num /= 10;
        }
        
        // Copiar en orden inverso (los dígitos están al revés)
        for (int i = temp_pos - 1; i >= 0; --i) {
          result[pos++] = temp[i];
        }
      }
      
      // Sufijo "]B"
      result[pos++] = ']';
      result[pos++] = 'B';
      
      // Convertir Base a string
      char temp[11];
      int temp_pos = 0;
      std::uint64_t base_val = B;
      
      if (base_val == 0) {
        result[pos++] = '0';
      } else {
        while (base_val > 0) {
          temp[temp_pos++] = '0' + (base_val % 10);
          base_val /= 10;
        }
        
        // Copiar en orden inverso
        for (int i = temp_pos - 1; i >= 0; --i) {
          result[pos++] = temp[i];
        }
      }
      
      // Null terminator (el resto ya está inicializado a 0)
      result[pos] = '\0';
      
      return result;
    }
    
  }; /// END CLASS DIG_T

  /// FUNCIÓN DE PARSEO DESDE UNA CADENA DE CARACTERES (STATIC)
  template <std::uint64_t Base>
    requires(Base > 1)
  std::expected<dig_t<Base>, parse_error_t> dig_t<Base>::from_string(const std::string &str) noexcept {
    auto [value, success] = parse_impl(str.c_str(), str.size(), Base);

    if (!success) {
      return std::unexpected(parse_error_t::unknown);
    }

    return dig_t<Base>(value);
  }

  /// FUNCIÓN DE PARSEO DESDE UNA CADENA DE CARACTERES CSTR (STATIC)
  template <std::uint64_t Base>
    requires(Base > 1)
  constexpr std::expected<dig_t<Base>, parse_error_t> dig_t<Base>::from_cstr(const char *str) noexcept {
    if (str == nullptr) {
      return std::unexpected(parse_error_t::empty_or_null);
    }

    std::size_t len = 0;
    while (str[len] != '\0')
      len++;

    auto [value, success] = parse_impl(str, len, Base);

    if (!success) {
      return std::unexpected(parse_error_t::unknown);
    }

    return dig_t<Base>(value);
  }

  /**
   * @brief Factory consteval para crear dig_t desde std::array SIN excepciones
   * @tparam Base Base del sistema numérico (debe ser > 1)
   * @tparam Arr Array de caracteres (argumento template)
   * @return std::expected con dig_t o parse_error_t
   * 
   * @details Función factory consteval que usa parse_impl_ct para parsing seguro.
   *          Array pasado como argumento template garantiza evaluación compile-time.
   * 
   * @note VENTAJAS:
   *       - consteval: garantiza evaluación en tiempo de compilación
   *       - No lanza excepciones (std::expected)
   *       - Errores detallados (parse_error_t enum)
   *       - No requiere nullptr checking (std::array siempre válido)
   *       - Array como template: cero overhead runtime
   * 
   * @note USO TÍPICO:
   *       ```cpp
   *       constexpr auto result = dig_t<10>::from_array_ct<std::array{'d', '[', '7', ']', 'B', '1', '0'}>();
   *       static_assert(result.has_value());
   *       static_assert(result->get() == 7);
   *       ```
   * 
   * @see parse_impl_ct() para parsing de bajo nivel
   * @see from_string() para versión runtime sin excepciones
   * @see from_cstr() para versión constexpr sin excepciones
   */
  template <std::uint64_t Base>
    requires(Base > 1)
  template <auto Arr>
  inline std::expected<dig_t<Base>, parse_error_t>
  dig_t<Base>::from_array_ct() noexcept {
    auto result = parse_impl_ct(Arr);
    
    if (result.has_value()) {
      return dig_t<Base>(*result);
    } else {
      return std::unexpected(result.error());
    }
  }

  /// IMPLEMENTACIÓN DE LOS CONSTRUCTORES DESDE CADENA DE CARACTERES
  template <std::uint64_t Base>
    requires(Base > 1)
  dig_t<Base>::dig_t(const std::string &str) noexcept {
    auto result = from_string(str);
    if (result) {
      *this = *result;
    } else {
      // Si parsing falla, construir dig_t(0)
      m_d = 0;
    }
  }

  /// IMPLEMENTACIÓN DE LOS CONSTRUCTORES DESDE CADENA DE CARACTERES CSTR
  template <std::uint64_t Base>
    requires(Base > 1)
  constexpr dig_t<Base>::dig_t(const char *str) noexcept {
    auto result = from_cstr(str);
    if (result) {
      *this = *result;
    } else {
      // Si parsing falla o str==nullptr, construir dig_t(0)
      m_d = 0;
    }
  }

    /// SOBRECARGAS DE LOS OPERADORES DE FLUJO
  template <std::uint64_t Base>
    requires(Base > 1)
  std::istream &operator>>(std::istream &is, dig_t<Base> &arg) {
    std::string input_str;
    is >> input_str;

    if (is.fail()) { return is; }

    auto result = dig_t<Base>::from_string(input_str);
    if (result) {
      arg = *result;
    } else {
      is.setstate(std::ios::failbit);
    }

    return is;
  }

  /// IMPLEMENTACIÓN DEL OPERADOR DE SALIDA
  template <std::uint64_t Base>
    requires(Base > 1)
  std::ostream &operator<<(std::ostream &os, dig_t<Base> arg) {
    os << "d[" << static_cast<std::int64_t>(arg())
       << "]B" << static_cast<std::int64_t>(Base);
    return os;
  }

  // ===========================================================================
  // FUNCIONES LIBRES - FACTORY FUNCTIONS
  // ===========================================================================
  
  /**
   * @defgroup make_digit Factory functions para creación conveniente de dig_t
   * @{
   * 
   * @brief Familia de funciones libres para crear dígitos de forma ergonómica
   * 
   * @details Esta familia proporciona 4 sobrecargas de `make_digit()`:
   * 
   * **1. make_digit<Base, Arr>()** - Array compile-time
   * ```cpp
   * auto d = make_digit<10, std::array{'d','[','5',']','B','1','0'}>();
   * // Retorna: std::expected<dig_t<10>, parse_error_t>
   * ```
   * 
   * **2. make_digit<Base>(string)** - String runtime
   * ```cpp
   * std::string str = "d[5]B10";
   * auto d = make_digit<10>(str);
   * // Retorna: std::expected<dig_t<10>, parse_error_t>
   * ```
   * 
   * **3. make_digit<Base>(const char*)** - C-string
   * ```cpp
   * auto d = make_digit<16>("d[7]B16");
   * // Retorna: std::expected<dig_t<16>, parse_error_t>
   * ```
   * 
   * **4. make_digit<Base>(entero)** - Valor directo
   * ```cpp
   * auto d = make_digit<10>(5);
   * // Retorna: dig_t<10> (siempre éxito, sin expected)
   * ```
   * 
   * @note Las versiones 1-3 retornan `std::expected` porque pueden fallar (parsing)
   * @note La versión 4 retorna `dig_t` directamente porque siempre tiene éxito
   * @note Todas normalizan valores automáticamente: `value % Base`
   * @note Base debe especificarse explícitamente (no se puede deducir)
   * 
   * @see dig_t<Base>::from_string() - Método estático equivalente a versión 2
   * @see dig_t<Base>::from_cstr() - Método estático equivalente a versión 3
   * @see dig_t<Base>::from_array_ct<Arr>() - Método estático equivalente a versión 1
   */

  /**
   * @brief Factory function para crear dig_t desde array con base especificada
   * @ingroup make_digit
   * @tparam Base Base del sistema numérico (debe coincidir con la del string)
   * @tparam Arr Array de caracteres (deducido automáticamente)
   * @return std::expected<dig_t<Base>, parse_error_t>
   * 
   * @details Versión que requiere especificar la base explícitamente.
   *          Valida que la base del string coincida con Base template.
   * 
   * @note NOTA IMPORTANTE: No es posible crear una versión que deduzca la base
   *       automáticamente sin especificarla, porque el tipo de retorno dig_t<Base>
   *       depende de Base como parámetro template (no puede ser runtime).
   * 
   * @note Para casos donde la base es dinámica, se necesitaría usar std::variant
   *       con todas las bases posibles, lo cual es poco práctico.
   * 
   * @note MSVC limitation: std::expected no puede evaluarse en constexpr con errores,
   *       así que la función es inline (runtime) a pesar del array compile-time
   * 
   * @example Uso básico:
   * ```cpp
   * auto d = make_digit<10, std::array{'d','[','5',']','B','1','0'}>();
   * assert(d.has_value());
   * assert(d->get() == 5);
   * ```
   * 
   * @example Con validación:
   * ```cpp
   * auto d = make_digit<16, std::array{'d','[','7',']','B','1','6'}>();
   * if (d) {
   *     std::cout << d->get() << std::endl; // 7
   * }
   * ```
   */
  template <std::uint64_t Base, auto Arr>
    requires(Base > 1)
  [[nodiscard]] inline auto make_digit() noexcept 
    -> std::expected<dig_t<Base>, parse_error_t> {
    return dig_t<Base>::template from_array_ct<Arr>();
  }

  /**
   * @brief Factory function runtime para crear dig_t desde string con base especificada
   * @ingroup make_digit
   * @tparam Base Base del sistema numérico
   * @param str String a parsear (formato: "d[N]BM" o "dig#N#BM")
   * @return std::expected<dig_t<Base>, parse_error_t>
   * 
   * @details Versión runtime que funciona con strings dinámicos.
   *          Útil cuando el string no se conoce en compile-time.
   * 
   * @note Para compile-time, usar make_digit<Base, Arr>() con std::array
   * 
   * @example Runtime:
   * ```cpp
   * std::string input = "d[5]B10";
   * auto d = make_digit<10>(input);
   * if (d) {
   *     std::cout << d->get() << std::endl; // 5
   * }
   * ```
   * 
   * @example Runtime con input de usuario:
   * ```cpp
   * std::string user_input;
   * std::cin >> user_input; // Usuario ingresa "d[42]B10"
   * auto d = make_digit<10>(user_input);
   * ```
   */
  template <std::uint64_t Base>
    requires(Base > 1)
  [[nodiscard]] inline std::expected<dig_t<Base>, parse_error_t> 
  make_digit(const std::string& str) noexcept {
    return dig_t<Base>::from_string(str);
  }

  /**
   * @brief Factory function runtime para crear dig_t desde C-string
   * @ingroup make_digit
   * @tparam Base Base del sistema numérico
   * @param str C-string a parsear (formato: "d[N]BM" o "dig#N#BM")
   * @return std::expected<dig_t<Base>, parse_error_t>
   * 
   * @details Versión runtime/constexpr que funciona con C-strings.
   * 
   * @note constexpr: puede usarse en compile-time si str es constexpr
   * 
   * @example Compile-time:
   * ```cpp
   * constexpr auto d = make_digit<10>("d[5]B10");
   * static_assert(d.has_value() && d->get() == 5);
   * ```
   * 
   * @example Runtime:
   * ```cpp
   * const char* input = "d[7]B16";
   * auto d = make_digit<16>(input);
   * ```
   */
  template <std::uint64_t Base>
    requires(Base > 1)
  [[nodiscard]] constexpr std::expected<dig_t<Base>, parse_error_t> 
  make_digit(const char* str) noexcept {
    return dig_t<Base>::from_cstr(str);
  }

  /**
   * @brief Factory function para crear dig_t desde valor entero con base especificada
   * @ingroup make_digit
   * @tparam Base Base del sistema numérico
   * @tparam Int_t Tipo entero (deducido automáticamente)
   * @param value Valor del dígito (se normalizará módulo Base)
   * @return dig_t<Base> con valor normalizado
   * 
   * @details Crea un dígito directamente desde un valor entero.
   *          El valor se normaliza automáticamente módulo Base.
   * 
   * @note Esta función siempre tiene éxito (no puede fallar)
   * @note Base debe especificarse explícitamente como template parameter
   * @note El valor se normaliza: value % Base
   * 
   * @example Uso básico:
   * ```cpp
   * auto d1 = make_digit<10>(5);        // dig_t<10> con valor 5
   * auto d2 = make_digit<16>(15);       // dig_t<16> con valor 15 (0xF)
   * auto d3 = make_digit<10>(42);       // dig_t<10> con valor 2 (42 % 10)
   * ```
   * 
   * @example Con diferentes tipos enteros:
   * ```cpp
   * auto d1 = make_digit<256>(255u);           // unsigned
   * auto d2 = make_digit<100>(static_cast<uint8_t>(99)); // uint8_t
   * ```
   */
  template <std::uint64_t Base, type_traits::integral_c Int_t>
    requires(Base > 1)
  constexpr dig_t<Base> make_digit(Int_t value) noexcept {
    return dig_t<Base>(value);
  }

  /** @} */ // end of make_digit group

} // namespace NumRepr

#endif // CORE_DIG_T_HPP_INCLUDED
