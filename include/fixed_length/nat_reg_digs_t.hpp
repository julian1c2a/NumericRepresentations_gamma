/**
 * @file nat_reg_digs_t.hpp
 * @brief Natural Number Register Digits Template
 *
 * Este archivo define la clase `nat_reg_digs_t`, que extiende `reg_digs_t` para
 * representar números naturales (enteros no negativos) usando representación por dígitos
 * con base y longitud fijas determinadas en tiempo de compilación.
 *
 * La clase proporciona una interfaz completa para operaciones aritméticas, comparaciones,
 * conversiones y E/O, optimizada para números naturales con representación interna
 * eficiente basada en arrays de dígitos.
 *
 * @tparam B Base numérica (debe ser > 1)
 * @tparam L Longitud del registro en dígitos (número de elementos)
 *
 * @author Julia
 * @date 2024
 * @copyright MIT License
 */

#ifndef NAT_REG_DIGS_T__HPP__INCLUDED
#define NAT_REG_DIGS_T__HPP__INCLUDED

#include "reg_digs_t.hpp"
#include "core/internal/basic_types.hpp"

namespace NumRepr
{

  using type_traits::suitable_base;
  using type_traits::uint_type_for_radix_c;

  /**
   * @class nat_reg_digs_t
   * @brief Plantilla de registro de dígitos para números naturales
   *
   * Esta clase template representa números naturales (enteros no negativos) usando
   * una representación por dígitos con base B y longitud L fijas en tiempo de compilación.
   * Extiende la funcionalidad de `reg_digs_t` añadiendo semántica específica para números
   * naturales, incluyendo operaciones aritméticas optimizadas y validaciones de rango.
   *
   * @tparam B Base numérica (2 ≤ B ≤ 2^64-1)
   * @tparam L Longitud del registro en dígitos (L > 0)
   *
   * **Características principales:**
   * - Herencia de reg_digs_t<B,L> para operaciones básicas de registro
   * - Operaciones aritméticas completas (+, -, *, /, %)
   * - Comparaciones con semántica de números naturales
   * - Constructores flexibles desde enteros, arrays y otros registros
   * - Conversiones automáticas y validaciones de rango natural
   * - E/O con formato personalizado
   * - Optimizaciones específicas para aritmética de números naturales
   *
   * **Rango representable:**
   * - Mínimo: 0
   * - Máximo: B^L - 1
   *
   * **Ejemplo de uso:**
   * @code
   * // Registro base 10, longitud 4 (rango 0-9999)
   * nat_reg_digs_t<10, 4> num{1234};
   * num += 567;  // num = 1801
   *
   * // Registro base 16, longitud 2 (rango 0-255)
   * nat_reg_digs_t<16, 2> hex{0xFF};
   * auto resultado = hex * 2; // 0x1FE, puede producir overflow
   * @endcode
   *
   * @warning Para operaciones que pueden producir overflow (resultado > B^L-1),
   *          se debe verificar el carry o usar registros de mayor longitud.
   *
   * @see reg_digs_t, dig_t, int_num_t
   * @since NumericRepresentations 1.0
   */
  template <uint64_t B, size_t L>
    requires((B > 1) && (L > 0))
  struct nat_reg_digs_t : public reg_digs_t<B, L>
  {
    /// @brief Tipo de dígito para la base B
    using dig_t = dig_t<B>;
    /// @brief Tipo entero sin signo subyacente
    using uint_t = typename dig_t::uint_t;
    /// @brief Tipo entero sin signo de siguiente tamaño
    using sig_uint_t = typename type_traits::sig_UInt_for_UInt_t<uint_t>;
    /// @brief Tipo entero con signo correspondiente
    using sig_sint_t = typename type_traits::sig_SInt_for_UInt_t<uint_t>;

    /// @brief Alias para registro base de longitud N
    template <size_t N>
      requires(N > 0)
    using base_N_t = reg_digs_t<B, N>;

    /// @brief Tipo base heredado reg_digs_t<B,L>
    using base_t = base_N_t<L>;

    /// @brief Alias para registro natural de longitud N
    template <size_t N>
      requires(N > 0)
    using nat_reg_N_digs_t = nat_reg_digs_t<B, N>;

    // =========================================================================
    // MÉTODOS FACTORY ESTÁTICOS CONSTEVAL
    // =========================================================================

  public:
    /**
     * @brief Crea un registro natural inicializado a cero en tiempo de compilación
     * @return Registro constexpr con valor 0
     * @note Función consteval para uso en contextos constantes
     */
    static consteval nat_reg_digs_t regd_0() noexcept
    {
      return nat_reg_digs_t{base_t::regd_0()};
    }

    /**
     * @brief Crea un registro natural inicializado a uno en tiempo de compilación
     * @return Registro constexpr con valor 1
     * @note Función consteval para uso en contextos constantes
     */
    static consteval nat_reg_digs_t regd_1() noexcept
    {
      return nat_reg_digs_t{base_t::regd_1()};
    }

    /**
     * @brief Crea un registro natural inicializado a B-1 en tiempo de compilación
     * @return Registro constexpr con valor B-1 (máximo dígito simple)
     * @note Función consteval para uso en contextos constantes
     */
    static consteval nat_reg_digs_t regd_Bm1() noexcept
    {
      return nat_reg_digs_t{base_t::regd_Bm1()};
    }

    /**
     * @brief Crea un registro natural inicializado a la base B en tiempo de compilación
     * @return Registro constexpr con valor B (representado como [0,1,0,...])
     * @note Función consteval para uso en contextos constantes
     */
    static consteval nat_reg_digs_t regd_B() noexcept
    {
      return nat_reg_digs_t{base_t::regd_B()};
    }

    /**
     * @brief Crea un registro natural con valor B^n en tiempo de compilación
     * @tparam n Exponente (0 ≤ n < L)
     * @return Registro constexpr con valor B^n
     * @note Función consteval para uso en contextos constantes
     */
    template <size_t n>
      requires((n >= 0) && (n < L))
    static consteval nat_reg_digs_t regd_pow_n_B() noexcept
    {
      return nat_reg_digs_t{base_t::regd_pow_n_B()};
    }

    /**
     * @brief Crea un registro natural con valor B^n - 1 en tiempo de compilación
     * @tparam n Exponente (0 ≤ n < L)
     * @return Registro constexpr con valor B^n - 1
     * @note Función consteval para uso en contextos constantes. B^n - 1 tiene todos
     *       los dígitos desde 0 hasta n-1 establecidos a B-1
     */
    template <size_t n>
      requires((n >= 0) && (n < L))
    static consteval nat_reg_digs_t regd_pow_n_B_m1() noexcept
    {
      return nat_reg_digs_t{base_t::regd_pow_n_B_m1()};
    }

    // =========================================================================
    // CONSTRUCTORES PÚBLICOS
    // =========================================================================

  public:
    /**
     * @brief Constructor por defecto - inicializa a cero
     * @note Todos los dígitos se establecen a 0. Función consteval.
     */
    consteval inline nat_reg_digs_t() noexcept : base_t{dig_t::dig_0()} {}

    /**
     * @brief Constructor desde lista de inicialización de dígitos
     * @param arg Lista de dígitos {d0, d1, d2, ...}
     * @note Los dígitos se almacenan en orden little-endian (d0 = menos significativo)
     */
    /*
    // TEMPORALMENTE COMENTADO: Constructor initializer_list causa crash compilador C1001
    constexpr inline nat_reg_digs_t(
        const std::initializer_list<dig_t> &arg) noexcept : base_t{arg} {}
    */

    /**
     * @brief Constructor variádico desde argumentos de tipo dig_t
     * @tparam Ts Tipos de los argumentos (todos deben ser dig_t)
     * @param args Secuencia de dígitos como argumentos separados
     * @note Permite construcción como nat_reg_digs_t{d0, d1, d2} directamente
     */
    template <typename... Ts>
      requires(std::is_same_v<Ts, dig_t> && ...)
    constexpr inline nat_reg_digs_t(const Ts &...args) noexcept : base_t(args...)
    {
    }

