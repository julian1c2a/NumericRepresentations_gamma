#ifndef REG_DIGS_T_HPP_INCLUDED
#define REG_DIGS_T_HPP_INCLUDED

#include <algorithm> // Para std::reverse, std::find_if, std::count_if
#include "core/dig_t.hpp"
#include "core/internal/basic_types.hpp"

namespace NumRepr
{ // BEGIN NAMESPACE NumRepr

  using type_traits::suitable_base;
  using type_traits::uint_type_for_radix_c;

  /**
   * @brief Registro de dígitos de longitud fija en base arbitraria
   *
   * `reg_digs_t` es una clase template que representa un registro (array) de dígitos
   * de longitud fija L en una base numérica B arbitraria. Hereda de std::array<dig_t<B>, L>
   * proporcionando todas las funcionalidades de un array STL más operaciones específicas
   * para aritmética multi-precisión.
   *
   * @tparam B Base numérica (debe ser > 1). Soporta bases desde 2 hasta 2^64-1
   * @tparam L Longitud del registro (número de dígitos, debe ser > 0)
   *
   * **CARACTERÍSTICAS PRINCIPALES:**
   *
   * - **Herencia de std::array**: Acceso completo a funcionalidades STL
   *   - Iteradores: begin(), end(), rbegin(), rend()
   *   - Acceso: operator[], at(), front(), back()
   *   - Capacidad: size(), empty(), max_size()
   *   - Operaciones: fill(), swap()
   *
   * - **Compatibilidad STL**:
   *   - Range-based for loops
   *   - Algoritmos estándar (std::reverse, std::find_if, etc.)
   *   - Contenedor secuencial completo
   *
   * - **Operaciones aritméticas**: Preparado para construcción de tipos numéricos
   *   - Comparaciones lexicográficas
   *   - Detección de patrones (potencias de B)
   *   - Construcción desde listas de inicialización
   *
   * - **Múltiples bases**: Optimizado para cualquier base numérica
   *   - Binario (B=2): Operaciones bit-level
   *   - Decimal (B=10): Aritmética decimal natural
   *   - Hexadecimal (B=16): Manipulación eficiente
   *   - Bases grandes: Hasta 2^64-1 para aplicaciones especializadas
   *
   * - **Constexpr**: Evaluación en tiempo de compilación cuando sea posible
   *
   * **EJEMPLO DE USO:**
   * ```cpp
   * // Número decimal de 4 dígitos (representa hasta 9999)
   * reg_digs_t<10, 4> decimal;
   * decimal[0] = dig_t<10>(5); // unidades
   * decimal[1] = dig_t<10>(6); // decenas
   * decimal[2] = dig_t<10>(7); // centenas
   * decimal[3] = dig_t<10>(8); // miles
   * // Representa el número 8765
   *
   * // Número binario de 8 bits
   * reg_digs_t<2, 8> binario;
   * for (size_t i = 0; i < 8; ++i) {
   *     binario[i] = dig_t<2>(i % 2);
   * }
   *
   * // Uso de algoritmos STL
   * std::reverse(decimal.begin(), decimal.end());
   *
   * // Range-based for
   * for (const auto& digit : binario) {
   *     std::cout << digit.get() << " ";
   * }
   * ```
   *
   * **CONVENCIÓN DE INDEXADO:**
   * - Índice 0: Dígito menos significativo (unidades)
   * - Índice L-1: Dígito más significativo
   * - Orden little-endian para facilitar aritmética
   *
   * **VALIDACIÓN:**
   * - Exhaustivamente probado en GCC 15.2.0 y Clang 21.1.4
   * - 28+ test cases cubriendo todas las operaciones básicas
   * - Validado en múltiples bases (2, 10, 16, 100, 1000, 65536)
   * - Probado con tamaños extremos (1 a 100+ elementos)
   *
   * @see dig_t Para la implementación de dígitos individuales
   * @see nat_num_t, int_num_t Para tipos numéricos que usan reg_digs_t
   */
  template <uint64_t B, size_t L>
    requires((B > 1) && (L > 0))
  struct reg_digs_t : public std::array<dig_t<B>, L>
  {
  public:
    // ========================================
    // TIPOS Y ALIAS FUNDAMENTALES
    // ========================================

    /** @brief Tipo entero sin signo usado por los dígitos individuales */
    using UINT_T = typename dig_t<B>::uint_t;

    /** @brief Tipo de dígito individual en base B */
    using dig_t = dig_t<B>;

    /**
     * @brief Array de dígitos base B de longitud arbitraria N
     * @tparam N Longitud del array (debe ser > 0)
     */
    template <size_t N>
      requires(N > 0)
    using base_N_t = std::array<dig_t, N>;

    /**
     * @brief Tipo base del que hereda reg_digs_t
     * Equivale a std::array<dig_t<B>, L>
     */
    using base_t = base_N_t<L>;

    /**
     * @brief Tipo hermano con diferente longitud
     * Permite crear registros de la misma base pero distinto tamaño
     * @tparam N Nueva longitud del registro
     */
    template <size_t N>
    using reg_N_digs_t = reg_digs_t<B, N>;

  private:
    // ========================================
    // MÉTODOS INTERNOS DE CONVERSIÓN Y ACCESO
    // ========================================

    /**
     * @brief Obtiene puntero mutable a la clase base std::array
     * @return Puntero a base_t para acceso directo al array subyacente
     * @note Usado internamente para operaciones que requieren acceso directo al array
     */
    constexpr inline base_t *base_this() noexcept
    {
      return static_cast<base_t *>(this);
    }

    /**
     * @brief Obtiene referencia mutable a la clase base
     * @return Referencia a base_t para operaciones de modificación
     */
    constexpr inline base_t &r_base_cthis() noexcept { return (*base_this()); }

    /**
     * @brief Obtiene copia de la clase base
     * @return Copia del std::array subyacente
     * @note Útil para operaciones que necesitan una copia del array sin la semántica de reg_digs_t
     */
    constexpr inline base_t cp_base_cthis() const noexcept
    {
      const base_t &base_cthis{static_cast<const base_t &>(*this)};
      return std::move(base_t{base_cthis});
    }

    // ========================================
    // ACCESO A LA INSTANCIA ACTUAL
    // ========================================

    /** @brief Obtiene referencia mutable a this */
    constexpr inline reg_digs_t &r_cthis() noexcept { return (*this); }

    /** @brief Obtiene referencia const a this */
    constexpr inline const reg_digs_t &cr_cthis() const noexcept
    {
      return (*this);
    }

    /**
     * @brief Obtiene copia de la instancia actual
     * @return Nueva instancia reg_digs_t idéntica a this
     */
    constexpr inline reg_digs_t cp_cthis() const noexcept
    {
      return reg_digs_t{*this};
    }

    // ========================================
    // ACCESO A ELEMENTOS INDIVIDUALES
    // ========================================

    /**
     * @brief Obtiene copia del dígito en la posición k
     * @param k Índice del dígito (0 = menos significativo)
     * @return Copia del dígito en la posición k
     */
    constexpr inline dig_t cp_cthis_at(size_t k) const noexcept
    {
      return cp_base_cthis()[k];
    }

    /**
     * @brief Obtiene referencia mutable al dígito en la posición k
     * @param k Índice del dígito (0 = menos significativo)
     * @return Referencia mutable al dígito en la posición k
     */
    constexpr inline dig_t &r_cthis_at(size_t k) noexcept
    {
      return (r_base_cthis()[k]);
    }

    /**
     * @brief Obtiene referencia const al dígito en la posición k
     * @param k Índice del dígito (0 = menos significativo)
     * @return Referencia const al dígito en la posición k
     */
    constexpr inline const dig_t &cr_cthis_at(size_t k) const noexcept
    {
      const base_t &base_cthis{static_cast<const base_t &>(*this)};
      return (base_cthis[k]);
    }

  public:
    // ========================================
    // TIPOS ADICIONALES PARA ARITMÉTICA AVANZADA
    // ========================================

    /** @brief Tipo entero sin signo "significativo" para operaciones extendidas */
    using SIG_UINT_T = typename type_traits::sig_UInt_for_UInt_t<UINT_T>;

    /** @brief Tipo entero con signo correspondiente al tipo sin signo */
    using SIG_SINT_T = typename type_traits::sig_SInt_for_UInt_t<UINT_T>;

    // ========================================
    // CONSTANTES ESTÁTICAS DE DÍGITOS
    // ========================================

    /** @brief Dígito cero (0) */
    static consteval dig_t dig_0() noexcept { return dig_t::dig_0(); }

    /** @brief Dígito uno (1) */
    static consteval dig_t dig_1() noexcept { return dig_t::dig_1(); }

    /** @brief Dígito máximo válido (B-1) */
    static consteval dig_t dig_Bm1() noexcept { return dig_t::dig_Bm1(); }

    /** @brief Dígito penúltimo válido (B-2) */
    static consteval dig_t dig_Bm2() noexcept { return dig_t::dig_Bm2(); }

    // ========================================
    // CONSTANTES ESTÁTICAS DE ENTEROS
    // ========================================

    /** @brief Valor entero cero */
    static consteval UINT_T ui_0() noexcept { return dig_t::ui_0(); }

    /** @brief Valor entero uno */
    static consteval UINT_T ui_1() noexcept { return dig_t::ui_1(); }

    /** @brief Valor entero máximo válido para un dígito (B-1) */
    static consteval UINT_T ui_Bm1() noexcept { return B - 1; }

    /** @brief Valor de la base B */
    static consteval UINT_T ui_B() noexcept { return B; }

    /** @brief Valor entero cero con signo extendido */
    static consteval SIG_UINT_T sui_0() noexcept { return dig_t::sui_0(); }

    /** @brief Valor entero uno con signo extendido */
    static consteval SIG_UINT_T sui_1() noexcept { return dig_t::sui_1(); }

    /** @brief Valor máximo con signo extendido (B-1) */
    static consteval SIG_UINT_T sui_Bm1() noexcept { return dig_t::sui_B() - 1; }

    /** @brief Valor de la base B con signo extendido */
    static consteval SIG_UINT_T sui_B() noexcept { return dig_t::sui_B(); }

    /** @brief Valor B+1 con signo extendido */
    static consteval SIG_UINT_T sui_Bp1() noexcept { return dig_t::sui_B() + 1; }

    // ========================================
    // CONSTANTES CON SIGNO (SIGNED)
    // ========================================

    /** @brief Valor cero como entero con signo */
    static consteval SIG_SINT_T ssi_0() noexcept { return dig_t::ssi_0(); }

    /** @brief Valor uno como entero con signo */
    static consteval SIG_SINT_T ssi_1() noexcept { return dig_t::ssi_1(); }

    /** @brief Valor B-1 como entero con signo */
    static consteval SIG_SINT_T ssi_Bm1() noexcept { return dig_t::ssi_B() - 1; }

    /** @brief Valor base B como entero con signo */
    static consteval SIG_SINT_T ssi_B() noexcept { return dig_t::ssi_B(); }

    /** @brief Valor B+1 como entero con signo */
    static consteval SIG_SINT_T ssi_Bp1() noexcept { return dig_t::ssi_B() + 1; }

    // ========================================
    // GENERADORES DE REGISTROS CONSTANTE
    // ========================================

    /**
     * @brief Genera un registro de tamaño N inicializado con todos ceros
     * @tparam N Longitud del registro a generar
     * @return Registro base_N_t<N> con todos los dígitos en cero
     * @note Función consteval - se evalúa en tiempo de compilación
     */
    template <size_t N>
    static consteval base_N_t<N> regd_base_N_0() noexcept
    {
      base_N_t<N> ret;
      for (dig_t &dig : ret)
      {
        dig = dig_0();
      }
      return ret;
    }

    /**
     * @brief Genera un registro de tamaño N que representa el valor 1
     * @tparam N Longitud del registro a generar
     * @return Registro con todos ceros excepto dígito menos significativo = 1
     */
    template <size_t N>
    static consteval base_N_t<N> regd_base_N_1() noexcept
    {
      base_N_t<N> ret{regd_base_N_0<N>()};
      ret[0] = dig_1();
      return ret;
    }

    /**
     * @brief Genera un registro de tamaño N que representa el valor B-1
     * @tparam N Longitud del registro a generar
     * @return Registro con todos ceros excepto dígito menos significativo = B-1
     */
    template <size_t N>
    static consteval base_N_t<N> regd_base_N_Bm1() noexcept
    {
      base_N_t<N> ret{regd_base_N_0<N>()};
      ret[0] = dig_Bm1();
      return ret;
    }

    /**
     * @brief Genera un registro de tamaño N que representa el valor B (base)
     * @tparam N Longitud del registro a generar
     * @return Registro que representa B = 10...0 en base B
     * @note ret[1] = 1, ret[0] = 0 representa 1*B^1 + 0*B^0 = B
     */
    template <size_t N>
    static consteval base_N_t<N> regd_base_N_B() noexcept
    {
      base_N_t<N> ret{regd_base_N_0<N>()};
      ret[1] = dig_1();
      return ret;
    }

    /**
     * @brief Genera un registro que representa B^n (potencia de la base)
     * @tparam N Longitud del registro
     * @tparam n Exponente de la potencia (posición del dígito = 1)
     * @return Registro que representa B^n
     * @note ret[n] = 1, resto = 0 representa 1*B^n
     */
    template <size_t N, size_t n>
      requires((n >= 0) && (n < N))
    static consteval base_N_t<N> regd_base_N_pow_n_B() noexcept
    {
      base_N_t<N> ret{regd_base_N_0<N>()};
      ret[n] = dig_1();
      return ret;
    }

    template <size_t N, size_t n>
      requires((n >= 0) && (n < N))
    static consteval base_N_t<N> regd_base_N_pow_n_B_m1() noexcept
    {
      base_N_t<N> ret{};
      for (size_t ix{0}; ix < n; ++ix)
      {
        ret[ix] = dig_Bm1();
      }
      for (size_t ix{n}; ix < N; ++ix)
      {
        ret[ix] = dig_0();
      }
      return ret;
    }

    /// <summary>
    /// IDEM QUE ANTES PERO CON EL TIPO BASE_T
    /// ESPECIALIZANDO AL TIPO BASE_N_T<L>
    /// DONDE BASE_T == BASE_N_T<L>
    /// TENEMOS CUIDADO DE CREAR UN RVALUE TEMPORAL
    /// </summary>