    // =========================================================================
    // FUNCIONES DE ACCESO PRIVADAS
    // =========================================================================

  private:
    // inline constexpr const base_t* const const_base_this() const noexcept {
    //   return static_cast<const base_t* const>(this);
    // } // warning : qualifiers ignored on function return type

    /**
     * @brief Obtiene una copia del registro base subyacente
     * @return Copia del objeto base_t subyacente
     * @note Función de utilidad interna para operaciones que requieren copia
     */
    inline constexpr base_t base_cpy_cthis() const noexcept
    {
      const base_t &cr_base_this{static_cast<const base_t &>(*this)};
      return cr_base_this;
    }

    // inline constexpr const base_t& base_const_ref_cthis() const noexcept {
    //   return (*const_base_this());
    // }

    /**
     * @brief Acceso de solo lectura a dígito por índice
     * @param ix Índice del dígito (0 = menos significativo)
     * @return Referencia constante al dígito en la posición ix
     * @note Función interna para acceso eficiente sin copia
     */
    inline constexpr const dig_t &const_by_index(size_t ix) const noexcept
    {
      const base_t &cr_base_this{static_cast<const base_t &>(*this)};
      return (cr_base_this[ix]);
    }

    /**
     * @brief Obtiene copia de dígito por índice
     * @param ix Índice del dígito (0 = menos significativo)
     * @return Copia del dígito en la posición ix
     * @note Función interna que devuelve copia para uso seguro
     */
    inline constexpr dig_t cpy_by_index(size_t ix) const noexcept
    {
      const base_t &cr_base_this{static_cast<const base_t &>(*this)};
      return dig_t{cr_base_this[ix]};
    }

  private:
    //  inline constexpr base_t* const base_this() noexcept {
    //    return static_cast<base_t *const>(this);
    //  } // warning : qualifiers ignored on function return type

    /**
     * @brief Obtiene referencia mutable al registro base subyacente
     * @return Referencia al objeto base_t para modificación
     * @note Función interna para operaciones que modifican el registro
     */
    inline constexpr base_t &base_ref_cthis() noexcept
    {
      base_t &r_base_cthis{static_cast<base_t &>(*this)};
      return (r_base_cthis);
    }

    /**
     * @brief Acceso mutable a dígito por índice
     * @param ix Índice del dígito (0 = menos significativo)
     * @return Referencia al dígito en la posición ix para modificación
     * @note Función interna para modificación de dígitos individuales
     */
    inline constexpr dig_t &by_index(size_t ix) noexcept
    {
      base_t &r_base_this{static_cast<base_t &>(*this)};
      return (r_base_this[ix]);
    }

    // =========================================================================
    // FUNCIONES AUXILIARES DE CONSTRUCCIÓN/COPIA
    // =========================================================================

  private:
    /**
     * @brief Función delegada para copia desde array de longitud N
     * @tparam N Longitud del array fuente
     * @param arg Array fuente a copiar
     * @note Maneja automáticamente diferencias de tamaño entre arrays.
     *       Si N < L, rellena con ceros. Si N > L, trunca al tamaño L.
     */
    template <size_t N>
      requires(N > 0)
    constexpr inline void copy_arg_N(const base_N_t<N> &arg) noexcept
    {
      ///< Z < W or Z == W

      constexpr auto Z{std::min(N, L)};
      constexpr auto W{std::max(N, L)};
      if constexpr (Z == L)
      {
        for (size_t ix{0}; ix < Z; ++ix)
          by_index(ix) = arg[ix];
      }
      else if constexpr (W == L)
      {
        for (size_t ix{0}; ix < Z; ++ix)
          by_index(ix) = arg[ix];
        for (size_t ix{Z}; ix < W; ++ix)
          by_index(ix) = dig_t::dig_0();
      }
      else
      {
        base_ref_cthis() = arg;
      }
    }

    /**
     * @brief Función delegada para movimiento desde array de longitud N
     * @tparam N Longitud del array fuente
     * @param arg Array fuente a mover (rvalue reference)
     * @note Maneja automáticamente diferencias de tamaño entre arrays.
     *       Si N < L, rellena con ceros. Si N > L, trunca al tamaño L.
     *       Usa semántica de movimiento para eficiencia.
     */
    template <size_t N>
      requires(N > 0)
    constexpr inline void move_arg_N(base_N_t<N> &&arg) noexcept
    {
      ///< Z < W or Z == W
      constexpr auto Z{std::min(N, L)};
      constexpr auto W{std::max(N, L)};
      if constexpr (Z == L)
      {
        for (size_t ix{0}; ix < Z; ++ix)
          (*this)[ix] = std::move(arg[ix]);
      }
      else if constexpr (W == L)
      {
        for (size_t ix{0}; ix < Z; ++ix)
          (*this)[ix] = std::move(arg[ix]);
        for (size_t ix{Z}; ix < W; ++ix)
          (*this)[ix] = std::move(dig_t::dig_0());
      }
      else
      {
        base_ref_cthis() = std::move(arg);
      }
    }

    /**
     * @brief Función delegada para copia desde otro nat_reg_digs_t de longitud N
     * @tparam N Longitud del registro fuente
     * @param arg Registro natural fuente
     * @note Copia el registro base subyacente directamente
     */
    template <size_t N>
      requires(N > 0)
    constexpr inline void copy_arg_N(const nat_reg_N_digs_t<N> &arg) noexcept
    {
      base_ref_cthis() = arg.base_const_ref_cthis();
    }

    /**
     * @brief Función delegada para movimiento desde otro nat_reg_digs_t de longitud N
     * @tparam N Longitud del registro fuente
     * @param arg Registro natural fuente (rvalue reference)
     * @note Mueve el registro base subyacente directamente
     */
    template <size_t N>
      requires(N > 0)
    constexpr inline void move_arg_N(nat_reg_N_digs_t<N> &&arg) noexcept
    {
      base_ref_cthis() = std::move(arg.base_const_ref_cthis());
    }

    // =========================================================================
    // CONSTRUCTORES DESDE ARRAYS Y REGISTROS
    // =========================================================================

  public:
    /**
     * @brief Constructor de copia desde array de dígitos
     * @tparam N Longitud del array fuente
     * @param arg Array de dígitos fuente
     * @note Maneja automáticamente conversión de tamaño
     */
    template <size_t N>
      requires(N > 0)
    constexpr inline nat_reg_digs_t(const base_N_t<N> &arg) noexcept
        : base_t{copy_arg_N<N>(arg)}
    {
    }

    /**
     * @brief Constructor de movimiento desde array de dígitos
     * @tparam N Longitud del array fuente
     * @param arg Array de dígitos fuente (rvalue reference)
     * @note Usa semántica de movimiento para eficiencia
     */
    template <size_t N>
      requires(N > 0)
    constexpr inline nat_reg_digs_t(const base_N_t<N> &&arg) noexcept
        : base_t{}
    {
      move_arg_N<N>(std::move(const_cast<base_N_t<N> &>(arg)));
    }

    // =========================================================================
    // CONSTRUCTORES DESDE ENTEROS
    // =========================================================================

  public:
    /**
     * @brief Constructor desde argumentos enteros variádicos
     * @tparam Ints_type Tipos enteros (deducidos automáticamente)
     * @param dig_pow_i Secuencia de valores enteros a normalizar
     * @note Los valores se normalizan automáticamente a la base B
     */
    /*
    // TEMPORALMENTE COMENTADO: Constructor variádico causa crash compilador C1001
    template <type_traits::integral_c... Ints_type>
      requires((sizeof...(Ints_type)) == L)
    constexpr inline nat_reg_digs_t(Ints_type... dig_pow_i) noexcept
        : base_t{(base_t::template normalize<Ints_type...>(dig_pow_i...))}
    {
    }
    */