    static consteval base_t regd_base_0() noexcept
    {
      return base_t{regd_base_N_0<L>()};
    }

    static consteval base_t regd_base_1() noexcept
    {
      return base_t{regd_base_N_1<L>()};
    }

    static consteval base_t regd_base_Bm1() noexcept
    {
      return base_t{regd_base_N_Bm1<L>()};
    }

    static consteval base_t regd_base_B() noexcept
    {
      return base_t{regd_base_N_B<L>()};
    }

    static consteval base_t regd_base_pow_n_B() noexcept
    {
      return base_t{regd_base_N_pow_n_B<L>()};
    }

    static consteval base_t regd_base_pow_n_B_m1() noexcept
    {
      return base_t{regd_base_N_pow_n_B_m1<L>()};
    }

    /// <summary>
    /// IDEM QUE ANTES PERO CON LA CLASE DERIVADA, LA ACTUAL
    /// TENEMOS CUIDADO DE CREAR UN RVALUE TEMPORAL
    /// </summary>

    static consteval reg_digs_t regd_0() noexcept
    {
      return reg_digs_t{regd_base_N_0<L>()};
    }

    static consteval reg_digs_t regd_1() noexcept
    {
      return reg_digs_t{regd_base_1()};
    }

    static consteval reg_digs_t regd_Bm1() noexcept
    {
      return reg_digs_t{regd_base_Bm1()};
    }

    static consteval reg_digs_t regd_B() noexcept
    {
      return reg_digs_t{regd_base_B()};
    }

    static consteval reg_digs_t regd_pow_n_B() noexcept
    {
      return reg_digs_t{regd_base_pow_n_B()};
    }

    static consteval reg_digs_t regd_pow_n_B_m1() noexcept
    {
      return reg_digs_t{regd_base_pow_n_B_m1()};
    }

  public:
    /// <summary>
    /// En heredamos como clase derivada de base_t == std::array<B,L>
    /// Tenemos todos los métodos de la STL
    /// </summary>

    // constexpr decltype(auto) begin()         noexcept
    // constexpr decltype(auto) cbegin()  const noexcept
    // constexpr decltype(auto) end()           noexcept
    // constexpr decltype(auto) cend()    const noexcept
    // constexpr decltype(auto) rbegin()        noexcept
    // constexpr decltype(auto) crbegin() const noexcept
    // constexpr decltype(auto) rend()          noexcept
    // constexpr decltype(auto) crend()   const noexcept

    // constexpr size_t size()            const noexcept
    // constexpr size_t max_size()        const noexcept
    // constexpr bool empty()             const noexcept
    // constexpr dig_t front()                  noexcept
    // constexpr dig_t const& front()     const noexcept
    // constexpr dig_t back()                   noexcept
    // constexpr dig_t const& back()      const noexcept
    // constexpr void fill(const dig_t& value)  noexcept
    // constexpr void swap(base_t& other)       noexcept

    /**
     * @brief Invierte el orden de los dígitos en el registro
     * @note Convierte entre little-endian y big-endian
     * @note Ejemplo: {1,2,3} se convierte en {3,2,1}
     * @note Útil para cambiar la representación del orden de dígitos
     */
    constexpr void reverse() noexcept
    {
      std::reverse(this->begin(), this->end());
    }

    /// <summary>
    ///
    ///
    ///
    ///    Constructores de reg_digs_t
    ///
    ///
    ///
    /// </summary>

  public:
    // ========================================
    // CONSTRUCTORES
    // ========================================

    /**
     * @brief Constructor por defecto - inicializa todos los dígitos en cero
     * @note Se evalúa en tiempo de compilación (consteval)
     * @note Representa el número 0 en cualquier base
     */
    consteval inline reg_digs_t() noexcept : base_t{regd_base_0()} {}

  private:
    /**
     * @brief Función auxiliar para construir base_t desde initializer_list
     * @param larg Lista de inicialización de dígitos
     * @return Array base_t construido desde la lista
     * @note Si larg.size() >= L, copia los primeros L elementos
     * @note Si larg.size() < L, rellena el resto con ceros
     * @note Los elementos se invierten para mantener convención little-endian
     */
    static constexpr base_t
    make_base_t(const std::initializer_list<dig_t> &larg)
    {
      base_t rarg;
      if (larg.size() >= L)
      {
        auto itlist{larg.begin()};
        const auto itlistend{larg.end()};
        auto itcthis{rarg.begin()};
        const auto itcthisend{rarg.end()};
        while ((itcthis != itcthisend) && (itlist != itlistend))
        {
          *itcthis = *itlist;
          ++itcthis;
          ++itlist;
        }
      }
      else
      {
        auto itlist{larg.begin()};
        auto itcthis{rarg.begin()};
        const auto itlistend{larg.end()};
        const auto itcthisend{rarg.end()};
        while ((itcthis != itcthisend) && (itlist != itlistend))
        {
          *itcthis = *itlist;
          ++itcthis;
          ++itlist;
        }
        while (itcthis != itcthisend)
        {
          *itcthis = dig_0();
          ++itcthis;
        }
      }
      std::reverse(rarg.begin(), rarg.end());
      return rarg;
    }

  public:
    /**
     * @brief Constructor desde lista de inicialización de dígitos
     * @param arg Lista de dígitos {d0, d1, d2, ...}
     * @note Los dígitos se almacenan en orden little-endian
     * @note Ejemplo: {dig_t<10>(5), dig_t<10>(6), dig_t<10>(7)} → representa 765
     * @note Si hay más dígitos que L, se toman solo los primeros L
     * @note Si hay menos dígitos que L, el resto se rellena con ceros
     */
    constexpr inline reg_digs_t(const std::initializer_list<dig_t> &arg) noexcept
        : base_t{make_base_t(arg)} {}

    /**
     * @brief Constructor variádico desde argumentos dig_t individuales
     * @tparam Ts Tipos de argumentos (todos deben ser dig_t)
     * @param args Dígitos individuales como argumentos separados
     * @note Permite construcción directa: reg_digs_t(dig_t(1), dig_t(2), dig_t(3))
     * @note Número de argumentos debe coincidir exactamente con L
     * @note MSVC: Requiere flags /constexpr:depth2048 /constexpr:steps1048576 /bigobj
     */
    template <typename... Ts>
      requires(std::is_same_v<Ts, dig_t> && ... && (sizeof...(Ts) == L))
    constexpr inline reg_digs_t(const Ts &...args) noexcept
    {
      static_assert(sizeof...(Ts) == L, "Number of arguments must match L");
      base_t temp_array{args...};
      static_cast<base_t &>(*this) = temp_array;
    }

    /**
     * @brief Constructor copia desde std::array base
     * @param rarg Array std::array<dig_t<B>, L> a copiar
     * @note Copia directa del array subyacente
     */
    constexpr inline reg_digs_t(const base_t &rarg) noexcept : base_t{rarg} {}

    /**
     * @brief Constructor movimiento desde std::array base
     * @param rarg Array std::array<dig_t<B>, L> a mover
     * @note Movimiento directo del array subyacente
     */
    constexpr inline reg_digs_t(base_t &&rarg) noexcept
        : base_t{std::move(rarg)} {}

    /// <summary>
    /// Constructor por Copia/Movimiento desde una
    /// sucesión variadica de dígitos dig_t
    /// </summary>

  private:
    /// <summary>
    /// Función miembro (delegada) para copiar un
    ///	objeto tipo base_t desde una referencia constante
    ///	a un array cualquiera de dig_t
    /// </summary>
    /// <function name="copy_arg_N"></function>
    /// <param name="const base_N_t<N>&  arg"></param>
    /// <returns="void"></returns>
    template <size_t N>
    constexpr inline const base_t &copy_arg_N(const base_N_t<N> &arg) noexcept
    {
      ///< Z < W or Z == W
      constexpr auto Z{std::min(N, L)};
      constexpr auto W{std::max(N, L)};
      if constexpr (Z == L)
      {
        for (size_t ix{0}; ix < Z; ++ix)
          r_cthis_at(ix) = arg[ix];
      }
      else if constexpr (W == L)
      {
        for (size_t ix{0}; ix < Z; ++ix)
          r_cthis_at(ix) = arg[ix];
        for (size_t ix{Z}; ix < W; ++ix)
          r_cthis_at(ix) = dig_0();
      }
      else
      {
        r_cthis() = arg;
      }
      return (cr_cthis());
    }

    /// <summary>
    /// Función miembro (delegada) para mover un objeto tipo base_t desde una
    /// referencia derecha a un array cualquiera de dig_t
    /// </summary>
    /// <function name="move_arg_N"></function>
    /// <param name="const base_N_t<N>&  arg"></param>
    /// <returns="void"></returns>
    template <size_t N>
    constexpr inline void move_arg_N(base_N_t<N> &&arg) noexcept
    {
      base_t &cthis{r_base_cthis()};
      ///< Z < W or Z == W
      constexpr auto Z{std::min(N, L)};
      constexpr auto W{std::max(N, L)};
      if constexpr (Z == L)
      {
        for (size_t ix{0}; ix < Z; ++ix)
          r_cthis_at(ix) = std::move(arg[ix]);
      }
      else if constexpr (W == L)
      {
        for (size_t ix{0}; ix < Z; ++ix)
          r_cthis_at(ix) = std::move(arg[ix]);
        for (size_t ix{Z}; ix < W; ++ix)
          r_cthis_at(ix) = std::move(dig_0());
      }
      else
      {
        r_cthis() = std::move(arg);
      }
    }

  public:
    /// <summary>
    /// Constructor copia desde un array cualquiera de dígitos dig_t
    /// (usando copy_arg_N<N>)
    /// </summary>
    template <size_t N>
    constexpr inline reg_digs_t(const base_N_t<N> &arg) noexcept
        : base_t{copy_arg_N<N>(arg)} {}

    /// <summary>
    /// Constructor por movimiento desde un array cualquiera de dígitos dig_t
    /// (usando move_arg_N<N>)
    /// </summary>
    template <size_t N>
    constexpr inline reg_digs_t(base_N_t<N> &&arg) noexcept
        : base_t{}
    {
      move_arg_N<N>(std::move(arg));
    }

  private:
    /// <summary="Funcion de normalizacion a dig_t (dígitos base B) que construye
    /// un objeto de tipo ">
    /// </summary>
    /// <summary="base_t desde una sucesion variadica de enteros cualquiera">
    /// </summary>
    /// <function name="normalize"></function>
    /// <param name="Ints_type ... digits_pow_i"></param>
    /// <returns="base_t"></returns>
    template <typename... Ints_type>
      requires((sizeof...(Ints_type)) == L)
    static constexpr inline base_t normalize(Ints_type... digits_pow_i) noexcept
    {
      ///< CREA UN STD_ARRAY DEL TIPO INT PASADO POR UN PACK DE ARGUMENTOS
      ///< EL TAMANO ES EL DEL PACK DE ARGUMENTOS PASADO (MENOR O IGUAL QUE L)
      using pack_type = typename utilities::ugly_pack_details::pack2array<Ints_type...>;
      ///< DEVUELVE EL TIPO INTERNO DE ELEMENTO DEL ARRAY ANTERIOR
      ///< [UN TIPO ENTERO]
      using unique_type = typename pack_type::elem_type;

      ///< DEVUELVE EL TAMANO DEL ARRAY ANTERIOR (TAMAÑO == L)

      ///< ELIGE ENTRE CUATRO TIPOS DE ENTEROS SEGUN TENGAN SIGNO O NO
      ///< Y SU TAMANO
      ///< SEA MAYOR O MENOR QUE EL PROPIO DE LA BASE UINT_T
      ///< 		TIPO PROPIO ES :
      ///< UINT_T 		TIPO PASADO COMO ARGUMENTO ES :  	UNIQUE_TYPE
      ///< LOS TIPOS PASADOS EN LOS ARGUMENTOS HAN DE SER IGUALES ENTRE SI
      ///< PARA EL INTERROGANTE CONDICIONAL UTILIZAMOS
      ///< 		        UNSIGNED_INTEGRAL_C<S>		PRIMERA PREGUNTA O MAS
      ///< EXTERIOR 		IS_UNISGNED_SZ_GT_V<T,S>	PREGUNTAS SEGUNDAS O MAS
      ///< INTERIORES PARA ELEGIR EL TIPO APROPIADO EN CADA CASO USAMOS
      ///<		SIG_UINT_FOR_UINT_T<UINT_T>
      ///<		SIG_UINT_FOR_UINT_T<UNIQUE_TYPE>
      ///<		SIG_UINT_FOR_UINT_T<UINT_T>
      ///<		SIG_UINT_FOR_SINT_T<UNIQUE_TYPE>
      using namespace NumRepr::type_traits;
      using SUInt_type = typename std::conditional_t<
          is_unsigned_type_v<UINT_T>,
          typename std::conditional_t<
              is_unsigned_type_v<unique_type>,
              typename std::conditional_t<gt_sz_v<UINT_T, unique_type>,
                                          sig_UInt_for_UInt_t<UINT_T>,
                                          sig_UInt_for_UInt_t<unique_type>>,
              typename std::conditional_t<gt_sz_v<UINT_T, unique_type>,
                                          sig_UInt_for_UInt_t<UINT_T>,
                                          sig_UInt_for_SInt_t<unique_type>>>,
          typename std::conditional_t<
              is_unsigned_type_v<unique_type>,
              typename std::conditional_t<
                  ge_sz_v<UINT_T, unique_type>,
                  sig_UInt_for_SInt_t<sig_SInt_for_UInt_t<UINT_T>>,
                  sig_UInt_for_SInt_t<sig_SInt_for_UInt_t<unique_type>>>,
              typename std::conditional_t<
                  gt_sz_v<UINT_T, unique_type>,
                  sig_UInt_for_SInt_t<sig_SInt_for_UInt_t<UINT_T>>,
                  sig_UInt_for_SInt_t<sig_SInt_for_SInt_t<unique_type>>>>>;
      std::array<SUInt_type, L> ret_array{((dig_t(digits_pow_i))())...};
      base_t ret;

      for (size_t ix{0}; ix < L; ++ix)
      {
        ret[ix] = dig_t(ret_array[ix]);
      }

      return ret;
    }

  public:
    /// <summary="Constructor copia desde una sucesión de objetos enteros
    /// variádica, normalizándolos">
    /// </summary>

    template <type_traits::integral_c... Ints_type>
      requires((sizeof...(Ints_type)) == L)
    constexpr inline reg_digs_t(Ints_type... dig_pow_i) noexcept
        : base_t((normalize<Ints_type...>((dig_t(dig_pow_i))()...)).reverse())
    {
    }

    /// <summary>
    /// Sobrecarga del operador copia
    /// </summary>

    /// OPERACION ASIGNACION POR COPIA REFERENCIA
    /// CONST _NO_ COPIABLE DESDE REG_N_DIGS_T EN LA IZQUIERDA
    template <size_t N>
    constexpr inline const reg_digs_t &
    operator=(const reg_N_digs_t<N> &arg) noexcept
    {
      if (this != &arg)
        copy_arg_N<N>(arg);
      return (*this);
    }

    /// OPERACION ASIGNACION POR COPIA REFERENCIA
    /// DESDE REG_N_DIGS_T EN LA IZQUIERDA
    template <size_t N>
    constexpr inline reg_digs_t &operator=(reg_N_digs_t<N> &arg) noexcept
    {
      if (this != &arg)
        copy_arg_N<N>(arg);
      return (*this);
    }

    /// OPERACION ASIGNACION POR MOVIMIENTO
    /// DESDE REGS_N_DIGS_T EN LA QUE NO SE PUEDE COPIAR
    template <size_t N>
    constexpr inline const base_t &operator=(reg_N_digs_t<N> &&arg) noexcept
    {
      if (this != &arg)
        move_arg_N<N>(std::move(arg));
      return (*this);
    }

    /// OPERACION COPIA DESDE UN DIGITO (CONVERSION)
    constexpr inline reg_digs_t &operator=(const dig_t &arg) noexcept
    {
      if ((&(r_cthis_at(0))) != (&arg))
      {
        r_cthis().set_0();
        r_cthis_at(0) = arg;
      }
      return (*this);
    }

    /// OPERACION COPIA DESDE UN ENTERO (CONVERSION A LA BASE B)
    /// A UN REG_DIGS_T
    template <type_traits::integral_c Int_Type>
    constexpr inline const reg_digs_t &operator=(Int_Type arg) noexcept
    {
      using type_traits::maxbase;
      using max_int_t =
          std::conditional_t<type_traits::is_unsigned_type_v<Int_Type>, /// ?
                             uint64_t,                                  /// :
                             sint64_t>;
      constexpr max_int_t B_128t_v{static_cast<max_int_t>(B)};
      constexpr bool puede_multiplicarse{(maxbase<max_int_t>() / B_128t_v) > 0};

      max_int_t creg_g{static_cast<max_int_t>(arg)};
      max_int_t BasePowIx{B_128t_v};
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
        this->r_cthis_at(k) = dig_t(static_cast<UINT_T>(creg_g % B_128t_v));
        creg_g /= B_128t_v;
      }

      return (cr_cthis());
    }

    ///
    ///       Algunas Conversiones
    ///

    /// PUEDE SER QUE NO QUEPA EN EL TIPO ENTERO
    template <type_traits::integral_c Int_Type>
    /// SE PODRÍA HACER PARA UINT128_T
    constexpr inline explicit operator Int_Type() const noexcept
    {
      const reg_digs_t &cthis{*this};
      using type_traits::maxbase;
      uint64_t retInt{0};
      uint64_t BasePowIx{1};
      for (size_t k{0}; k < L; ++k)
      {
        retInt += cthis(k) * BasePowIx;
        BasePowIx *= B;
        if ((k + 1 < L) &&
            (maxbase<Int_Type>() < (retInt + ((cthis(k + 1) * BasePowIx)))))
        {
          return retInt;
        }
      }
      return retInt;
    }

  private:
    /// for compatibility with std::array
    /// <summary=" sobrecarga de las funciones miembro : ">
    /// constexpr dig_t* data() noexcept;
    /// </summary>
    /// <returns name="dig_t*"></returns>
    /// HEREDADO DE LA STL
    /// constexpr dig_t* data() noexcept { return (this->base_t::data()); }

  private:
    /// for compatibility with std::array
    ///< summary=" sobrecarga de las funciones miembro : ">
    /// constexpr const dig_t* data() const noexcept;
    ///</summary>
    ///< returns name="const dig_t*"></returns>
    /// HEREDADO DE LA STL
    /// constexpr const dig_t* const data() const noexcept;

  public:
    inline constexpr decltype(auto) cpy_data() const noexcept
    {
      return (*(this->base_t::data()));
    }

    ///< summary="Sobrecarga del const dig_t&  operator[](size_t)
    ///< const"></summary>
    /// HEREDADA DE LA CLASE STD::ARRAY DE LA STL
    /// inline constexpr const dig_t& operator[](size_t ix) const noexcept;

    ///< summary="Sobrecarga del dig_t&  operator[](size_t)"></summary>
    /// HEREDADA DE LA CLASE STD::ARRAY DE LA STL
    /// inline constexpr dig_t& operator[](size_t ix) noexcept;

    ///< summary="Sobrecarga del const dig_t&  operator[](size_t)
    ///< const"></summary>
    inline constexpr auto operator()(size_t ix) const noexcept
    {
      return (cr_cthis_at(ix)());
    }

    ///< summary>
    /// Funciones que ponen a constantes (constexpr) los objetos base_t
    ///</summary>
    ///< typeparam name="size_t"></typeparam>
    ///< nontypeparam name="N"></nontypeparam>
    ///< param name="this"></param>
    ///< returns name="void"></returns>

    constexpr inline void set_0() noexcept
    {
      fill(dig_0());
    }

    constexpr inline void set_1() noexcept
    {
      fill(dig_0());
      (*this)[0].set_1();
    }

    constexpr inline void set_Bm1() noexcept
    {
      fill(dig_0());
      (*this)[0].set_Bm1();
    }

    constexpr inline void set_dig(dig_t d) noexcept
    {
      fill(d);
      (*this)[0] = d;
    }

    constexpr inline void set_fill_dig(dig_t d) noexcept
    {
      fill(d);
    }

    constexpr inline void set_fill_1() noexcept
    {
      fill(dig_1());
    }

    constexpr inline void set_fill_Bm1() noexcept
    {
      fill(dig_Bm1());
    }

    ///< summary>
    ///"Funciones que ponen a constantes(constexpr) los objetos un subintervalo
    /// o subrango de base_t cualquiera"
    ///</summary>
    ///< typeparam name="size_t"></typeparam>
    ///< nontypeparam name="N_i"></nontypeparam>
    ///< nontypeparam name="N_pf"></nontypeparam>
    ///< param name="this"></param>
    ///< returns name="void"></returns>

    /// OPERACION DE PONER A VALOR DIG_0 DEL ARRAY
    /// DESDE [N_0 , N_1) EN BASE_N_T<N>
    template <size_t N_i, size_t N_pf> // i  es inicio
                                       // pf es pasado el final STL
      requires((N_i < N_pf) && (N_pf <= L))
    constexpr inline void set_interval_0() noexcept
    {
      for (size_t ix{N_i}; ix < N_pf; ++ix)
        (*this)[ix].set_0();
    }

    /// OPERACION DE PONER A VALOR DIG_Bm1 DEL ARRAY
    ///	DESDE [N_0 , N_1) EN BASE_N_T<N>
    template <size_t N, size_t N_i, size_t N_pf> // i  es inicio
                                                 // pf es pasado el final
      requires((N_i < N_pf) && (N_pf <= L))
    constexpr inline void set_interval_Bm1() noexcept
    {
      for (size_t ix{N_i}; ix < N_pf; ++ix)
        (*this)[ix].set_Bm1();
    }

    /// OPERACION DE PONER A VALOR DIG DEL ARRAY
    ///	DESDE [N_0 , N_1) EN BASE_T
    template <size_t N_i, size_t N_pf> // i  es inicio
                                       // pf es pasado el final
      requires((N_i < N_pf) && (N_pf <= L))
    constexpr inline void set_interval_dig(dig_t dig) noexcept
    {
      for (size_t ix{N_i}; ix < N_pf; ++ix)
        (*this)[ix] = dig;
    }

    ///< summary="Funciones comparativas con constantes tipo constexpr"></summary>
    ///< param name="this"></param>
    ///< returns="bool"></returns>

    inline constexpr bool is_0() const noexcept
    {
      auto it{this->cbegin()};
      const auto itend{this->cend()};
      while (it != itend)
      {
        if (it->is_not_0())
          return false;
        ++it;
      }
      return true;
    }

    inline constexpr bool is_1() const noexcept
    {
      if (cr_cthis_at(0).is_not_1())
        return false;
      for (size_t ix{1}; ix < L; ++ix)
      {
        if (cr_cthis_at(ix).is_not_0())
          return false;
      }
      return true;
    }

    inline constexpr bool is_Bm1() const noexcept
    {
      if (cr_cthis_at(0).is_not_Bm1())
        return false;
      for (size_t ix{1}; ix < L; ++ix)
      {
        if (cr_cthis(ix).is_not_0())
          return false;
      }
      return true;
    }

    inline constexpr bool is_B() const noexcept
    {
      if constexpr (L == 1)
        return false;
      else
      {
        if (cr_cthis_at(0).is_not_0())
          return false;
        if (cr_cthis_at(1).is_not_1())
          return false;
        for (size_t ix{2}; ix < L; ++ix)
        {
          if (cr_cthis_at(ix).is_not_0())
            return false;
        }
        return true;
      }
    }

    inline constexpr bool is_Bp1() const noexcept
    {
      if constexpr (L == 1)
        return false;
      else
      {
        if (cr_cthis_at(0).is_not_1())
          return false;
        if (cr_cthis_at(1).is_not_1())
          return false;
        for (size_t ix{2}; ix < L; ++ix)
        {
          if (cr_cthis_at(ix).is_not_0())
            return false;
        }
        return true;
      }
    }

    template <size_t n>
      requires(n < L)
    inline constexpr bool is_B_pow_m1() const noexcept
    {
      if constexpr (n == 0)
      {
        return cr_cthis().is_0();
      }
      else if constexpr (n == 1)
      {
        return cr_cthis().is_Bm1();
      }
      else if constexpr (n == 2)
      {
        if (cr_cthis_at(1).is_not_Bm1() || cr_cthis_at(0).is_not_Bm1())
          return false;
        for (size_t ix{2}; ix < L; ++ix)
          if (cr_cthis_at(ix).is_not_0())
            return false;
        return true;
      }
      else
      {
        for (size_t ix{0}; ix < n; ++ix)
          if (cr_cthis_at(ix).is_not_Bm1())
            return false;
        for (size_t ix{n}; ix < L; ++ix)
          if (cr_cthis_at(ix).is_not_0())
            return false;
        return true;
      }
    }

    template <size_t n>
      requires(n < L - 1)
    inline constexpr bool is_B_pow() const noexcept
    {
      if constexpr (n == 0)
      {
        if (cr_cthis_at(0).is_not_1())
          return false;
        for (size_t ix{1}; ix < L; ++ix)
          if (cr_cthis_at(ix).is_not_0())
            return false;
        return true;
      }
      else if constexpr (n == 1)
      {
        if (cr_cthis_at(0).is_not_0())
          return false;
        if (cr_cthis_at(1).is_not_1())
          return false;
        for (size_t ix{2}; ix < L; ++ix)
          if (cr_cthis_at(ix).is_not_0())
            return false;
        return true;
      }
      else if constexpr (n == 2)
      {
        if (cr_cthis_at(0).is_not_0() || cr_cthis_at(1).is_not_0())
          return false;
        if (cr_cthis_at(2).is_not_1())
          return false;
        for (size_t ix{3}; ix < L; ++ix)
          if (cr_cthis_at(ix).is_not_0())
            return false;
        return true;
      }
      else
      {
        for (size_t ix{0}; ix < n; ++ix)
          if (cr_cthis_at(ix).is_not_0())
            return false;
        if (cr_cthis_at(n).is_not_1())
          return false;
        for (size_t ix{n + 1}; ix < L; ++ix)
          if (cr_cthis_at(ix).is_not_0())
            return false;
        return true;
      }
    }

    inline constexpr bool is_any_B_pow() const noexcept
    {
      /// r_ for reference (non const)
      /// cr_ for const reference
      const auto pred_not_0 = [](dig_t d)
      { return (d.is_not_0()); };
      const auto it{
          std::find_if(cr_cthis().cbegin(), cr_cthis().cend(), pred_not_0)};
      if (it != cr_cthis().cend())
      {
        const int64_t card{
            std::count_if(cr_cthis().cbegin(), cr_cthis().cend(), pred_not_0)};
        if (card != 1)
          return false;
        else
          return (it->is_0());
      }
      return false;
    }

    inline constexpr bool is_filled_of_1() const noexcept
    {
      for (size_t ix{0}; ix < L; ++ix)
        if (cr_cthis_at(ix).is_not_1())
          return false;
      return true;
    }

    inline constexpr bool is_filled_of_Bm1() const noexcept
    {
      for (size_t ix{0}; ix < L; ++ix)
        if (cr_cthis_at(ix).is_not_Bm1())
          return false;
      return true;
    }

    inline constexpr bool is_filled_of(dig_t d) const
    {
      for (size_t ix{0}; ix < L; ++ix)
        if (cr_cthis_at(ix) != d)
          return false;
      return true;
    }

    ///		STATIC FUNCTIONS :  CONCATENATE REGISTERS AND DIGITS
    /// 		reg_digs_t<1>		cat(dig_t)
    /// 		reg_digs_t<1+1>		cat(dig_t,dig_t)
    /// 		reg_digs_t			cat(reg_digs_t)
    ///   <M> reg_N_digs_t<L+M>	cat(reg_digs_t,reg_N_digs_t<M>)
    /// 		reg_N_digs_t<L+1>	cat(reg_digs_t,dig_t)
    /// 		reg_N_digs_t<1+L>	cat(dig_t,reg_digs_t)
    /// 		reg_N_digs_t<sizeof...(dig_pack)>
    ///							cat(dig_t ... dig_pack) VARIADIC PACK