    /**
     * @brief Constructor de copia desde otro registro natural
     * @tparam N Longitud del registro fuente
     * @param arg Registro natural fuente
     * @note Permite conversión entre registros de diferentes longitudes
     */
    template <size_t N>
      requires(N > 0)
    constexpr inline nat_reg_digs_t(const nat_reg_N_digs_t<N> &arg) noexcept
        : base_t{copy_arg_N<N>(arg.base_const_ref_cthis())}
    {
    }

    /**
     * @brief Constructor de movimiento desde otro registro natural
     * @tparam N Longitud del registro fuente
     * @param arg Registro natural fuente (rvalue reference)
     * @note Usa semántica de movimiento para eficiencia
     */
    template <size_t N>
      requires(N > 0)
    constexpr inline nat_reg_digs_t(nat_reg_N_digs_t<N> &&arg) noexcept
        : base_t{move_arg_N(std::move(*(arg.base_this())))}
    {
    }

    // =========================================================================
    // OPERADORES DE ASIGNACIÓN
    // =========================================================================

    /**
     * @brief Operador de asignación desde array base constante
     * @tparam N Longitud del array fuente
     * @param arg Array base fuente (const reference)
     * @return Referencia constante a este objeto
     * @note Incluye verificación de auto-asignación
     */
    template <size_t N>
      requires(N > 0)
    constexpr inline const nat_reg_digs_t &
    operator=(const base_N_t<N> &arg) noexcept
    {
      const base_t *const ptr_base_this{static_cast<const base_t *const>(this)};
      if (ptr_base_this != &arg)
        copy_arg_N<N>(arg);
      return (*this);
    }

    /**
     * @brief Operador de asignación desde array base mutable
     * @tparam N Longitud del array fuente
     * @param arg Array base fuente (mutable reference)
     * @return Referencia a este objeto
     * @note Incluye verificación de auto-asignación
     */
    template <size_t N>
      requires(N > 0)
    constexpr inline nat_reg_digs_t &operator=(base_N_t<N> &arg) noexcept
    {
      const base_t *const ptr_base_this{static_cast<const base_t *const>(this)};
      if (ptr_base_this != &arg)
        copy_arg_N<N>(arg);
      return (*this);
    }

    /**
     * @brief Operador de asignación por movimiento desde array base
     * @tparam N Longitud del array fuente
     * @param arg Array base fuente (rvalue reference)
     * @return Referencia constante a este objeto
     * @note Incluye verificación de auto-asignación y usa semántica de movimiento
     */
    template <size_t N>
      requires(N > 0)
    constexpr inline const nat_reg_digs_t &operator=(base_N_t<N> &&arg) noexcept
    {
      const base_t *const ptr_base_this{static_cast<const base_t *const>(this)};
      if (ptr_base_this != &arg)
        move_arg_N<N>(std::move(arg));
      return (*this);
    }

    /// OPERACIÓN ASIGNACIÓN POR COPIA REFERENCIA EN LA QUE SE PUEDE COPIAR
    template <size_t N>
      requires(N > 0)
    constexpr inline nat_reg_digs_t &
    operator=(const nat_reg_N_digs_t<N> &arg) noexcept
    {
      base_t *const ptr_base_this{static_cast<base_t *const>(this)};
      if (this != &arg)
        (*ptr_base_this) = arg.base_const_ref_cthis();
      return (*this);
    }

    /// OPERACIÓN ASIGNACIÓN POR MOVIMIENTO
    template <size_t N>
      requires(N > 0)
    constexpr inline nat_reg_digs_t &
    operator=(nat_reg_N_digs_t<N> &&arg) noexcept
    {
      base_t *const ptr_base_this{static_cast<base_t *const>(this)};
      if (this != &arg)
        (*ptr_base_this) = std::move(*(arg.base_this()));
      return (*this);
    }

    /// OPERACIÓN ASIGNACIÓN POR COPIA EN LA QUE  _NO_  SE PUEDE COPIAR
    template <size_t N>
      requires(N > 0)
    constexpr inline const nat_reg_digs_t &
    operator=(const nat_reg_N_digs_t<N> &arg) noexcept
    {
      base_t *const ptr_base_this{static_cast<base_t *const>(this)};
      if (this != &arg)
        (*ptr_base_this) = arg.base_const_ref_cthis();
      return (*this);
    }

  public:
    template <size_t N>
      requires(N > 0)
    static constexpr inline void set_0(base_N_t<N> &arg) noexcept
    {
      for (dig_t &dig : arg)
        dig = dig_t::dig_0();
    }

    template <size_t N>
      requires(N > 0)
    static constexpr inline void set_1(base_N_t<N> &arg) noexcept
    {
      arg[0].set_1();
      for (size_t ix{1}; ix < N; ++ix)
        arg[ix] = dig_t::dig_0();
    }

    template <size_t N>
      requires(N > 0)
    static constexpr inline void set_Bm1(base_N_t<N> &arg) noexcept
    {
      arg[0].set_Bm1();
      for (size_t ix{1}; ix < N; ++ix)
        arg[ix] = dig_t::dig_0();
    }

    template <size_t N>
      requires(N > 0)
    static constexpr inline void set_dig(base_N_t<N> &larg, dig_t d) noexcept
    {
      larg[0] = d;
      for (size_t ix{1}; ix < N; ++ix)
        larg[ix] = dig_t::dig_0();
    }

    template <size_t N>
      requires(N > 0)
    static constexpr inline void set_fill_dig(base_N_t<N> &larg, dig_t d) noexcept
    {
      for (auto &elem : larg)
        elem = d;
    }

    template <size_t N>
      requires(N > 0)
    static constexpr inline void set_fill_1(base_N_t<N> &larg) noexcept
    {
      for (auto &elem : larg)
        elem.set_1();
    }

    template <size_t N>
      requires(N > 0)
    static constexpr inline void set_fill_Bm1(base_N_t<N> &larg) noexcept
    {
      for (auto &elem : larg)
        elem.set_Bm1();
    }

    /// OPERACIÓN DE PONER A VALOR DIG_0 DEL ARRAY
    ///	DESDE [N_0 , N_1) EN BASE_N_T<N>
    template <size_t N, size_t N_i, size_t N_pf> // i es inicio
                                                 // pf es pasado el final
      requires((N_i < N_pf) && (N_pf <= N) && (N > 0))
    static constexpr inline void set_interval_0(base_N_t<N> &larg) noexcept
    {
      for (size_t ix{N_i}; ix < N_pf; ++ix)
        larg[ix] = dig_t::dig_0();
    }

    /// OPERACIÓN DE PONER A VALOR DIG_Bm1 DEL ARRAY
    ///	DESDE [N_0 , N_1) EN BASE_N_T<N>
    template <size_t N, size_t N_i, size_t N_pf> // i es inicio
                                                 // pf es pasado el final
      requires((N_i < N_pf) && (N_pf <= N) && (N > 0))
    static constexpr inline void set_interval_Bm1(base_N_t<N> &larg) noexcept
    {
      for (size_t ix{N_i}; ix < N_pf; ++ix)
        larg[ix] = dig_t::dig_Bm1();
    }

    /// OPERACIÓN DE PONER A VALOR DIG DEL ARRAY
    ///	DESDE [N_0 , N_1) EN BASE_N_T<N>
    template <size_t N, size_t N_i, size_t N_pf> // i es inicio
                                                 // pf es pasado el final
      requires((N_i < N_pf) && (N_pf <= N) && (N > 0))
    static constexpr inline void set_interval_dig(base_N_t<N> &larg, dig_t dig) noexcept
    {
      for (size_t ix{N_i}; ix < N_pf; ++ix)
        larg[ix] = dig;
    }

    /// OPERACIÓN DE PONER A VALOR DIG_0 DEL ARRAY
    ///	DESDE [N_0 , N_1) EN BASE_T
    /// ESPECIALIZACIÓN PARA N==L
    template <size_t N_i, size_t N_pf> // i es inicio
                                       // pf es pasado el final
      requires((N_i < N_pf) && (N_pf <= L))
    static constexpr inline void set_interval_0(base_t &larg) noexcept
    {
      for (size_t ix{N_i}; ix < N_pf; ++ix)
        larg[ix] = dig_t::dig_0();
    }

    /// OPERACIÓN DE PONER A VALOR DIG_Bm1 DEL ARRAY
    ///	DESDE [N_0 , N_1) EN BASE_T
    /// ESPECIALIZACIÓN PARA N==L
    template <size_t N_i, size_t N_pf> // i es inicio
                                       // pf es pasado el final
      requires((N_i < N_pf) && (N_pf <= L))
    static constexpr inline void set_interval_Bm1(base_t &larg) noexcept
    {
      for (size_t ix{N_i}; ix < N_pf; ++ix)
        larg[ix] = dig_t::dig_Bm1();
    }

    /// OPERACIÓN DE PONER A VALOR DIG DEL ARRAY
    ///	DESDE [N_0 , N_1) EN BASE_T
    /// ESPECIALIZACIÓN PARA N==L
    template <size_t N_i, size_t N_pf> // i es inicio
                                       // pf es pasado el final
      requires((N_i < N_pf) && (N_pf <= L))
    static constexpr inline void set_interval_dig(base_t &larg, dig_t dig) noexcept
    {
      for (size_t ix{N_i}; ix < N_pf; ++ix)
        larg[ix] = dig;
    }

  public:
    /// OPERACIÓN DE LIMPIEZA A CERO DEL NAT_REG_DIGS_T
    constexpr inline void set_0() noexcept { set_0(base_ref_cthis()); }

    /// OPERACIÓN DE LIMPIEZA A B-1 DEL NAT_REG_DIGS_T
    constexpr inline void set_Bm1() noexcept { set_Bm1(base_ref_cthis()); }

    /// OPERACIÓN DE LIMPIEZA A VALOR DIG_T DEL NAT_REG_DIGS_T
    constexpr inline void set_dig(dig_t arg) noexcept
    {
      set_dig(base_ref_cthis(), arg);
    }

    /// OPERACIÓN DE PONER A VALOR DIG_0 EN [N_0 , N_1) DEL NAT_REG_DIGS_T
    template <size_t N_i, size_t N_pf> // i es inicio
                                       // pf es pasado el final
      requires((N_i < N_pf) && (N_pf <= L))
    constexpr inline void set_interval_0() noexcept
    {
      set_interval_0<N_i, N_pf>(base_ref_cthis());
    }
    /// OPERACIÓN DE PONER A VALOR DIG_Bm1 EN [N_0 , N_1) DEL NAT_REG_DIGS_T
    template <size_t N_i, size_t N_pf> // i es inicio
                                       // pf es pasado el final
      requires((N_i < N_pf) && (N_pf <= L))
    constexpr inline void set_interval_Bm1() noexcept
    {
      set_interval_Bm1<N_i, N_pf>(base_ref_cthis());
    }
    /// OPERACIÓN DE PONER A VALOR DIG EN [N_0 , N_1) DEL NAT_REG_DIGS_T
    template <size_t N_i, size_t N_pf> // i es inicio
                                       // pf es pasado el final
      requires((N_i < N_pf) && (N_pf <= L))
    constexpr inline void set_interval_dig(dig_t dig) noexcept
    {
      set_interval_0<N_i, N_pf>(base_ref_cthis(), dig);
    }

    /// OPERACIÓN COPIA DESDE UN DIGITO (CONVERSIÓN)
    constexpr inline nat_reg_digs_t &operator=(const dig_t &arg) noexcept
    {
      if (&const_by_index(0) != &arg)
      {
        set_0();
        by_index(0) = arg;
      }
      return (*this);
    }

  private:
    /// OPERACIÓN COPIA DESDE UN ENTERO (CONVERSIÓN A LA BASE B) A UN BASE_T
    template <type_traits::integral_c Int_Type>
    static constexpr inline const base_t &assign(base_t &larg, Int_Type arg) noexcept
    {
      using type_traits::maxbase;
      constexpr sint64_t B_128t_v{static_cast<sint64_t>(B)};
      constexpr bool puede_multiplicarse{(maxbase<sint64_t>() / B_128t_v) > 0};
      if ((&larg) != (&arg))
      {
        sint64_t creg_g{static_cast<sint64_t>(arg)};
        sint64_t BasePowIx{B_128t_v};
        for (size_t k{1u}; k < L; ++k)
        {
          if constexpr (puede_multiplicarse)
            BasePowIx *= B_128t_v;
          else
            break;
        }
        while (creg_g < 0)
        {
          creg_g += BasePowIx;
        }
        for (size_t k{0u}; k < L; ++k)
        {
          larg[k] = dig_t(creg_g % B_128t_v);
          creg_g /= B_128t_v;
        }
      }
      return (larg);
    }

  public:
    /// NAT_REG_N_T<L+M> NAT_REG_T::CAT(NAT_REG_N_T<M>)
    /// NAT_REG_N_T<L+M> NAT_REG_T::CAT_INV(NAT_REG_N_T<M>)
    /// NAT_REG_N_T<L+1> NAT_REG_T::CAT(DIG_T)
    /// NAT_REG_N_T<L+1> NAT_REG_T::CAT_INV(DIG_T)

    /// SUBREPR => SUB REPRESENTACION

    template <size_t N>
      requires(N > 0)
    constexpr inline nat_reg_N_digs_t<N + L> cat(const nat_reg_N_digs_t<N> &arg) const noexcept
    {
      return (concat<L, N>(base_cpy_cthis(), arg));
    }

    constexpr inline nat_reg_N_digs_t<L + 1> cat(dig_t arg) const noexcept
    {
      return (concat<L>(base_cpy_cthis(), arg));
    }

    template <size_t N>
      requires(N > 0)
    constexpr inline nat_reg_N_digs_t<N + L> cat_inv(const nat_reg_N_digs_t<N> &arg) const noexcept
    {
      return (concat<N, L>(arg, base_cpy_cthis()));
    }

    constexpr inline nat_reg_N_digs_t<L + 1> cat_inv(dig_t arg) const noexcept
    {
      return (concat<L>(arg, base_cpy_cthis()));
    }

    template <size_t ibegin, size_t iend>
      requires((iend <= L) && (ibegin < iend))
    constexpr inline nat_reg_N_digs_t<iend - ibegin> subrepr() const noexcept
    {
      return subregister<L, ibegin, iend>(base_cpy_cthis());
    }

    /****************************************/
    /*							      	  */
    /*       Algunas Conversiones    		  */
    /*							      	  */
    /****************************************/

    template <type_traits::integral_c Int_Type>
    constexpr inline explicit operator Int_Type() const noexcept
    {
      using type_traits::maxbase;
      uint64_t retInt{0};
      uint64_t BasePowIx{1};
      for (size_t k{0}; k < L; ++k)
      {
        retInt += const_by_index(k) * BasePowIx;
        BasePowIx *= B;
        if (
            (k + 1 < L) &&
            (maxbase<Int_Type>() < retInt + (const_by_index(k + 1) * BasePowIx)))
        {
          return retInt;
        }
      }
      return retInt;
    }

  public:
    /****************************************************/
    /*							    				  */
    /* OPERADORES COMPARATIVOS                          */
    /*							    				  */
    /****************************************************/

    /// OPERADOR COMPARACIÓN SPACESHIP C++20
    template <size_t N>
      requires(N > 0)
    constexpr inline std::strong_ordering operator<=>(const nat_reg_N_digs_t<N> &arg) const
        noexcept
    {
      const base_t &cr_base_this_this{static_cast<const base_t &>(*this)};
      const base_t &cr_base_this_arg{static_cast<const base_t &>(arg)};
      return (cr_base_this_this <=> cr_base_this_arg);
    }