    /// STATIC REG_DIGS_T CAT(REG_DIGS_T)
    static constexpr inline reg_digs_t cat(const reg_digs_t &arg) noexcept
    {
      return std::move(reg_digs_t{arg});
    }

    /// STATIC <M> REG_N_DIGS_T<L+M> CAT(REG_DIGS_T,REG_N_DIGS_T<M>)
    template <size_t M>
    static constexpr inline reg_N_digs_t<L + M>
    cat(const reg_digs_t &larg, const reg_N_digs_t<M> &rarg) noexcept
    {
      base_N_t<L + M> ret;
      for (size_t ix{0}; ix < L; ++ix)
        ret[ix] = larg[ix];
      for (size_t ix{L}; ix < M; ++ix)
        ret[ix] = rarg[ix];
      return std::move(ret);
    }

    /// STATIC REG_DIGS_T<L+1> CAT(REG_DIGS_T,DIG_T)
    static constexpr inline reg_N_digs_t<L + 1> cat(const reg_digs_t &larg, dig_t rarg) noexcept
    {
      reg_N_digs_t<L + 1> ret;
      for (size_t ix{0}; ix < L; ++ix)
        ret[ix] = larg[ix];
      ret[L] = rarg;
      return std::move(ret);
    }

    /// REG_N_DIGS_T<1+L> CAT(DIG_T,REG_DIGS_T)
    static constexpr inline reg_N_digs_t<1 + L> cat(dig_t larg, const reg_digs_t &rarg) noexcept
    {
      reg_N_digs_t<1 + L> ret;
      ret[0] = larg;
      for (size_t ix{1}; ix < L + 1; ++ix)
        ret[ix] = rarg[ix - 1];
      return std::move(ret);
    }

    /// STATIC REG_N_DIGS_T<1> CAT(DIG_T)
    static constexpr inline reg_N_digs_t<1> cat(dig_t larg) noexcept
    {
      return std::move(reg_N_digs_t<1>{larg});
    }

    /// STATIC REG_N_DIGS_T<1+1> CAT(DIG_T,DIG_T)
    static constexpr inline reg_N_digs_t<2> cat(dig_t larg, dig_t rarg) noexcept
    {
      reg_N_digs_t<2> ret;
      ret[0] = larg;
      ret[1] = rarg;
      return std::move(ret);
    }

    /// STATIC REG_N_DIGS_T<SIZEOF...(DIG_PACK)>
    /// CAT(DIG_T,DIG_T ... DIG_PACK) VARIADIC
    template <typename T0, typename... Ts>
      requires(((std::is_same_v<Ts, dig_t>) && ...) && (std::is_same_v<T0, dig_t>))
    static constexpr inline reg_N_digs_t<1 + (sizeof...(Ts))> cat(
        T0 dig0, Ts... dig_pack) noexcept
    {
      return cat(dig0, dig_pack...);
    }

    /// STATIC REG_N_DIGS_T<L+1+(SIZEOF...(DIG_PACK))>
    /// CAT(REG_DIGS_T,DIG_T,DIG_T ... DIG_PACK) VARIADIC
    template <typename T, typename... Ts>
      requires(((std::is_same_v<Ts, dig_t>) && ...) && (std::is_same_v<T, dig_t>))
    static constexpr inline reg_N_digs_t<L + 1 + (sizeof...(Ts))> cat(
        reg_digs_t larg, T dig, Ts... dig_pack) noexcept
    {
      return cat(larg, cat(dig, dig_pack...));
    }

    /// STATIC REG_N_DIGS_T<L+1+(SIZEOF...(DIG_PACK))>
    /// CAT(DIG_T,DIG_T ... DIG_PACK,REG_DIGS_T) VARIADIC
    template <typename T, typename... Ts>
      requires(((std::is_same_v<Ts, dig_t>) && ...) && (std::is_same_v<T, dig_t>))
    static constexpr inline reg_N_digs_t<L + 1 + (sizeof...(Ts))> cat(
        T dig, Ts... dig_pack, reg_digs_t rarg) noexcept
    {
      return cat(cat(dig, dig_pack...), rarg);
    }

    /// STATIC REG_N_DIGS_T<SIZE_T N,SIZE_T ... N_PACK>
    /// CAT(
    ///           REG_N_DIGS_T<N> LARG,
    ///			REG_N_DIGS_T<N_PACK> ... RARG_PACK
    /// ) VARIADIC PACK
    template <size_t N, size_t... N_pack>
      requires((N > 0) && ((N_pack > 0) && ...))
    static constexpr inline reg_N_digs_t<N + (... + (N_pack))> cat(
        reg_N_digs_t<N> larg, reg_N_digs_t<N_pack>... rarg_pack) noexcept
    {
      return cat(larg, rarg_pack...);
    }

    /// TAKE A SUBREGISTER OF A REGISTER OF DIGITS
    template <size_t ibegin, size_t iend>
      requires((iend <= L) && (ibegin < L) && (ibegin != iend))
    constexpr inline reg_N_digs_t<(ibegin < iend) ? (iend - ibegin) : (ibegin - iend)>
    subregister() const noexcept
    {
      const reg_digs_t &cthis{*this};
      if constexpr (ibegin < iend)
      {
        reg_N_digs_t<iend - ibegin> ret;
        for (size_t ix{ibegin}; ix < iend; ++ix)
        {
          ret[ix - ibegin] = cthis[ix];
        }
        return ret;
      }
      else
      {
        reg_N_digs_t<iend - ibegin> ret;
        for (int64_t ix{iend}; ix > ibegin - 1; --ix)
        {
          ret[ix - ibegin] = cthis[L - 1 - ix];
        }
        return ret;
      }
    }

    /// OPERADORES ARITMETICOS
    ///	C_B()  C_Bm1()
    ///	mC_B() mC_Bm1()
    ///	operator~() operator-()

    constexpr inline const reg_digs_t &mC_Bm1() noexcept
    {
      for (size_t ix{0}; ix < L; ++ix)
      {
        r_cthis(ix).mC_Bm1();
      }
      return (cr_cthis());
    }

    constexpr inline const reg_digs_t &mC_B() noexcept
    {
      r_cthis().mC_Bm1();
      for (size_t ix{0}; ix < L; ++ix)
      {
        if (r_cthis(ix).is_not_Bm1())
        {
          ++r_cthis(ix);
          return (cr_cthis());
        }
        else
        {
          r_cthis(ix).set_0();
        }
      }
      return (cr_cthis());
    }

    constexpr inline reg_digs_t C_Bm1() const noexcept
    {
      return std::move(cp_cthis().mC_Bm1());
    }

    constexpr inline reg_digs_t C_B() const noexcept
    {
      return std::move(cp_cthis().mC_B());
    }

    /// A partir de aquí no he hecho las sustituciones de
    /// const reg_digs_t&  cthis{*this}; / reg_digs_t&  cthis{*this}; / reg_digs_t
    /// cthis{*this}; Por las correspondientes funciones cr_cthis() / r_cthis() /
    /// cp_cthis()

    constexpr inline reg_digs_t operator~() const noexcept { return C_Bm1(); }

    constexpr inline reg_digs_t operator-() const noexcept { return C_B(); }

  public:
    /// OPERATORS >> >>= << <<= REM_B^n M_REM_B^n MER_B_N M_MER_B_N

    /// MULTIPLY BY THE BASE B (10) << <<=
    /// DIVIDE BY THE BASE B (10)	  >> >>=
    /// REMAINDER BY THE BASE B (10) REM_B_N M_REM_B_N
    /// MULTIPLICATIVE CARRY BY THE BASE B (10) MER_B_N M_MER_B_N
    constexpr inline reg_digs_t operator<<(size_t n) const noexcept
    {
      for (int32_t ix{L - 1 - n}; ix > -1; --ix)
      {
        cp_cthis(ix + n) = cr_cthis(ix);
      }
      for (int32_t ix{0}; ix < n; ++ix)
      {
        cp_cthis(ix + n) = dig_0();
      }
      return (cp_cthis());
    }

    constexpr inline const reg_digs_t &operator<<=(size_t n) noexcept
    {
      reg_digs_t &cthis{*this};
      for (int64_t ix{int64_t(L) - 1 - int64_t(n)}; ix > -1; --ix)
      {
        cthis[ix + n] = cthis[ix];
      }
      for (int64_t ix{0}; ix < int64_t(n); ++ix)
      {
        cthis[ix] = dig_0();
      }
      return (*this);
    }

    constexpr inline const reg_digs_t &operator>>=(size_t n) noexcept
    {
      reg_digs_t &cthis{*this};
      constexpr int64_t Ls = static_cast<int64_t>(L);
      const int64_t ns = static_cast<int64_t>(n);
      for (int64_t ix{0}; ix < Ls - ns; ++ix)
      {
        cthis[ix] = cthis[ix + n];
      }
      for (int64_t ix{Ls - ns}; ix < Ls; ++ix)
      {
        cthis[ix] = dig_0();
      }
      return (*this);
    }

    constexpr inline reg_digs_t operator>>(size_t n) const noexcept
    {
      reg_digs_t ret{*this};
      ret >>= n;
      return ret;
    }

    constexpr inline reg_digs_t rem_B(size_t n) const noexcept
    {
      reg_digs_t ret{*this};
      ret <<= L - n;
      return ret;
    }

    constexpr inline const reg_digs_t &m_rem_B(size_t n) noexcept
    {
      reg_digs_t &cthis{*this};
      cthis <<= L - n;
      return (*this);
    }

    constexpr inline reg_digs_t mer_B(size_t n) const noexcept
    {
      reg_digs_t ret{*this};
      ret >>= L - n;
      return (ret);
    }

    constexpr inline const reg_digs_t &m_mer_B(size_t n) noexcept
    {
      reg_digs_t &cthis{*this};
      cthis >>= L - n;
      return (cthis);
    }

    /// OPERATOR BIT A BIT BITWISE | |= & &=

    constexpr inline const reg_digs_t &operator|=(const base_t &rarg) const noexcept
    {
      reg_digs_t &cthis{*this};
      for (size_t ix{0}; ix < L; ++ix)
      {
        cthis[ix] |= rarg[ix];
      }
      return (cthis);
    }

    constexpr inline reg_digs_t operator|(const reg_digs_t &rarg) const noexcept
    {
      reg_digs_t ret{*this};
      ret |= rarg;
      return (ret);
    }

    constexpr inline const reg_digs_t &
    operator&=(const reg_digs_t &rarg) noexcept
    {
      reg_digs_t &cthis{*this};
      for (size_t ix{0}; ix < L; ++ix)
      {
        cthis[ix] &= rarg[ix];
      }
      return (cthis);
    }

    constexpr inline reg_digs_t operator&(const reg_digs_t &rarg) noexcept
    {
      reg_digs_t ret{*this};
      ret &= rarg;
      return (ret);
    }

    //  PRIVATE
    /// NOS DEVUELVE EL ÍNDICE DEL DÍGITO NO 0 DE POTENCIA DE B MAS GRANDE
    /// NOS DEVUELVE 0 SI ES UN DÍGITO NO 0
    /// NOS DEVUELVE -1 SI ES EL DÍGITO 0
    /// CUALQUIER OTRO CASO NOS DARÁ MAYOR QUE 0
    inline constexpr int64_t index_of_MSDig() const noexcept
    {
      for (int64_t ix{L - 1}; ix > -1; --ix)
      {
        if (cr_cthis_at(ix).is_not_0())
          return ix;
      }
      return (-1);
    }

    ///
    ///
    /// SOBRECARGA DE LOS OPERADORES DE COMPARACION Y ORDEN
    ///
    ///

    // ========================================
    // OPERADORES DE COMPARACIÓN CON dig_t
    // ========================================

    /**
     * @brief Compara el registro con un dígito individual
     * @param rarg Dígito a comparar
     * @return true si el registro representa exactamente el valor del dígito
     * @note Solo es true si: cthis[0] == rarg && todos los demás dígitos son 0
     * @note Ejemplo: reg{5,0,0} == dig_t(5) es true, pero reg{5,1,0} == dig_t(5) es false
     */
    constexpr inline bool operator==(const dig_t &rarg) const noexcept
    {
      const reg_digs_t &cthis{*this};
      if (cthis[0] != rarg)
        return false;
      for (size_t ix{1}; ix < L; ++ix)
        if (cthis[ix].is_not_0())
          return false;
      return true;
    }

    /**
     * @brief Verifica desigualdad con un dígito individual
     * @param rarg Dígito a comparar
     * @return true si el registro NO representa exactamente el valor del dígito
     */
    constexpr inline bool operator!=(const dig_t &rarg) const noexcept
    {
      const reg_digs_t &cthis{*this};
      for (size_t ix{1}; ix < L; ++ix)
        if (cthis[ix].is_not_0())
          return true;
      if (cthis[0] != rarg)
        return true;
      return false;
    }

    constexpr inline bool operator>(const dig_t &rarg) const noexcept
    {
      const reg_digs_t &cthis{*this};
      for (size_t ix{1}; ix < L; ++ix)
        if (cthis[ix].is_not_0())
          return true;
      if (cthis[0] > rarg)
        return true;
      return false;
    }

    constexpr inline bool operator<(const dig_t &rarg) const noexcept
    {
      const reg_digs_t &cthis{*this};
      for (size_t ix{1}; ix < L; ++ix)
        if (cthis[ix].is_not_0())
          return false;
      if (cthis[0] >= rarg)
        return false;
      return true;
    }

    constexpr inline bool operator>=(const dig_t &rarg) const noexcept
    {
      const reg_digs_t &cthis{*this};
      for (size_t ix{1}; ix < L; ++ix)
        if (cthis[ix].is_not_0())
          return true;
      if (cthis[0] >= rarg)
        return true;
      return false;
    }

    constexpr inline bool operator<=(const dig_t &rarg) const noexcept
    {
      const reg_digs_t &cthis{*this};
      for (size_t ix{1}; ix < L; ++ix)
        if (cthis[ix].is_not_0())
          return false;
      if (cthis[0] > rarg)
        return false;
      else
        return true;
    }