    /// OPERADOR COMPARACIÓN SPACESHIP C++20
    template <size_t N>
      requires(N > 0)
    constexpr inline std::weak_ordering operator<=>(const base_N_t<N> &arg) const noexcept
    {
      const base_t &cr_base_this_this{static_cast<const base_t &>(*this)};
      return (cr_base_this_this <=> arg);
    }

    /// OPERADOR COMPARACIÓN SPACESHIP C++20
    constexpr inline std::weak_ordering operator<=>(const dig_t &arg) const noexcept
    {
      const base_t &cr_base_this_this{static_cast<const base_t &>(*this)};
      return (cr_base_this_this <=> arg);
    }

    /********************************/
    /*							  */
    /* 		  PRIMER DIGITO	 	  */
    /*		  SEGUNDO DIGITO	  */
    /*							  */
    /********************************/

    ///  HEREDADO DE REG_DIGS_T (DE LA QUE DERIVA NAT_REG_DIGS_T)
    ///  inline constexpr
    ///  dig_t operator[](size_t idx) const noexcept;
    ///
    ///  inline constexpr
    ///  dig_t& operator[](size_t idx) noexcept;

    template <type_traits::integral_c Int_Type = uint_t>
    inline constexpr Int_Type operator()(size_t idx) const noexcept
    {
      return static_cast<Int_Type>(cpy_by_index(idx)());
    }

    /*******************************/
    /*						     */
    /* OPERADORES ARITMÉTICOS	     */
    /*	POSTINCREMENTO ++(int)   */
    /*	PREINCREMENTO ++()	     */
    /*	POSTDECREMENTO --(int)   */
    /*	PREDECREMENTO ++()	     */
    /*                             */
    /*******************************/

  public:
    constexpr inline const nat_reg_digs_t &operator++() noexcept
    {
      NumRepr::m_incr(*this);
      return (*this);
    }

    constexpr inline nat_reg_digs_t operator++(int) noexcept
    {
      nat_reg_digs_t cp_this{*this};
      ++(*this);
      return cp_this;
    }

    constexpr inline const nat_reg_digs_t &operator--() noexcept
    {
      NumRepr::m_decr(*this);
      return (*this);
    }

    constexpr inline nat_reg_digs_t operator--(int) noexcept
    {
      nat_reg_digs_t cp_this{*this};
      --(*this);
      return cp_this;
    }

    /******************************/
    /*							*/
    /*    OPERADORES ARITMETICOS	*/
    /*	 C_B()  C_Bm1()  		*/
    /*	mC_B() mC_Bm1()		    */
    /*	operator!() operator-() */
    /*                            */
    /******************************/

    /// HEREDADO DE REG_DIGS_T
    //
    ///	const nat_reg_digs_t&  mC_B() noexcept
    //
    ///	const nat_reg_digs_t&  mC_Bm1() noexcept
    //
    ///	nat_reg_digs_t C_Bm1() const noexcept
    //
    ///	nat_reg_digs_t C_B() const noexcept
    //
    ///	nat_reg_digs_t operator~() const noexcept
    //
    ///	nat_reg_digs_t operator-() const noexcept

    /// BEGIN : OPERATORS >> >>= << <<= REM_B^n M_REM_B^n MER_B_N M_MER_B_N

    /// MULTIPLY BY THE BASE B (10) << <<=
    /// DIVIDE BY THE BASE B (10)	  >> >>=
    /// REMAINDER BY THE BASE B (10) REM_B_N M_REM_B_N
    /// MULTIPLICATIVE CARRY BY THE BASE B (10) MER_B_N M_MER_B_N

    ///  HEREDADO DE REG_DIGS_T
    ///  nat_reg_digs_t operator<<(size_t n) const noexcept
    //
    ///  const nat_reg_digs_t& operator<<=(size_t n) noexcept
    //
    ///  nat_reg_digs_t operator>>(size_t n) const noexcept
    //
    ///  const nat_reg_digs_t& operator>>=(size_t n) noexcept
    //
    ///  nat_reg_digs_t rem_B(size_t n) const noexcept
    //
    ///  const nat_reg_digs_t& m_rem_B(size_t n) noexcept
    //
    ///  nat_reg_digs_t mer_B(size_t n) const noexcept
    //
    ///  const nat_reg_digs_t& m_mer_B(size_t n) noexcept

    /// END   : OPERATORS >> >>= << <<= REM_B^n M_REM_B^n MER_B_N M_MER_B_N

    /// BEGIN : OPERATORS | & |= &=

    ///  HEREDADO DE REG_DIGS_T
    ///  const nat_reg_digs_t& operator|=(const nat_reg_digs_t& arg) noexcept
    //
    ///  nat_reg_digs_t operator|(const nat_reg_digs_t& arg) noexcept
    //
    ///  const nat_reg_digs_t& operator&=(const nat_reg_digs_t& arg) noexcept
    //
    ///  nat_reg_digs_t operator&(const nat_reg_digs_t& arg) noexcept

    /// END   : OPERATORS | & |= &=

    /*****************************************/
    /* OPERADORES ARITMETICOS BASICOS		   */
    /*	    nat_reg_digs_t  @  dig_t       */
    /*      nat_reg_digs_t  @= dig_t         */
    /*      nat_reg_digs_t  @  10B^n         */
    /*      nat_reg_digs_t  @= 10B^n         */
    /*****************************************/

    /******************************************/
    /*								   		*/
    /*  ARITMETICOS CON ASIGNACIÓN			*/
    /*		nat_reg_digs_t @= dig_t			*/
    /*                                        */
    /******************************************/

    /// OPERACIONES BINARIAS CON/SIN TIPO RESULTADO
    /// OPERACIONES BINARIAS SIN REPORTE DE ERROR O CARRY
    /// 	     BASE_T @ DIG_T
    /// OPERATOR BASE_T += DIG_T CON NOMBRE M_ADDITION

  public:
    constexpr inline const nat_reg_digs_t &operator+=(dig_t rarg) noexcept
    {
      m_incr_by_digit(*this, rarg);
      return (*this);
    }

    constexpr inline const nat_reg_digs_t &operator-=(dig_t rarg) noexcept
    {
      m_decr_by_digit(*this, rarg);
      return (*this);
    }

    constexpr inline const nat_reg_digs_t &operator*=(dig_t rarg) noexcept
    {
      m_mult(*this, rarg);
      return (*this);
    }

    constexpr inline const nat_reg_digs_t &operator/=(dig_t rarg) noexcept
    {
      nat_reg_N_digs_t<1u> registro_1_digito{rarg};
      auto result{fediv(*this, registro_1_digito)};
      (*this) = std::get<0>(result);
      return (*this);
    }

    constexpr inline const nat_reg_digs_t &operator%=(dig_t rarg) noexcept
    {
      nat_reg_N_digs_t<1u> registro_1_digito{rarg};
      auto result{fediv(*this, registro_1_digito)};
      (*this) = std::get<1>(result);
      return (*this);
    }

    constexpr inline nat_reg_digs_t operator+(dig_t rarg) const noexcept
    {
      return nat_reg_digs_t{std::get<0>(sum(*this, rarg))};
    }

    constexpr inline nat_reg_digs_t operator-(dig_t rarg) const noexcept
    {
      return nat_reg_digs_t{std::get<0>(subtract(*this, rarg))};
    }

    constexpr inline nat_reg_digs_t operator*(dig_t rarg) const noexcept
    {
      return nat_reg_digs_t{std::get<0>(mult(*this, rarg))};
    }

    constexpr inline nat_reg_digs_t operator/(dig_t rarg) const noexcept
    {
      nat_reg_digs_t cpy_cthis{*this};
      cpy_cthis /= rarg;
      return std::move(cpy_cthis);
    }

    constexpr inline nat_reg_digs_t operator%(dig_t rarg) const noexcept
    {
      nat_reg_digs_t cpy_cthis{*this};
      cpy_cthis %= rarg;
      return std::move(cpy_cthis);
    }

    /// OPERACIONES BINARIAS HOMÓLOGAS A LAS ANTERIORES
    /// QUE REPORTAN ERROR DE DIV BY 0, CARRY/BORROW ...
    /// 	     M_OP BASE_T DIG_T
    /// OPERATOR M_OP BASE_T DIG_T CON NOMBRE M_ADDITION

    /// RETORNA EL CARRY Y EL RESULTADO EN *THIS
    /// MEJOR QUE OPERATOR+=
    constexpr inline dig_t m_addition(dig_t rarg) noexcept
    {
      return m_incr_by_digit(*this, rarg);
    }

    /// RETORNA EL BORROW Y EL RESULTADO EN *THIS
    /// MEJOR QUE OPERATOR-=
    constexpr inline dig_t m_subtract(dig_t rarg) noexcept
    {
      return m_decr_by_digit(*this, rarg);
    }

    /// RETORNA EL CARRY Y EL RESULTADO EN *THIS
    /// MEJOR QUE OPERATOR*=
    constexpr inline dig_t m_multiplication(dig_t rarg) noexcept
    {
      return m_mult(*this, rarg);
    }

    /// MEJOR QUE /= Y %= ES FEDIV QUE NO TIENE
    /// PARALELO APROPIADO CON M_DIVISION O M_RESTO_DE_DIV
    /// SE EXPONDRÁ CON LAS OPERACIONES ENTRE EL MISMO TIPO
    /// NAT_REG_DIGS_T

    /****************************************/
    /*									  */
    /* OPERADORES ARITMETICOS               */
    /* nat_reg_digs_t @ nat_reg_digs_t	  */
    /*                            		  */
    /****************************************/

    constexpr inline const nat_reg_digs_t &operator+=(const nat_reg_digs_t &rarg) noexcept
    {
      NumRepr::m_sum(*this, rarg);
      return (*this);
    }

    constexpr inline const nat_reg_digs_t &operator-=(const nat_reg_digs_t &rarg) noexcept
    {
      NumRepr::m_subtract(*this, rarg);
      return (*this);
    }

    constexpr inline const nat_reg_digs_t &operator*=(const nat_reg_digs_t &rarg) noexcept
    {
      m_mult(*this, rarg);
      return (*this);
    }

    constexpr inline const nat_reg_digs_t &operator/=(const nat_reg_digs_t &rarg) noexcept
    {
      nat_reg_digs_t right{rarg};
      auto result{fediv(*this, right)};
      (*this) = std::get<0>(result);
      return (*this);
    }

    constexpr inline const nat_reg_digs_t &operator%=(const nat_reg_digs_t &rarg) noexcept
    {
      nat_reg_digs_t right{rarg};
      auto result{fediv(*this, right)};
      (*this) = std::get<1>(result);
      return (*this);
    }

    constexpr inline nat_reg_digs_t operator+(const nat_reg_digs_t &rarg) const noexcept
    {
      return nat_reg_digs_t{std::get<0>(sum(*this, rarg))};
    }

    constexpr inline nat_reg_digs_t operator-(const nat_reg_digs_t &rarg) const noexcept
    {
      return nat_reg_digs_t{std::get<0>(subtract(*this, rarg))};
    }

    constexpr inline nat_reg_digs_t operator*(const nat_reg_digs_t &rarg) const noexcept
    {
      return nat_reg_digs_t{std::get<0>(mult(*this, rarg))};
    }

    constexpr inline nat_reg_digs_t operator/(const nat_reg_digs_t &rarg) const noexcept
    {
      nat_reg_digs_t cpy_cthis{*this};
      cpy_cthis /= rarg;
      return std::move(cpy_cthis);
    }

    constexpr inline nat_reg_digs_t operator%(const nat_reg_digs_t &rarg) const noexcept
    {
      nat_reg_digs_t cpy_cthis{*this};
      cpy_cthis %= rarg;
      return std::move(cpy_cthis);
    }

    /// OPERACIONES BINARIAS HOMÓLOGAS A LAS ANTERIORES
    /// QUE REPORTAN ERROR DE DIV BY 0, CARRY/BORROW ...
    /// 	     M_OP BASE_T DIG_T
    /// OPERATOR M_OP BASE_T DIG_T CON NOMBRE M_ADDITION

    /// RETORNA EL CARRY Y EL RESULTADO EN *THIS
    /// MEJOR QUE OPERATOR+=
    constexpr inline dig_t m_addition(const nat_reg_digs_t &rarg) noexcept
    {
      return m_sum(*this, rarg);
    }

    /// RETORNA EL BORROW Y EL RESULTADO EN *THIS
    /// MEJOR QUE OPERATOR-=
    constexpr inline dig_t m_subtract(const nat_reg_digs_t &rarg) noexcept
    {
      return m_subtract(*this, rarg);
    }

    /// RETORNA EL CARRY Y EL RESULTADO EN *THIS
    /// MEJOR QUE OPERATOR*=
    constexpr inline nat_reg_digs_t m_multiplication(const nat_reg_digs_t &rarg) noexcept
    {
      return m_mult(*this, rarg);
    }

    /// MEJOR QUE /= Y %= ES FEDIV QUE NO TIENE
    /// PARALELO APROPIADO CON M_DIVISION O M_RESTO_DE_DIV

    constexpr inline auto fediv(const nat_reg_digs_t &rarg) const noexcept
    {
      return fediv(*this, rarg);
    }

    std::string to_string() const noexcept
    {
      std::stringstream strstr_os{};
      strstr_os << "reg_dig#";
      for (int32_t ix{L - 1}; ix > 0; --ix)
      {
        strstr_os << static_cast<sig_uint_t>((*this)(ix));
        strstr_os << ':';
      }
      strstr_os << static_cast<sig_uint_t>((*this)(0));
      strstr_os << "#B";
      strstr_os << static_cast<sig_uint_t>(B);
      return strstr_os.str();
    }

    // ===================================================================
    // MÉTODOS MATEMÁTICOS ADICIONALES (compatibles con int_reg_digs_t)
    // ===================================================================

    /// @brief Valor absoluto (identidad para números naturales)
    /// @return Copia del mismo número (siempre positivo)
    constexpr inline nat_reg_digs_t abs() const noexcept
    {
      return *this; // Los números naturales son siempre no-negativos
    }

    /// @brief Valor absoluto en place (no-op para números naturales)
    /// @return Referencia al mismo objeto
    constexpr inline const nat_reg_digs_t &abs_in_place() noexcept
    {
      return *this; // No hay cambios necesarios
    }

    /// @brief Función signo para números naturales
    /// @return 0 si es cero, 1 si es positivo
    constexpr inline int sign() const noexcept
    {
      return is_zero() ? 0 : 1;
    }

    /// @brief Verifica si el número es cero
    /// @return true si todos los dígitos son cero
    constexpr inline bool is_zero() const noexcept
    {
      for (size_t i = 0; i < L; ++i)
      {
        if ((*this)[i].get() != 0)
        {
          return false;
        }
      }
      return true;
    }

    /// @brief Verifica si el número es positivo
    /// @return true si no es cero (números naturales > 0)
    constexpr inline bool is_positive() const noexcept
    {
      return !is_zero();
    }

    /// @brief Verifica si el número es negativo
    /// @return false siempre (números naturales no son negativos)
    constexpr inline bool is_negative() const noexcept
    {
      return false;
    }

    /// @brief Valor máximo representable en esta configuración
    /// @return Número con todos los dígitos en valor máximo (B-1)
    static consteval nat_reg_digs_t max_value() noexcept
    {
      nat_reg_digs_t result;
      for (size_t i = 0; i < L; ++i)
      {
        result[i] = dig_t{B - 1}; // Cada dígito al máximo (B-1)
      }
      return result;
    }

    /// @brief Valor mínimo representable en esta configuración
    /// @return Número cero (mínimo valor natural)
    static consteval nat_reg_digs_t min_value() noexcept
    {
      return nat_reg_digs_t{}; // Constructor por defecto = todos ceros
    }