    /// COMPARACIONES ENTRE REG_DIGS_T Y REG_N_DIGS_T
    ///	HETEROGENEOS EN GENERAL
    ///	EN FORMA REG_DIGS_T @ REG_N_DIGS_T<M>
    template <size_t M>
    constexpr inline bool operator==(const reg_N_digs_t<M> &rarg) const noexcept
    {
      const reg_digs_t &cthis{*this};

      constexpr size_t P{std::min(L, M)};
      constexpr size_t Q{std::max(L, M)};
      constexpr bool L_gt_M{L > M};

      if constexpr (L != M)
      {
        for (size_t ix{P}; ix < Q; ++ix)
        {
          if constexpr (L_gt_M)
          {
            if (cthis[ix].is_not_0())
              return false;
          }
          else
          {
            if (rarg[ix].is_not_0())
              return false;
          }
        }
      }
      for (size_t ix{0}; ix < P; ++ix)
        if (cthis[ix] != rarg[ix])
          return false;
      return true;
    }

    template <size_t M>
    constexpr inline bool operator!=(const reg_N_digs_t<M> &rarg) const noexcept
    {
      const reg_digs_t &cthis{*this};

      constexpr size_t P{std::min(L, M)};
      constexpr size_t Q{std::max(L, M)};
      constexpr bool L_gt_M{L > M};

      if constexpr (L != M)
      {
        for (size_t ix{P}; ix < Q; ++ix)
        {
          if constexpr (L_gt_M)
          {
            if (cthis[ix].is_not_0())
              return true;
          }
          else
          {
            if (rarg[ix].is_not_0())
              return true;
          }
        }
      }
      for (size_t ix{0}; ix < P; ++ix)
        if (cthis[ix] != rarg[ix])
          return true;
      return false;
    }

    template <size_t M>
    constexpr inline bool operator>(const reg_N_digs_t<M> &rarg) const noexcept
    {
      const reg_digs_t &cthis{*this};
      constexpr size_t P{std::min(L, M)};
      constexpr size_t Q{std::max(L, M)};

      if constexpr (L != M)
      {
        for (size_t ix{P}; ix < Q; ++ix)
        {
          if constexpr (L > M)
          {
            if (cthis[ix].is_not_0())
              return true;
          }
          else
          {
            if (rarg[ix].is_not_0())
              return false;
          }
        }
      }

      for (int32_t ix{P - 1}; ix > -1; --ix)
        if (cthis[ix] > rarg[ix])
          return true;
        else if (cthis[ix] < rarg[ix])
          return false;
      return false;
    }

    template <size_t M>
    constexpr inline bool operator<(const reg_N_digs_t<M> &rarg) const noexcept
    {
      const reg_digs_t &cthis{*this};
      constexpr size_t P{std::min(L, M)};
      constexpr size_t Q{std::max(L, M)};
      constexpr bool L_gt_M{L > M};

      if constexpr (L != M)
      {
        for (size_t ix{P}; ix < Q; ++ix)
        {
          if constexpr (L_gt_M)
          {
            if (cthis[ix].is_not_0())
              return false; // NO ES MENOR
          }
          else
          {
            if (rarg[ix].is_not_0())
              return true; // ES MENOR
          }
        }
      }

      for (int32_t ix{P - 1}; ix > -1; --ix)
        if (cthis[ix] < rarg[ix])
          return true;
        else if (cthis[ix] > rarg[ix])
          return false;
      return false;
    }

    template <size_t M>
    constexpr inline bool operator>=(const reg_N_digs_t<M> &rarg) const noexcept
    {
      const reg_digs_t &cthis{*this};
      return (!(cthis < rarg));
    }

    template <size_t M>
    constexpr inline bool operator<=(const reg_N_digs_t<M> &rarg) const noexcept
    {
      const reg_digs_t &cthis{*this};
      return (!(cthis > rarg));
    }

    /// OPERADOR COMPARACION SPACESHIP C++20

    template <size_t M>
    constexpr inline std::strong_ordering
    operator<=>(const reg_N_digs_t<M> &arg) const noexcept
    {
      const reg_digs_t &cthis{*this};
      if (cthis == arg)
        return std::strong_ordering::equal;
      else if (cthis < arg)
        return std::strong_ordering::less;
      else
        return std::strong_ordering::greater;
    }

    /// OPERADOR COMPARACION SPACESHIP C++20
    constexpr inline std::weak_ordering
    operator<=>(const dig_t &arg) const noexcept
    {
      const reg_digs_t &cthis{*this};
      if (cthis > arg)
        return std::weak_ordering::greater;
      else if (cthis < arg)
        return std::weak_ordering::less;
      else
        return std::weak_ordering::equivalent;
    }