    /// @brief Intercambia contenido con otro número
    /// @param other Otro número a intercambiar
    constexpr inline void swap(nat_reg_digs_t &other) noexcept
    {
      for (size_t i = 0; i < L; ++i)
      {
        auto temp = (*this)[i];
        (*this)[i] = other[i];
        other[i] = temp;
      }
    }

    /// @brief Conversión a string con formato personalizable
    /// @param separator Separador entre dígitos (ej: "," para 1,2,3,4)
    /// @param show_base_info Si mostrar información de base y longitud
    /// @return String formateado del número
    std::string to_string_formatted(const std::string &separator = ",",
                                    bool show_base_info = false) const noexcept
    {
      std::stringstream ss;

      if (show_base_info)
      {
        ss << "[B" << B << ",L" << L << "]:";
      }

      // Mostrar dígitos desde el más significativo al menos significativo
      for (int32_t ix = static_cast<int32_t>(L) - 1; ix >= 0; --ix)
      {
        ss << static_cast<sig_uint_t>((*this)[ix].get());
        if (ix > 0 && !separator.empty())
        {
          ss << separator;
        }
      }

      return ss.str();
    }

    // ===================================================================
    // FUNCIONES FRIEND PARA COMPATIBILIDAD STL
    // ===================================================================

    /// @brief Función abs global (identidad para naturales)
    friend constexpr nat_reg_digs_t abs(const nat_reg_digs_t &x) noexcept
    {
      return x; // Valor absoluto es identidad
    }

    /// @brief Función swap global
    friend constexpr void swap(nat_reg_digs_t &a, nat_reg_digs_t &b) noexcept
    {
      a.swap(b);
    }

    /// @brief Función sign global
    friend constexpr int sign(const nat_reg_digs_t &x) noexcept
    {
      return x.sign();
    }

    /// @brief Función min global
    friend constexpr const nat_reg_digs_t &min(const nat_reg_digs_t &a, const nat_reg_digs_t &b) noexcept
    {
      return (a <= b) ? a : b;
    }

    /// @brief Función max global
    friend constexpr const nat_reg_digs_t &max(const nat_reg_digs_t &a, const nat_reg_digs_t &b) noexcept
    {
      return (a >= b) ? a : b;
    }

    /// @brief Función clamp global - limita valor entre mín y máx
    friend constexpr nat_reg_digs_t clamp(const nat_reg_digs_t &value,
                                          const nat_reg_digs_t &min_val,
                                          const nat_reg_digs_t &max_val) noexcept
    {
      if (value < min_val)
        return min_val;
      if (value > max_val)
        return max_val;
      return value;
    }
  };

  /****************************/
  /*							*/
  /* 	   ISTREAM Y OSTREAM	*/
  /*							*/
  /****************************/

  template <uint64_t Base, size_t Length>
  std::istream &
  operator>>(std::istream &is, nat_reg_digs_t<Base, Length> &arg)
  {
    enum estado_e
    {
      e0ini,
      e1r,
      e1e,
      e1g,
      e1sep,
      e1d,
      e1i,
      e2g,
      e2start,
      e2dig,
      e2dp,
      e2end,
      e2B,
      e3dig,
      e0fin
    };
    /// STRING RECOGIDO DESDE LA ENTRADA ESTANDAR CIN
    std::string sds;
    /// TIPOS A SER UTILIZADOS EN LA FUNCIÓN: NOMBRE DE TIPOS CORTOS
    using dig_t = dig_t<Base>;
    using nat_reg_digs_t = nat_reg_digs_t<Base, Length>;
    using Int_Type = typename dig_t::uint_t;
    using inttype = typename dig_t::sig_uint_t;

    /// INDICE QUE RECORRE EL STRING RECOGIDO DE ENTRADA
    size_t indice{0};
    /// VARIABLES PARA ACCIONES EN LOS ESTADOS
    Int_Type digito{0};
    size_t longitud{0};
    inttype numero_base_recogido{0};
    /// VARIABLE DE RETORNO (BINDED TO THE ARG BY REFERENCE)
    nat_reg_digs_t &numero_ret{arg};
    /// ESTADO ACTUAL
    estado_e est_act = e0ini;
    /// CARACTER QUE GUARDARA EL INDICADO POR EL INDICE DEL STRING RECOGIDO DESDE
    /// CIN
    char c{'\0'};
    /// RECOGEMOS DESDE LA ENTRADA EL STRING CON LA INFORMACION CODIFICADA
    is >> sds;
    /// MAQUINA DE ESTADOS FINITOS QUE HACE EL PARSE() DE LA ENTRADA
    do
    {
      c = sds[indice];
      switch (est_act)
      {
      case e0ini:
      {
        if (c == 'r')
        {
          est_act = e1r;
        }
        else
        {
          est_act = e0ini;
        }
      }
      break;
      case e1r:
      {
        {
          if (c == 'e')
          {
            est_act = e1e;
          }
          else
          {
            est_act = e0ini;
          }
        }
      }
      break;
      case e1e:
      {
        if (c == 'g')
        {
          est_act = e1g;
        }
        else
        {
          est_act = e0ini;
        }
      }
      break;
      case e1g:
      {
        if (c == '_')
        {
          est_act = e1sep;
        }
        else
        {
          est_act = e0ini;
        }
      }
      break;
      case e1sep:
      {
        if (c == 'd')
        {
          est_act = e1d;
        }
        else
        {
          est_act = e0ini;
        }
      }
      break;
      case e1d:
      {
        if (c == 'i')
        {
          est_act = e1i;
        }
        else
        {
          est_act = e0ini;
        }
      }
      break;
      case e1i:
      {
        if (c == 'g')
        {
          est_act = e2g;
        }
        else
        {
          est_act = e0ini;
        }
      }
      break;
      case e2g:
      {
        if (c == '#')
        {
          est_act = e2start;
        }
        else
        {
          est_act = e0ini;
        }
      }
      break;
      case e2start:
      {
        if (((c >= '0') && (c <= '9')) && (std::abs(c - '0') < Base))
        {
          est_act = e2dig;
          digito = (c - '0');
          longitud = 0;
          numero_base_recogido = 0;
          numero_ret = nat_reg_digs_t::regd_0();
        }
        else
        {
          est_act = e0ini;
          digito = 0;
          longitud = 0;
          numero_base_recogido = 0;
          numero_ret = nat_reg_digs_t::regd_0();
        }
      }
      break;
      case e2dig:
      {
        const bool dig_c{(c >= '0') && (c <= '9')};
        const bool dig_lt_Base{digito < Base};
        const bool c_es_dig_B{dig_c && dig_lt_Base};
        const bool dig_B_cdl{c_es_dig_B && (longitud < Length - 1)};
        const bool tt_dig_B_cdl{(c == ':') && dig_lt_Base &&
                                (longitud < Length - 1)};
        const bool tf_dig_B_cdl{(c == '#') && dig_lt_Base &&
                                (longitud == Length - 1)};
        if (dig_B_cdl)
        {
          digito *= 10;
          digito += (c - '0');
          est_act = e2dig;
        }
        else if (tt_dig_B_cdl)
        {
          est_act = e2dp;
        }
        else if (tf_dig_B_cdl)
        {
          est_act = e2end;
        }
        else
        {
          est_act = e0ini;
          digito = 0;
          longitud = 0;
          numero_base_recogido = 0;
          numero_ret = nat_reg_digs_t::regd_0();
        }
      }
      break;
      case e2dp:
      {
        const bool dig_c{(c >= '0') && (c <= '9')};
        const bool c_es_dig_B{(std::abs(c - '0') < Base) && (digito < Base)};
        const bool dig_B_cdl{dig_c && c_es_dig_B && (longitud < Length - 1)};
        const bool rdig_B_cdl{dig_c && c_es_dig_B && dig_B_cdl};
        if (rdig_B_cdl)
        {
          est_act = e2dig;
          numero_ret[Length - 1 - longitud] = dig_t(digito);
          ++longitud;
          digito = c - '0';
        }
        else
        {
          est_act = e0ini;
          digito = 0;
          longitud = 0;
          numero_base_recogido = 0;
          indice = 0;
          numero_ret = nat_reg_digs_t::regd_0();
        }
      }
      break;
      case e2end:
      {
        const bool c_es_sepfin{c == 'B'};
        const bool d_es_digB{digito < Base};
        const bool l_coincide{longitud == Length - 1}; // llega al cero ?
        const bool rdigB_cdl{c_es_sepfin && d_es_digB && l_coincide};
        if (rdigB_cdl)
        {
          est_act = e2B;
          numero_ret[Length - 1 - longitud] = dig_t(digito);
          digito = 0;
          longitud = 0;
        }
        else
        {
          est_act = e0ini;
          indice = 0;
          digito = 0;
          longitud = 0;
          numero_ret = nat_reg_digs_t::regd_0();
        }
      }
      break;
      case e2B:
      {
        if ((c >= '0') && (c <= '9'))
        {
          est_act = e3dig;
          numero_base_recogido = c - '0';
          digito = 0;
        }
        else
        {
          est_act = e0ini;
          digito = 0;
          longitud = 0;
          numero_base_recogido = 0;
          numero_ret = nat_reg_digs_t::regd_0();
        }
      }
      break;
      case e3dig:
      {
        const bool espacio_c{c < 16};
        const bool digito_c{(c >= '0') && (c <= '9')};
        const bool num_base_lt_Base{numero_base_recogido < Base};
        const bool num_base_eq_Base{numero_base_recogido == Base};
        if (digito_c && num_base_lt_Base)
        {
          est_act = e3dig;
          numero_base_recogido *= 10;
          numero_base_recogido += (c - '0');
        }
        else if (espacio_c && num_base_eq_Base)
        {
          numero_base_recogido *= 10;
          numero_base_recogido += (c - '0');
          est_act = e0fin;
          numero_base_recogido = 0;
        }
        else
        {
          est_act = e0ini;
          numero_base_recogido = 0;
        }
      }
      break;
      case e0fin:
      {
        est_act = e0fin;
      }
      }
      ++indice;
    } while (est_act != e0fin);

    return (is);
  }

  template <uint64_t Base, size_t Long>
  std::ostream &operator<<(
      std::ostream &os,
      const nat_reg_digs_t<Base, Long> &arg)
  {
    os << arg.to_string();
    return (os);
  }

  // ===================================================================
  // FUNCIONES UTILITARIAS GLOBALES
  // ===================================================================

  /// @brief Máximo Común Divisor usando algoritmo de Euclides
  /// @param a Primer número
  /// @param b Segundo número
  /// @return MCD de a y b
  /// @note COMENTADO: Problemas con operator% y fediv
  /*
  template <std::uint64_t Base, std::size_t Length>
  constexpr nat_reg_digs_t<Base, Length> gcd(nat_reg_digs_t<Base, Length> a,
                                             nat_reg_digs_t<Base, Length> b)
  {
    // Algoritmo de Euclides
    while (!b.is_zero())
    {
      auto temp = b;
      b = a % b;
      a = temp;
    }
    return a;
  }

  /// @brief Mínimo Común Múltiplo
  /// @param a Primer número
  /// @param b Segundo número
  /// @return MCM de a y b
  /// @note COMENTADO: Depende de gcd que tiene problemas
  template <std::uint64_t Base, std::size_t Length>
  constexpr nat_reg_digs_t<Base, Length> lcm(const nat_reg_digs_t<Base, Length> &a,
                                             const nat_reg_digs_t<Base, Length> &b)
  {
    if (a.is_zero() || b.is_zero())
    {
      return nat_reg_digs_t<Base, Length>{}; // LCM(0, x) = 0
    }

    auto mcd = gcd(a, b);
    return (a / mcd) * b; // LCM = (a*b)/GCD(a,b) reordenado para evitar overflow
  }
  */

  /// @brief Potenciación entera eficiente (exponenciación por cuadrados)
  /// @param base Base de la potenciación
  /// @param exponent Exponente (entero no negativo)
  /// @return base^exponent
  template <std::uint64_t Base, std::size_t Length>
  constexpr nat_reg_digs_t<Base, Length> power(const nat_reg_digs_t<Base, Length> &base,
                                               std::size_t exponent)
  {
    if (exponent == 0)
    {
      nat_reg_digs_t<Base, Length> one{};
      one[0] = typename nat_reg_digs_t<Base, Length>::dig_t{1};
      return one;
    }

    if (exponent == 1)
    {
      return base;
    }

    // Exponenciación por cuadrados
    nat_reg_digs_t<Base, Length> result{};
    result[0] = typename nat_reg_digs_t<Base, Length>::dig_t{1}; // result = 1
    nat_reg_digs_t<Base, Length> current_base = base;

    while (exponent > 0)
    {
      if (exponent % 2 == 1)
      { // Si exponente es impar
        result = result * current_base;
      }
      current_base = current_base * current_base; // base = base²
      exponent /= 2;
    }

    return result;
  }

  /// @brief Crear nat_reg_digs_t desde string decimal
  /// @param str String con representación decimal del número
  /// @return Número creado desde el string
  template <std::uint64_t Base, std::size_t Length>
  nat_reg_digs_t<Base, Length> from_string(const std::string &str)
  {
    nat_reg_digs_t<Base, Length> result{};
    nat_reg_digs_t<Base, Length> base_power{};
    base_power[0] = typename nat_reg_digs_t<Base, Length>::dig_t{1}; // power = 1

    // Procesar dígitos desde el menos significativo
    for (auto it = str.rbegin(); it != str.rend(); ++it)
    {
      char c = *it;
      if (c >= '0' && c <= '9')
      {
        uint64_t digit_val = c - '0';
        if (digit_val < Base)
        { // Validar que el dígito sea válido para la base
          nat_reg_digs_t<Base, Length> digit_contrib{};
          digit_contrib[0] = typename nat_reg_digs_t<Base, Length>::dig_t{digit_val};
          result = result + (digit_contrib * base_power);

          // Actualizar potencia de la base
          nat_reg_digs_t<Base, Length> base_val{};
          base_val[0] = typename nat_reg_digs_t<Base, Length>::dig_t{Base};
          base_power = base_power * base_val;
        }
      }
      // Ignorar caracteres no numéricos (espacios, comas, etc.)
    }

    return result;
  }

  /// @brief Verificar si un número es potencia de la base
  /// @param num Número a verificar
  /// @return true si num es potencia de Base (1, Base, Base², Base³, ...)
  template <std::uint64_t Base, std::size_t Length>
  constexpr bool is_power_of_base(const nat_reg_digs_t<Base, Length> &num)
  {
    if (num.is_zero())
    {
      return false; // 0 no es potencia de ninguna base
    }

    // Verificar si es 1 (Base^0)
    if (num[0].get() == 1)
    {
      for (size_t i = 1; i < Length; ++i)
      {
        if (num[i].get() != 0)
        {
          return false;
        }
      }
      return true; // num == 1
    }

    // Buscar el primer dígito no cero (debe ser 1 y estar en posición múltiplo de log_Base)
    size_t first_nonzero_pos = Length;
    for (size_t i = 0; i < Length; ++i)
    {
      if (num[i].get() != 0)
      {
        if (num[i].get() != 1)
        {
          return false; // Primer dígito no cero debe ser 1
        }
        first_nonzero_pos = i;
        break;
      }
    }

    // Verificar que todos los demás dígitos sean cero
    for (size_t i = first_nonzero_pos + 1; i < Length; ++i)
    {
      if (num[i].get() != 0)
      {
        return false;
      }
    }

    return true; // Es potencia de la base
  }

} // namespace NumRepr
#endif // NAT_REG_DIGS_T__HPP__INCLUDED