    std::string to_string() const noexcept
    {
      std::string reg_escrito{};
      std::stringstream os{};
      os << "reg_digs_t#";
      for (std::int64_t ix{static_cast<std::int64_t>(this->size()) - 1}; ix > -1; --ix)
      {
        os << (*this)(ix);
        if (ix != 0)
          os << ':';
      }
      os << "#B" << B;
      reg_escrito = os.str();
      return std::move(reg_escrito);
    }
  };

  using namespace type_traits;
  /// STATIC BASE_N_T<N>
  /// CONCAT(BASE_N_T<N>) STATIC BASE_N_T<N+M>
  /// CONCAT(BASE_N_T<N>,BASE_N_T<M>) STATIC BASE_N_T<N+1>
  /// CONCAT(BASE_N_T<N>,DIG_T) STATIC BASE_N_T<1+M>
  /// CONCAT(DIG_T,BASE_N_T<M>) STATIC BASE_N_T<1>
  /// CONCAT(DIG_T) STATIC BASE_N_T<1+1>
  /// CONCAT(DIG_T,DIG_T) STATIC BASE_N_T<SIZEOF...(DIG_PACK)>
  /// CONCAT(DIG_T...DIG_PACK) VARIADIC PACK

  /// STATIC BASE_N_T<N> CONCAT(BASE_N_T<N>)
  template <uint64_t B, size_t N>
    requires(N > 0)
  constexpr inline reg_digs_t<B, N> concat(
      const reg_digs_t<B, N> &larg) noexcept
  {
    return reg_digs_t<B, N>{larg};
  }

  /// STATIC BASE_N_T<N+M> CONCAT(BASE_N_T<N>,BASE_N_T<M>)
  template <uint64_t B, size_t N, size_t M>
    requires((N > 0) && (M > 0))
  constexpr inline reg_digs_t<B, N + M> concat(
      const reg_digs_t<B, N> &larg,
      const reg_digs_t<B, M> &rarg) noexcept
  {
    reg_digs_t<B, N + M> ret;
    for (size_t ix{0}; ix < N; ++ix)
      ret[ix] = larg[ix];
    for (size_t ix{N}; ix < M; ++ix)
      ret[ix] = rarg[ix];
    return ret;
  }

  /// STATIC BASE_N_T<N+1> CONCAT(BASE_N_T<N>,DIG_T)
  template <uint64_t B, size_t N>
    requires(N > 0)
  constexpr inline reg_digs_t<B, N + 1> concat(
      const reg_digs_t<B, N> &larg, dig_t<B> rarg) noexcept
  {
    reg_digs_t<B, N + 1> ret;
    for (size_t ix{0}; ix < N; ++ix)
      ret[ix] = larg[ix];
    ret[N] = rarg;
    return ret;
  }

  /// STATIC BASE_N_T<1+M> CONCAT(DIG_T,BASE_N_T<M>)
  template <uint64_t B, size_t M>
    requires(M > 0)
  constexpr inline reg_digs_t<B, 1 + M> concat(
      dig_t<B> larg,
      const reg_digs_t<B, M> &rarg) noexcept
  {
    reg_digs_t<B, 1 + M> ret;
    ret[0] = larg;
    for (size_t ix{1}; ix < M + 1; ++ix)
      ret[ix] = rarg[ix - 1];
    return ret;
  }

  /// STATIC REG_DIGS_N_T<1> CONCAT(DIG_T)
  template <uint64_t B>
  constexpr inline reg_digs_t<B, 1>
  concat(dig_t<B> larg) noexcept
  {
    return reg_digs_t<B, 1>{larg};
  }

  /// STATIC REG_DIGS_N_T<1+1> CONCAT(DIG_T,DIG_T)
  template <uint64_t B>
  constexpr inline reg_digs_t<B, 2>
  concat(dig_t<B> larg, dig_t<B> rarg) noexcept
  {
    reg_digs_t<B, 2> ret;
    ret[0] = larg;
    ret[1] = rarg;
    return ret;
  }

  /// STATIC BASE_N_T<SIZEOF...(DIG_PACK)>
  ///		CONCAT(DIG_T,DIG_T ... DIG_PACK)
  ///	VARIADIC PACK
  template <uint64_t B, typename T0, typename... Ts>
    requires(((std::is_same_v<Ts, dig_t<B>>) && ...) &&
             (std::is_same_v<T0, dig_t<B>>))
  constexpr inline reg_digs_t<B, 1 + (sizeof...(Ts))> concat(T0 dig0, Ts... dig_pack) noexcept
  {
    return concat(dig0, concat(dig_pack...));
  }

  /// STATIC BASE_N_T<N+1+(SIZEOF...(DIG_PACK))>
  ///		CONCAT(BASE_N_T<N>,DIG_T,DIG_T ... DIG_PACK)
  ///	VARIADIC PACK
  template <uint64_t B, size_t N, typename T, typename... Ts>
    requires(((std::is_same_v<Ts, dig_t<B>>) && ...) &&
             (std::is_same_v<T, dig_t<B>>) && (N > 0))
  constexpr inline reg_digs_t<B, N + 1 + (sizeof...(Ts))> concat(
      reg_digs_t<B, N> larg,
      T dig,
      Ts... dig_pack) noexcept
  {
    return concat(larg, concat(dig, dig_pack...));
  }

  /// STATIC BASE_N_T<M+1+(SIZEOF...(DIG_PACK))>
  ///			CONCAT(DIG_T,DIG_T ... DIG_PACK,BASE_N_T<M>)
  ///	VARIADIC PACK
  template <uint64_t B, size_t M, typename T, typename... Ts>
    requires(((std::is_same_v<Ts, dig_t<B>>) && ...) &&
             (std::is_same_v<T, dig_t<B>>) && (M > 0))
  constexpr inline reg_digs_t<B, M + 1 + (sizeof...(Ts))> concat(
      T dig,
      Ts... dig_pack,
      reg_digs_t<B, M> rarg) noexcept
  {
    return concat(concat(dig, dig_pack...), rarg);
  }

  /// STATIC BASE_N_T<SIZE_T N,SIZE_T ... N_PACK>
  ///	CONCAT(BASE_N_T<N> LARG,BASE_N_T<N_PACK> ... RARG_PACK)
  ///	VARIADIC PACK
  template <uint64_t B, size_t N, size_t... N_pack>
    requires((N > 0) && ((N_pack > 0) && ...))
  constexpr inline reg_digs_t<B, N + (... + (N_pack))> concat(
      reg_digs_t<B, N> larg,
      reg_digs_t<B, N_pack>... rarg_pack) noexcept
  {
    return concat(larg, concat(rarg_pack...));
  }

  template <uint64_t B, size_t N>
    requires(N > 0 && B > 1)
  constexpr inline reg_digs_t<B, N> operator<<(const reg_digs_t<B, N> &larg, size_t n) noexcept
  {
    reg_digs_t<B, N> cparg{larg};
    for (std::int32_t ix{N - 1 - n}; ix > -1; --ix)
    {
      cparg[ix + n] = larg[ix];
    }
    for (std::int32_t ix{0}; ix < n; ++ix)
    {
      cparg[ix] = dig_t<B>::dig_0();
    }
    return cparg;
  }

  template <uint64_t B, size_t N>
    requires(B > 1 && N > 0)
  constexpr inline const reg_digs_t<B, N> &operator<<=(reg_digs_t<B, N> &larg, size_t n) noexcept
  {
    for (std::int32_t ix{N - 1 - n}; ix > -1; --ix)
    {
      larg[ix + n] = larg[ix];
    }
    for (std::int32_t ix{0}; ix < n; ++ix)
    {
      larg[ix] = dig_t<B>::dig_0();
    }
    return (larg);
  }

  template <uint64_t B, size_t N>
    requires((B > 1) && (N > 0))
  constexpr inline const reg_digs_t<B, N> &operator>>=(reg_digs_t<B, N> &larg, size_t n) noexcept
  {
    for (std::int32_t ix{0}; ix < N - n; ++ix)
    {
      larg[ix] = larg[ix + n];
    }
    for (std::int32_t ix{N - n}; ix < N; ++ix)
    {
      larg[ix] = dig_t<B>::dig_0();
    }
    return (larg);
  }

  template <uint64_t B, size_t N>
    requires((B > 1) && (N > 0))
  constexpr inline reg_digs_t<B, N> operator>>(const reg_digs_t<B, N> &larg, size_t n) noexcept
  {
    return (reg_digs_t<B, N>{larg} >>= n);
  }

  template <uint64_t B, size_t N>
    requires((B > 1) && (N > 0))
  constexpr inline reg_digs_t<B, N> rem_B(
      const reg_digs_t<B, N> &larg, size_t n) noexcept
  {
    reg_digs_t<B, N> ret{larg};
    ret <<= N - n;
    return ret;
  }

  template <uint64_t B, size_t N>
    requires((B > 1) && (N > 0))
  constexpr inline const reg_digs_t<B, N> &m_rem_B(
      reg_digs_t<B, N> &larg,
      size_t n) noexcept
  {
    larg <<= N - n;
    return (larg);
  }

  template <uint64_t B, size_t N>
    requires((B > 1) && (N > 0))
  constexpr inline reg_digs_t<B, N> mer_B(const reg_digs_t<B, N> &larg, size_t n) noexcept
  {
    reg_digs_t<B, N> ret{larg};
    ret >>= N - n;
    return ret;
  }

  template <uint64_t B, size_t N>
    requires((B > 1) && (N > 0))
  constexpr inline const reg_digs_t<B, N> &m_mer_B(reg_digs_t<B, N> &larg, size_t n) noexcept
  {
    larg >>= N - n;
    return (larg);
  }

  template <uint64_t B, size_t N>
    requires((B > 1) && (N > 0))
  constexpr inline const reg_digs_t<B, N> &operator|=(
      reg_digs_t<B, N> &larg,
      const reg_digs_t<B, N> &rarg) noexcept
  {
    for (size_t ix{0}; ix < N; ++ix)
    {
      larg[ix] |= rarg[ix];
    }
    return (larg);
  }

  template <uint64_t B, size_t N>
    requires((B > 1) && (N > 0))
  constexpr inline reg_digs_t<B, N> operator|(
      const reg_digs_t<B, N> &larg,
      const reg_digs_t<B, N> &rarg) noexcept
  {
    reg_digs_t ret{larg};
    ret |= rarg;
    return (ret);
  }

  template <uint64_t B, size_t N>
    requires((B > 1) && (N > 0))
  constexpr inline const reg_digs_t<B, N> &operator&=(
      reg_digs_t<B, N> &larg,
      const reg_digs_t<B, N> &rarg) noexcept
  {
    for (size_t ix{0}; ix < N; ++ix)
    {
      larg[ix] &= rarg[ix];
    }
    return (larg);
  }

  template <uint64_t B, size_t N>
    requires((B > 1) && (N > 0))
  constexpr inline reg_digs_t<B, N> operator&(
      reg_digs_t<B, N> &larg,
      const reg_digs_t<B, N> &rarg) noexcept
  {
    reg_digs_t<B, N> ret{larg};
    ret &= rarg;
    return (ret);
  }

  /// COMENTAR m_incr
  template <uint64_t B, size_t N>
  constexpr inline reg_digs_t<B, N> &m_incr(reg_digs_t<B, N> &rarg) noexcept
  {
    using dig_t = dig_t<B>;

    dig_t carry{dig_t::dig_1()};
    for (size_t i = 0; i < N; ++i)
    {
      if (carry.is_not_0())
      {
        if (rarg[i].is_Bm1())
        {
          rarg[i].set_0();
          carry.set_1();
        }
        else
        {
          ++rarg[i];
          carry.set_0();
          break;
        }
      }
      else
      {
        carry.set_0();
        break;
      }
    }
    return (rarg);
  }

  /// COMENTAR incr
  template <uint64_t B, size_t N>
  constexpr inline std::tuple<reg_digs_t<B, N>, dig_t<B>> incr(
      const reg_digs_t<B, N> &rarg) noexcept
  {
    using dig_t = dig_t<B>;
    using reg_digs_t = reg_digs_t<B, N>;
    using ret_type = std::tuple<reg_digs_t, dig_t>;
    reg_digs_t cp_rarg{rarg};
    dig_t carry{m_incr(cp_rarg)};
    return std::move(ret_type{cp_rarg, carry});
  }

  /// comentar m_decr
  template <uint64_t B, size_t N>
  constexpr inline reg_digs_t<B, N> &m_decr(reg_digs_t<B, N> &rarg) noexcept
  {
    using dig_t = dig_t<B>;

    dig_t borrow{dig_t::dig_1()};
    for (size_t i = 0; i < N; ++i)
    {
      if (borrow.is_not_0())
      {
        if (rarg[i].is_0())
        {
          rarg[i].set_Bm1();
          borrow.set_1();
        }
        else
        {
          --rarg[i];
          borrow.set_0();
          break;
        }
      }
      else
      {
        borrow.set_0();
        break;
      }
    }
    return (rarg);
  }

  /// comentar decr
  template <uint64_t B, size_t N>
  constexpr inline std::tuple<reg_digs_t<B, N>, dig_t<B>> decr(
      const reg_digs_t<B, N> &rarg) noexcept
  {
    using dig_t = dig_t<B>;
    using reg_digs_t = reg_digs_t<B, N>;
    using ret_type = std::tuple<reg_digs_t, dig_t>;
    reg_digs_t cp_rarg{rarg};
    dig_t borrow{m_decr(cp_rarg)};
    return std::move(ret_type{cp_rarg, borrow});
  }

  /// comentar m_sum_digs_carryin_0
  template <uint64_t B>
  constexpr inline dig_t<B> m_sum_digs_carryin_0(dig_t<B> &left, const dig_t<B> &right) noexcept
  {
    /// SUMA 2 DIGITOS
    /// CARRY C==0
    /// L + D <  B-1 					=>  L.C_Bm1 > D
    /// :
    ///														C:=0
    /// S:=L+D
    /// L == B-1	&& D == B-1	=>	L+D == 2B-2  == (B-1)-1
    /// :
    ///														C:=1
    /// S:=(B-1)-1
    /// L == B-1	&& D != B-1	=>	L+D == B-1+D == B + (D-1)
    /// :
    ///														C:=1
    /// S:=D-1
    /// D == B-1	&& L != B-1	=>	L+D == B-1+L == B + (L-1)
    /// :
    ///														C:=1
    /// S:=L-1
    /// L + D >= B-1 					=>	(L.C_Bm1+1-D).C_B =mod_B= L+D
    /// :
    ///														C:=1
    /// S:=(L.C_Bm1-D).C_Bm1

    using dig_t = dig_t<B>;
    constexpr dig_t d_0 = dig_t::dig_0();
    constexpr dig_t d_1 = dig_t::dig_1();
    constexpr dig_t d_Bm1 = dig_t::dig_Bm1();

    const dig_t left_CBm1{left.C_Bm1()};
    const bool left_is_Bm1{left.is_Bm1()};
    const bool right_is_Bm1{right.is_Bm1()};
    if (left_CBm1 >= right)
    {
      left += right;
      return d_0;
    }
    else
    {
      if (left_is_Bm1 || right_is_Bm1)
      {
        if (left_is_Bm1 && right_is_Bm1)
        {
          if constexpr (B == 2)
          {
            left = d_0;
            return d_1;
          }
          else
          {
            left = d_Bm1;
            --left;
            return d_1;
          }
        }
        else if (left_is_Bm1)
        {
          if constexpr (B == 2)
          {
            return d_1;
          }
          else
          {
            left = right;
            --left;
            return d_1;
          }
        }
        else
        {
          if constexpr (B == 2)
          {
            left = d_Bm1();
            return d_1;
          }
          else
          {
            --left;
            return d_1;
          }
        }
      }
      else
      {
        left = right - left_CBm1;
        --left;
        return d_1;
      }
    }
  }

  /// comentar m_sum_digs_carryin_1
  template <uint64_t B>
  constexpr inline dig_t<B> m_sum_digs_carryin_1(dig_t<B> &left, const dig_t<B> &right) noexcept
  {
    /// SUMA 2 DIGITOS
    /// CARRY C==1
    /// L + D + 1 <  B-1 			=>  L.C_Bm1 > D	+ 1
    /// :
    ///														C:=0
    /// S:=L+D+1
    /// L == B-1	&& D == B-1	=>	L+D+1 == 2B-1  == B+(B-1)
    /// :
    ///														C:=1
    /// S:=B-1
    /// L == B-1	&& D != B-1	=>	L+D+1 == B-1+D+1 == B + D
    /// :
    ///														C:=1
    /// S:=D
    /// D == B-1	&& L != B-1	=>	L+D+1 == B-1+L+1 == B + L
    /// :
    ///														C:=1
    /// S:=L
    /// L + D + 1 >= B-1 			=>	((L.C_B-D)+1).C_B =mod_B= L+D
    /// :
    ///														C:=1
    /// S:=(L.C_Bm1+1-D).C_Bm1+1

    using dig_t = dig_t<B>;
    constexpr dig_t d_0 = dig_t::dig_0();
    constexpr dig_t d_1 = dig_t::dig_1();
    constexpr dig_t d_Bm1 = dig_t::dig_Bm1();

    const dig_t left_CBm1{left.C_Bm1()};
    const bool left_is_Bm1{left.is_Bm1()};
    const bool right_is_Bm1{right.is_Bm1()};
    if (left_is_Bm1 || right_is_Bm1)
    {
      if (left_is_Bm1 && right_is_Bm1)
      {
        left = d_Bm1();
      }
      else if (left_is_Bm1)
      {
        left = right;
      }
      return d_1;
    }
    else if (left_CBm1 < right + d_1)
    {
      left = right - left_CBm1;
      return d_1;
    }
    else
    {
      left += right;
      ++left;
      return d_0;
    }
  }

  /// comentar m_incr_by_digit
  template <uint64_t B, size_t N>
  constexpr inline dig_t<B> m_incr_by_digit(reg_digs_t<B, N> &larg, dig_t<B> dig) noexcept
  {
    using dig_t = dig_t<B>;

    dig_t carry{0};
    for (size_t i = 0; i < N; ++i)
    {
      if (carry.is_0())
      {
        carry = m_sum_digs_carryin_0(larg[i], dig);
        if (carry.is_0())
          break;
      }
      else
      {
        carry = m_sum_digs_carryin_1(larg[i], dig);
      }
    }
    return (carry);
  }

  /// comentar m_subtract_digs_borrowin_0
  template <uint64_t B>
  constexpr inline dig_t<B> m_subtract_digs_borrowin_0(dig_t<B> &left, dig_t<B> right) noexcept
  {
    /// RESTA 2 DIGITOS
    /// BORROW b==0
    /// L  > D				:
    ///		b:=0 S:=L-D
    /// L <= D				:
    ///		b:=1 S:=L+D.C_Bm1+1

    using dig_t = dig_t<B>;
    constexpr dig_t d_0 = dig_t::dig_0();
    constexpr dig_t d_1 = dig_t::dig_1();

    const dig_t right_CBm1{right.C_Bm1()};
    /// L  > D				:
    /// 	b:=0 S:=L-D
    if (left >= right)
    {
      left = dig_t(left() - right());
      return d_0;
    }
    /// L <= D				:
    ///		b:=1 S:=L+D.C_Bm1+1
    else
    {
      left = dig_t(left() + right_CBm1() + 1);
      return d_1;
    }
  }

  /// comentar m_subtract_digs_borrowin_1
  template <uint64_t B>
  constexpr inline dig_t<B> m_subtract_digs_borrowin_1(dig_t<B> &left, dig_t<B> right) noexcept
  {
    /// RESTA 2 DIGITOS
    /// BORROW b==1
    /// L == D									=>	L == D
    /// :
    ///                             b:=1 S:=B-1
    /// L != D
    /// D != B-1
    /// L >  D + 1 							=>  L.C_Bm1 + 1 <  D.C_Bm1
    /// :
    ///															b:=0
    /// S:=L-D-1
    /// L == D + 1 							=>  L.C_Bm1 + 1 == D.C_Bm1
    /// :
    ///															b:=1
    /// S:=0
    /// L < D + 1 							=>  L-D-1 == L+D.C_Bm1
    /// :
    ///															b:=1
    /// S:=L+D.C_Bm1
    /// D == B-1 :
    ///																b:=1
    /// S:=L

    using dig_t = dig_t<B>;
    constexpr dig_t d_0 = dig_t::dig_0();
    constexpr dig_t d_1 = dig_t::dig_1();
    constexpr dig_t d_Bm1 = dig_t::dig_Bm1();

    const dig_t left_CBm1{left.C_Bm1()};
    const dig_t right_CBm1{right.C_Bm1()};

    /// L != D
    if (left != right)
    {
      /// D != B-1
      if (right.is_not_Bm1())
      {
        /// L >  D + 1 		:
        ///		b:=0 S:=L-D-1
        if (left() > right() + 1)
        {
          left = left() - right() - 1;
          return d_0;
        }
        /// L == D + 1 		:
        ///		b:=0 S:=0
        else if (left() == right() + 1)
        {
          left = d_0;
          return d_0;
        }
        /// L < D + 1 		:
        ///		b:=1 S:=L+D.C_Bm1
        else
        {
          left += right_CBm1;
          return d_1;
        }
      }
      /// D == B-1 :
      ///		b:=1 S:=L
      else
      {
        return d_1;
      }
    }
    /// L == D	:
    /// 	b:=1 S:=B-1
    else
    {
      left = d_Bm1;
      return d_1;
    }
  }

  /// comentar m_decr_by_digit
  template <uint64_t B, size_t L>
  constexpr inline dig_t<B> m_decr_by_digit(reg_digs_t<B, L> &left, dig_t<B> dig) noexcept
  {
    using dig_t = dig_t<B>;
    constexpr dig_t d_0 = dig_t::dig_0();
    constexpr dig_t d_1 = dig_t::dig_1();

    dig_t borrow{d_0};
    for (size_t ix{0}; ix < L; ++ix)
    {
      if (borrow.is_0())
      {
        borrow = m_subtract_digs_borrowin_0(left[ix], dig);
      }
      else
      {
        borrow = m_subtract_digs_borrowin_1(left[ix], dig);
      }
      if (borrow.is_0())
        return d_0;
    }
    return borrow;
  }

  /// comentar m_sum
  template <uint64_t B, size_t N>
  constexpr inline dig_t<B> m_sum(reg_digs_t<B, N> &larg, const reg_digs_t<B, N> &rarg) noexcept
  {
    using dig_t = dig_t<B>;
    constexpr dig_t d_0 = dig_t::dig_0();

    dig_t carry{d_0};
    for (size_t i = 0; i < N; ++i)
    {
      const dig_t left_CBm1{larg[i].C_Bm1()};
      if (carry.is_0())
      {
        carry = m_sum_digs_carryin_0(larg[i], rarg[i]);
      }
      else
      {
        carry = m_sum_digs_carryin_1(larg[i], rarg[i]);
      }
    }
    return carry;
  }

  template <uint64_t B, size_t N>
  constexpr inline std::tuple<reg_digs_t<B, N>, dig_t<B>>
  sum(const reg_digs_t<B, N> &larg, const reg_digs_t<B, N> &rarg) noexcept
  {
    using dig_t = dig_t<B>;
    using reg_digs_t = reg_digs_t<B, N>;
    constexpr dig_t d_0 = dig_t::dig_0();

    reg_digs_t ret{larg};
    dig_t carry{d_0};
    for (size_t i = 0; i < N; ++i)
    {
      const dig_t left_CBm1{ret[i].C_Bm1()};
      if (carry.is_0())
      {
        carry = m_sum_digs_carryin_0(ret[i], rarg[i]);
      }
      else
      {
        carry = m_sum_digs_carryin_1(ret[i], rarg[i]);
      }
    }
    return std::tuple{ret, carry};
  }

  /// comentar m_subtract
  template <uint64_t B, size_t N>
  constexpr inline dig_t<B> m_subtract(
      reg_digs_t<B, N> &larg,
      const reg_digs_t<B, N> &rarg) noexcept
  {
    using dig_t = dig_t<B>;
    constexpr dig_t d_0 = dig_t::dig_0();

    dig_t borrow{d_0};
    for (size_t i = 0; i < N; ++i)
    {
      if (borrow.is_0())
      {
        borrow = m_subtract_digs_borrowin_0(larg[i], rarg[i]);
      }
      else
      {
        borrow = m_subtract_digs_borrowin_1(larg[i], rarg[i]);
      }
    }
    return borrow;
  }

  /// comentar subtract
  template <uint64_t B, size_t N>
  constexpr inline std::tuple<reg_digs_t<B, N>, dig_t<B>>
  subtract(const reg_digs_t<B, N> &larg, const reg_digs_t<B, N> &rarg) noexcept
  {
    using dig_t = dig_t<B>;
    using reg_digs_t = reg_digs_t<B, N>;
    constexpr dig_t d_0 = dig_t::dig_0();

    dig_t borrow{d_0};
    reg_digs_t ret{larg};
    for (size_t i = 0; i < N; ++i)
    {
      if (borrow.is_0())
      {
        borrow = m_subtract_digs_borrowin_0(ret[i], rarg[i]);
      }
      else
      {
        borrow = m_subtract_digs_borrowin_1(ret[i], rarg[i]);
      }
    }
    return std::tuple{ret, borrow};
  }

  /// FUNCIONES DE IMPLEMENTACION DE LA DIVISION ENTRE DOS REGISTROS DE DIGITOS
  /// BEGIN

  // comentar aprox_units_divB
  template <uint64_t B, size_t N>
    requires((N > 1) && (B > 1))
  constexpr inline reg_digs_t<B, N> aprox_units_divB(const reg_digs_t<B, N> &arg) noexcept
  ///< PRECONDITIONS : ARG != 0 AND ARG != 1 >
  {
    /// CASO EN QUE LA LONGITUD PERMANECE
    ///  999 		; 998 	; 997 	; 996
    ///  099,9 	; 099,8 ; 099,7 ; 099,6
    ///  100		; 100		;	100		;	100
    using dig_t = dig_t<B>;
    reg_digs_t<B, N> cparg{arg};
    if ((cparg >> 1).is_0())
    {
      return cparg;
    }
    else if ((cparg[0] <= dig_t{B / 2}) || (cparg.is_any_B_pow()))
    {
      cparg >>= 1;
      return cparg;
    }
    else if (cparg[N - 1].is_not_Bm1())
    {
      cparg >>= 1;
      m_incr(cparg);
      return cparg;
    }
    else
    {
      auto temp = arg.template subregister<1, N>();
      if (temp.is_filled_of_Bm1())
      {
        /// CPARG == Bm1 Bm1 Bm1 ... Bm1 Bm1 Bm1 d0<B/2
        cparg.set_0();
        cparg[N - 1].set_1();
        return cparg;
      }
      else
      {
        /// CPARG == Bm1 dnm2 dnm3 ... d3 d2 d1 d0
        cparg >>= 1;
        m_incr(cparg);
        return cparg;
      }
    }
  } ///< POSTCONDICION RET[N-1] == (DIG_1() | DIG_0())
    ///< POSTCONDICION RET[N-1] == DIG_1() => RET[N-2,0] == REGD_N_0()

  ///< HAY QUE VER SI ES POTENCIA DE B : is_any_B_pow
  template <uint64_t B, size_t N, size_t n>
    requires((n > 0) && (n < N) && (N > 1)) /// N == SIZE_FROM n == SIZE_TO
  constexpr inline reg_digs_t<B, N> aprox_units_divB_n(const reg_digs_t<B, N> &arg) noexcept
  {
    if constexpr (n == N - 1)
    {
      return aprox_units_divB<B, N>(arg);
    }
    else
    {
      reg_digs_t<B, N> cparg{arg}; // n = N-x => x = N-n
      for (size_t ix{0}; ix < N - n; ++ix)
      {
        cparg = aprox_units_divB<B, N>(cparg);
      }
      return cparg;
    }
  }

  // comentar m_mult
  template <uint64_t B>
  constexpr inline dig_t<B> m_mult(dig_t<B> &left, dig_t<B> right) noexcept
  {
    using namespace type_traits;
    using UINT_T = dig_t<B>::UINT_T;
    using SIG_UINT_T = dig_t<B>::SIG_UINT_T;
    constexpr UINT_T LIMIT{sqrt_max<UINT_T>()};
    constexpr bool BASE_IS_BOTTOM_LIMIT{B < LIMIT};
    using ELEC_UINT_T =
        std::conditional_t<
            BASE_IS_BOTTOM_LIMIT, /// CONDITION
            UINT_T,               /// IF CONDITION==TRUE THEN
            SIG_UINT_T            /// ELSE THEN
            >;
    constexpr ELEC_UINT_T Base{B};
    const ELEC_UINT_T left_opndo{left()};
    const ELEC_UINT_T right_opndo{right()};
    /// this narrowing of integer types is correct
    const ELEC_UINT_T mult{left_opndo * right_opndo};
    /// this narrowing of integer types is correct
    const UINT_T carry{mult / Base};
    /// this narrowing of integer types is correct
    const UINT_T result{mult % Base};
    left = dig_t<B>(result);
    return carry;
  }

  // comentar mult
  template <uint64_t B>
  constexpr inline std::tuple<dig_t<B>, dig_t<B>>
  mult(dig_t<B> left, dig_t<B> right) noexcept
  {
    using namespace type_traits;
    using UINT_T = dig_t<B>::UINT_T;
    using SIG_UINT_T = dig_t<B>::SIG_UINT_T;
    constexpr UINT_T LIMIT{sqrt_max<UINT_T>()};
    constexpr bool BASE_IS_BOTTOM_LIMIT{B < LIMIT};
    using ELEC_UINT_T = std::conditional_t<BASE_IS_BOTTOM_LIMIT, /// CONDITION
                                           UINT_T,               /// IF CONDITION==TRUE THEN
                                           SIG_UINT_T            /// ELSE THEN
                                           >;
    constexpr ELEC_UINT_T Base{B};
    const ELEC_UINT_T left_opndo{left()};
    const ELEC_UINT_T right_opndo{right()};
    /// this narrowing of integer types is correct
    const ELEC_UINT_T mult{left_opndo * right_opndo};
    /// this narrowing of integer types is correct
    const UINT_T carry{mult / Base};
    /// this narrowing of integer types is correct
    const UINT_T result{mult % Base};

    return std::make_tuple(dig_t{result}, dig_t{carry});
  }

  // comentar m_mult_with_carryin_dig
  template <uint64_t B>
  constexpr inline dig_t<B> m_mult_with_carryin_dig(
      dig_t<B> &left, dig_t<B> right, dig_t<B> carryin) noexcept
  {
    using namespace type_traits;
    using UINT_T = typename dig_t<B>::uint_t;
    using SIG_UINT_T = typename dig_t<B>::sig_uint_t;
    constexpr UINT_T LIMIT{sqrt_max<UINT_T>()};
    constexpr bool BASE_IS_BOTTOM_LIMIT{B < LIMIT};
    using ELEC_UINT_T = std::conditional_t<BASE_IS_BOTTOM_LIMIT, /// CONDITION
                                           UINT_T,               /// IF CONDITION==TRUE THEN
                                           SIG_UINT_T            /// ELSE THEN
                                           >;
    constexpr ELEC_UINT_T Base{B};
    const ELEC_UINT_T left_opndo{left()};
    const ELEC_UINT_T right_opndo{right()};
    const ELEC_UINT_T carryin_opndo{carryin()};
    /// this narrowing of integer types is correct
    const ELEC_UINT_T mult{static_cast<ELEC_UINT_T>(left_opndo * right_opndo)};
    const ELEC_UINT_T mult_w_carry{
        static_cast<ELEC_UINT_T>(mult + carryin_opndo)};
    /// this narrowing of integer types is correct
    const UINT_T carryout{static_cast<UINT_T>(mult_w_carry / Base)};
    /// this narrowing of integer types is correct
    const UINT_T result{static_cast<UINT_T>(mult_w_carry % Base)};
    left = dig_t<B>(result);
    return carryout;
  }

  // comentar mult_with_carryin_dig
  template <uint64_t B>
  constexpr inline std::tuple<dig_t<B>, dig_t<B>> mult_with_carryin_dig(
      dig_t<B> left, dig_t<B> right, dig_t<B> carryin) noexcept
  {
    using namespace type_traits;
    using UINT_T = dig_t<B>::UINT_T;
    using SIG_UINT_T = dig_t<B>::SIG_UINT_T;
    constexpr UINT_T LIMIT{sqrt_max<UINT_T>()};
    constexpr bool BASE_IS_BOTTOM_LIMIT{B < LIMIT};
    using ELEC_UINT_T =
        std::conditional_t<
            BASE_IS_BOTTOM_LIMIT, /// CONDITION
            UINT_T,               /// IF TRUE THEN
            SIG_UINT_T            /// ELSE THEN
            >;
    constexpr ELEC_UINT_T Base{B};
    const ELEC_UINT_T left_opndo{left()};
    const ELEC_UINT_T right_opndo{right()};
    const ELEC_UINT_T carryin_opndo{carryin()};
    /// this narrowing of integer types is correct
    const ELEC_UINT_T mult{static_cast<ELEC_UINT_T>(left_opndo * right_opndo)};
    const ELEC_UINT_T mult_w_carry{
        static_cast<ELEC_UINT_T>(mult + carryin_opndo)};
    /// this narrowing of integer types is correct
    const UINT_T carryout{static_cast<UINT_T>(mult_w_carry / Base)};
    /// this narrowing of integer types is correct
    const UINT_T result{static_cast<UINT_T>(mult_w_carry % Base)};

    return std::make_tuple(dig_t<B>(result), dig_t<B>(carryout));
  }

  // comentar m_mult
  template <uint64_t B, size_t N>
  constexpr inline dig_t<B> m_mult(reg_digs_t<B, N> &left, dig_t<B> right) noexcept
  {
    using dig_t = dig_t<B>;
    using namespace type_traits;

    dig_t old_carry{0};
    for (size_t index = 0; index < N; ++index)
    {
      const dig_t carry = m_mult_with_carryin_dig(left[index], right, old_carry);
      old_carry = carry;
    }
    return old_carry;
  }

  // comentar mult
  template <uint64_t B, size_t N>
  constexpr inline std::tuple<reg_digs_t<B, N>, dig_t<B>>
  mult(const reg_digs_t<B, N> &left, dig_t<B> right) noexcept
  {
    using dig_t = dig_t<B>;
    using reg_digs_t = reg_digs_t<B, N>;
    using namespace type_traits;
    std::tuple<reg_digs_t, dig_t> ret{};

    dig_t old_carry{0};
    for (size_t index = 0; index < N; ++index)
    {
      const auto carry = mult_with_carryin_dig(left[index], right, old_carry);
      old_carry = std::get<1>(carry);
      std::get<0>(ret)[index] = std::get<0>(carry);
    }
    std::get<1>(ret) = old_carry;
    return ret;
  }

  // comentar m_mult
  template <uint64_t B, size_t N>
  constexpr inline reg_digs_t<B, N>
  m_mult(reg_digs_t<B, N> &left, const reg_digs_t<B, N> &right) noexcept
  {
    using reg2_digs_t = reg_digs_t<B, 2 * N>;

    reg2_digs_t right_sup{right};
    reg2_digs_t accumulator{};
    for (size_t index_x = 0; index_x < N; ++index_x)
    {
      reg2_digs_t cp_right_sup{right_sup};
      m_mult(cp_right_sup, left[index_x]);
      m_sum(accumulator, cp_right_sup);
      right_sup <<= 1;
    }
    left = accumulator.template subregister<0, N>();
    return (accumulator.template subregister<N, 2 * N>());
  }

  // comentar mult
  template <uint64_t B, size_t N>
  constexpr inline std::tuple<reg_digs_t<B, N>, reg_digs_t<B, N>>
  mult(const reg_digs_t<B, N> &left, const reg_digs_t<B, N> &right) noexcept
  {
    using reg2_digs_t = reg_digs_t<B, 2 * N>;
    using reg_digs_t = reg_digs_t<B, N>;
    std::tuple<reg_digs_t, reg_digs_t> ret{};

    reg2_digs_t right_sup{right};
    reg2_digs_t accumulator{};
    for (size_t index_x = 0; index_x < N; ++index_x)
    {
      reg2_digs_t cp_right_sup{right_sup};
      m_mult(cp_right_sup, left[index_x]);
      m_sum(accumulator, cp_right_sup);
      right_sup <<= 1;
    }
    std::get<0>(ret) = accumulator.template subregister<0, N>();
    std::get<1>(ret) = accumulator.template subregister<N, 2 * N>();
    return ret;
  }

  // comentar num_right_zeros
  template <uint64_t B, std::size_t L>
  constexpr inline std::size_t num_right_zeros(const reg_digs_t<B, L> &arg) noexcept
  {
    std::size_t num{0};
    while (arg[num].is_0())
    {
      ++num;
    }
    return num;
  }

  template <uint64_t B, size_t N>
  constexpr inline
      /// std::array<SIG_UINT_T,2>{}[0] == COC
      /// std::array<SIG_UINT_T,2>{}[1] == REM
      /// PRECONDICION
      ///		DSOR != 0
      ///			Y
      ///		DSOR != 1
      /// PRECONDICION
      /// 	EXISTE 0<=N<B
      ///		PROCURAR EN DIVISION QUE N NOT IN {0,1}
      /// 		TAL QUE
      /// 	(DSOR*N <= REM) Y (DSOR*(N+1)>REM)
      dig_t<B>
      aprox_coc_dig_rem_div_dsor(
          const reg_digs_t<B, N> &rem, const reg_digs_t<B, N> &dsor) noexcept
  {

    using namespace type_traits;
    using dig_t = dig_t<B>;
    using SIG_UINT_T = dig_t::SIG_UINT_T;

    /// BEGIN : CONSTANTES PARA EL CONTROL DE LA REDUCCION POR APROXIMACION
    const size_t dsor_msb(dsor.index_of_MSDig());
    const size_t rem_msb(rem.index_of_MSDig());
    /// END   : CONSTANTES PARA EL CONTROL DE LA REDUCCION POR APROXIMACION

    /// BEGIN : VARIABLES COPIA SOBRE LAS QUE SE HACE LA REDUCCION
    auto rem_aprox{rem};
    auto dsor_aprox{dsor};
    /// END   : VARIABLES COPIA SOBRE LAS QUE SE HACE LA REDUCCION

    /// BEGIN : REDUCCION POR APROXIMACION DE REM_APROX Y DSOR_APROX
    ///		  CUIDADOS DE QUE LAS REDUCCIONES SE HAGAN EN AMBAS PARTES POR
    ///         IGUAL
    size_t ix = 0;
    while (ix < std::min(dsor_msb, rem_msb))
    {
      if (rem_aprox == dsor_aprox)
        return dig_t::dig_1();
      /// POR SI EL DIVISOR ESTA MULTIPLICADO POR UNA POTENCIA DE LA BASE
      const auto n_rzeros{num_right_zeros(dsor)};
      if (n_rzeros > 0)
      {
        rem_aprox >>= n_rzeros;
        dsor_aprox >>= n_rzeros;
      }
      if (rem_aprox == dsor_aprox)
        return dig_t::dig_1();
      /// PARA EL CONTROL DE QUE LAS REDUCCIONES
      /// SE HAGAN EN AMBOS ARGUMENTOS POR IGUAL
      const auto temp_dsor{aprox_units_divB(dsor_aprox)};
      const auto temp_rem{aprox_units_divB(rem_aprox)};
      const auto dsor_aprox_msb{temp_dsor.index_of_MSDig()};
      const auto rem_aprox_msb{temp_rem.index_of_MSDig()};
      const auto dif_dsor_msb = dsor_msb - dsor_aprox_msb;
      const auto dif_rem_msb = rem_msb - rem_aprox_msb;
      if ((dif_dsor_msb == dif_rem_msb) && (dif_dsor_msb > 0))
      {
        /// REDUCCION DE LOS ARGUMENTOS POR APROXIMACION
        rem_aprox = temp_rem;
        dsor_aprox = temp_dsor;
        if (dsor_aprox_msb == 0)
          break;
      }
      else
      {
        /// FIN TEMPRANO
        break;
      }
      ++ix;
    }
    /// END   : REDUCCION POR APROXIMACION DE REM_APROX Y DSOR_APROX

    /// BEGIN : CALCULO DE COCIENTE APROXIMADO
    const size_t dsor_aprox_msb = dsor_aprox.index_of_MSDig();
    const SIG_UINT_T rem_uint{SIG_UINT_T(
        SIG_UINT_T(rem_aprox) / SIG_UINT_T(std::pow(size_t(B), dsor_aprox_msb)))};
    SIG_UINT_T dsor_uint{SIG_UINT_T(dsor_aprox[dsor_aprox_msb]())};
    SIG_UINT_T coc = rem_uint / dsor_uint;
    /// END   : CALCULO DE COCIENTE APROXIMADO

    /// BEGIN : POR SI OCURRE ESTA DESGRACIA
    if (coc > B - 1)
    {
      coc = B - 1;
    }
    /// END   : POR SI OCURRE ESTA DESGRACIA

    return dig_t{coc};
  }

  ///< FUNCION DE APROXIMACION DEL COCIENTE : 1 SOLO DIGITO
  ///< PARA LA DIVISION Y DEDUCCION DEL NUEVO RESTO
  ///< CALC_COC_DIG_REM_DIV_DSOR CALCULA DIGITO COCIENTE DE
  ///< REMAINDER DIVIDIDO POR DIVISOR
  template <uint64_t B, size_t N>
    requires(N > 0)
  constexpr inline
      /// std::array<SIG_UINT_T,2>{}[0] == COC
      /// std::array<SIG_UINT_T,2>{}[1] == REM
      /// PRECONDICION
      ///		DSOR != 0
      ///			Y
      ///		DSOR != 1
      /// PRECONDICION
      /// 	EXISTE 0<=N<B
      ///		PROCURAR EN DIVISION QUE N NOT IN {0,1}
      /// 		TAL QUE
      /// 	(DSOR*N <= REM) Y (DSOR*(N+1)>REM)
      std::tuple<dig_t<B>, reg_digs_t<B, N>>
      calc_coc_dig_rem_div_dsor(
          const reg_digs_t<B, N> &rem, const reg_digs_t<B, N> &dsor) noexcept
  {

    using dig_t = dig_t<B>;
    using reg_digs_t = reg_digs_t<B, N>;

    reg_digs_t rem_aprox{rem};
    if (rem < dsor)
    {
      return std::make_tuple(dig_t::dig_0(), rem_aprox);
    }
    else if (rem == dsor)
    {
      return std::make_tuple(dig_t::dig_1(), reg_digs_t{});
    }
    else
    {
      dig_t coc_aprox{aprox_coc_dig_rem_div_dsor(rem, dsor)};
      bool coc_es_correcto{false};
      while (!coc_es_correcto)
      {
        ///  BEGIN dsor_x_coc *= coc_aprox ; inicialmente dsor_x_coc == dsor
        reg_digs_t dsor_x_coc{dsor};
        auto dsor_x_coc_high = m_mult(dsor_x_coc, coc_aprox);
        ///  END dsor_x_coc *= coc_aprox ; inicialmente dsor_x_coc == dsor
        rem_aprox = rem;
        if ((rem_aprox >= dsor_x_coc) && (dsor_x_coc_high.is_0()))
        {
          m_subtract(rem_aprox, dsor_x_coc);
          if (rem_aprox < dsor)
          {
            return std::make_tuple(coc_aprox, rem_aprox);
          }
          else
          {
            ++coc_aprox;
            continue;
          }
        }
        else
        {
          --coc_aprox;
          continue;
        }
      }
    }
    /// AQUI NO DEBERIA LLEGAR NUNCA : ERROR : TODO A 0
    return std::make_tuple(dig_t{}, reg_digs_t{});
  }

  /// FUNCIONES DE IMPLEMENTACION DE LA DIVISION ENTRE DOS REGISTROS DE DIGITOS
  template <uint64_t B, size_t N, size_t M>
    requires(N > 0) && (M > 0)
  constexpr inline
      /// (array[0] == cociente , array[1] == resto)
      std::tuple<reg_digs_t<B, std::max(N, M)>, reg_digs_t<B, std::max(N, M)>, bool>
      fediv(
          const reg_digs_t<B, N> &larg, /// dndo
          const reg_digs_t<B, M> &rarg  /// dsor
          ) noexcept
  {

    constexpr size_t MaxParam = std::max(N, M);
    using dig_t = dig_t<B>;
    using SIG_UINT_T = dig_t::SIG_UINT_T;
    using base_t = reg_digs_t<B, MaxParam>;
    using base_t::regd_0;
    using base_t::regd_1;

    using ret_type = std::tuple<base_t, base_t, bool>;
    ///                         cociente resto   error div 0

    base_t dndo{larg};
    base_t dsor{rarg};

    /// MOST SIGNIFICANT DIGIT DEL DIVISOR [DEL NUMERO NO DEL TIPO]
    const int64_t dndo_MSDig{static_cast<std::int64_t>(dndo.index_of_MSDig())};
    /// MOST SIGNIFICANT DIGIT DEL DIVIDENDO [DEL NUMERO NO DEL TIPO]
    const int64_t dsor_MSDig{static_cast<std::int64_t>(dsor.index_of_MSDig())};

    const int64_t dist_dndo_dsor{(dndo_MSDig >= dsor_MSDig)
                                     ? dndo_MSDig - dsor_MSDig
                                     : dsor_MSDig - dndo_MSDig};

    if (dsor.is_0())
    {
      return ret_type{base_t{}, base_t{}, true};
    }
    else if (dndo.is_0())
    {
      return ret_type{base_t{}, base_t{}, false};
    }
    else if (dsor.is_1())
    {
      return ret_type{dndo, regd_0(), false};
    }
    else if (dndo < dsor)
    {
      return ret_type{regd_0(), dndo, false};
    }
    else if (dndo == dsor)
    {
      return ret_type{regd_1(), regd_0(), false};
    }
    else
    {
      /// pl_dndo es el puntero (índice) del dígito mas significativo
      /// por debajo (low) del rem cogido
      /// marca el siguiente dígito del dividendo a bajar

      if ((dndo_MSDig <= 1) && (dsor_MSDig <= 1))
      {
        /// dndo_uint = dndo[1]()*B + dndo[0]()
        const SIG_UINT_T dndo_uint{static_cast<SIG_UINT_T>(
            (static_cast<SIG_UINT_T>(dndo[1]()) * static_cast<SIG_UINT_T>(B)) +
            static_cast<SIG_UINT_T>(dndo[0]()))};
        /// dsor_uint = dsor[0]()
        const SIG_UINT_T dsor_uint{static_cast<SIG_UINT_T>(
            (static_cast<SIG_UINT_T>(dsor[1]()) * static_cast<SIG_UINT_T>(B)) +
            static_cast<SIG_UINT_T>(dsor[0]()))};
        /// rem_uint = dndo_uint % dsor_uint
        const SIG_UINT_T rem_uint =
            static_cast<SIG_UINT_T>(dndo_uint % dsor_uint);
        /// coc_uint = dndo_uint / dsor_uint
        const SIG_UINT_T coc_uint =
            static_cast<SIG_UINT_T>(dndo_uint / dsor_uint);

        return ret_type{coc_uint, rem_uint, false};
      }
      else if (dist_dndo_dsor < 1)
      {
        base_t rem{};
        base_t coc{};
        for (int64_t ix{0}; ix < dndo_MSDig + 1; ++ix)
        {
          rem[ix] = dndo[ix];
        }
        auto result{calc_coc_dig_rem_div_dsor(rem, dsor)};
        const dig_t coc_dig{std::move(std::get<0>(result))};
        /// ASIGNACION BIEN HECHA CUANDO REMAINDER != 0
        rem = std::move(std::get<1>(result));
        coc[0] = coc_dig;
        return ret_type{std::move(coc), std::move(rem), false};
      }
      else
      {

        base_t rem{};
        base_t coc{};

        int64_t pl_dndo{dist_dndo_dsor};
        for (int64_t offset{0}; pl_dndo < dndo_MSDig + 1; ++pl_dndo, ++offset)
        {
          rem[offset] = dndo[pl_dndo];
        }
        pl_dndo = dist_dndo_dsor - 1;

        do
        {
          /// CÁLCULO DEL REM Y EL DÍGITO DE COCIENTE
          /// CON LOS ACTUALES RESTO (REM) Y EL DIVISOR (DSOR)
          auto result{calc_coc_dig_rem_div_dsor(rem, dsor)};
          const dig_t coc_dig{std::move(std::get<0>(result))};
          /// ASIGNACION BIEN HECHA CUANDO REMAINDER != 0
          rem = std::move(std::get<1>(result));

          /// ACTUALIZACIÓN DEL COCIENTE CON EL DIGITO ENCONTRADO
          coc <<= 1;        // coc = coc * B
          coc[0] = coc_dig; // coc = coc + q  ; B-1 >= q > 0
          /// ACTUALIZACIÓN DEL RESTO CON EL NUEVO DÍGITO DEL DIVIDENDO
          if (pl_dndo >= 0)
          {
            rem <<= 1;
            rem[0] = dndo[pl_dndo];
          }
          /// BAJAMOS EL ÍNDICE DEL DIVIDENDO
          --pl_dndo;
        } while (pl_dndo > -2);

        return ret_type{std::move(coc), std::move(rem), false};
      }
    }
  }

  /// SOBRECARGA DE ISTREAM Y OSTREAM CON LOS OPERADORES DE DESPLAZAMEINTO
  template <uint64_t Base, size_t Length>
    requires((Base > 1) && (Length > 0))
  std::istream &operator>>(std::istream &is, reg_digs_t<Base, Length> &arg)
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
    constexpr size_t longitud_cadena{
        (
            (Length + 1) *
            static_cast<size_t>(
                std::ceil((std::log2(static_cast<long double>(Base)) + 1)))) +
        16};
    std::string sds{};
    sds.resize(longitud_cadena);
    sds.assign(longitud_cadena, '\0');
    /// TIPOS A SER UTILIZADOS EN LA FUNCIÓN: NOMBRE DE TIPOS CORTOS
    using Int_Type = typename dig_t<Base>::UINT_T;
    using inttype = typename dig_t<Base>::SIG_UINT_T;
    using dig_t = dig_t<Base>;
    using reg_digs_t = reg_digs_t<Base, Length>;
    /// INDICE QUE RECORRE EL STRING RECOGIDO DE ENTRADA
    size_t indice{0};
    /// VARIABLES PARA ACCIONES EN LOS ESTADOS
    Int_Type digito{0};
    size_t longitud{0};
    inttype numero_base_recogido{0};
    /// VARIABLE DE RETORNO (BINDED TO THE ARG BY REFERENCE)
    reg_digs_t &numero_ret{arg};
    /// ESTADO ACTUAL
    estado_e est_act = e0ini;
    /// CARACTER QUE GUARDARA EL INDICADO POR EL INDICE DEL STRING RECOGIDO DESDE
    /// CIN
    char c = type_traits::nullchar<char>;
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
          numero_ret = reg_digs_t::regd_0();
        }
        else
        {
          est_act = e0ini;
          digito = 0;
          longitud = 0;
          numero_base_recogido = 0;
          numero_ret = reg_digs_t::regd_0();
        }
      }
      break;
      case e2dig:
      {
        const bool dig_c{(c >= '0') && (c <= '9')};
        const bool dig_lt_Base{digito < Base};
        const bool c_es_dig_B{dig_c && dig_lt_Base};
        const bool dig_B_cdl{c_es_dig_B && (longitud <= Length - 1)};
        const bool tt_dig_B_cdl{(c == ':') && dig_lt_Base &&
                                (longitud <= Length - 1)};
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
          numero_ret = reg_digs_t::regd_0();
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
          numero_ret = reg_digs_t::regd_0();
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
          numero_ret = reg_digs_t::regd_0();
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
          numero_ret = reg_digs_t::regd_0();
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
  std::ostream &operator<<(std::ostream &os, const reg_digs_t<Base, Long> &arg)
  {
    os << arg.to_string();
    return (os);
  }

} // END NAMESPACE NumRepr

#endif // REG_DIGS_T_HPP_INCLUDED
