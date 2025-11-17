#ifndef INT_REG_DIGS_T_HPP
#define INT_REG_DIGS_T_HPP

#include "nat_reg_digs_t.hpp"
#include "core/internal/basic_types.hpp"

#include <istream>
#include <ostream>

namespace NumRepr
{

  // using type_traits::suitable_base;

  template <uint64_t B, size_t R>
    requires((B > 1) && (R > 0))
  struct int_reg_digs_t : public nat_reg_digs_t<B, R + 1>
  {

    /// NECESITAMOS:
    ///	B = BASE EN LA QUE TRABAJAMOS
    ///			=> TIPO BASE 	UINT_T
    ///			=> TIPO 		SIG_UINT_T  ALGO MAYOR (EL DOBLE) QUE UN UINT_T
    ///			=> TIPO			SIG_SINT_T  TIPO CON SIGNO DONDE CABE UN UINT_T
    /// L = LONGITUD DEL MÓDULO DEL NÚMERO EN DÍGITOS INCLUYENDO EL SIGNO
    /// R = LONGITUD DEL MÓDULO DEL NÚMERO EN DÍGITOS SIN CONTAR EL SIGNO
    /// DÍGITO EXTRA PARA EL SIGNO
    ///     UINT_T SERÁ DEDUCIDO DEL PARÁMETRO TEMPLATE R
    ///     EL TIPO UNSIGNEDINT SIG_UINT_T SERÁ DEDUCIDO DEL TIPO PARÁMETRO UINT_T
    ///     EL TIPO SIGNEDINT   SIG_SINT_T SERÁ DEDUCIDO DEL TIPO PARÁMETRO UINT_T
    ///     DIG_T ES UN ALIAS RECOMENDABLE PARA EL TIPO DÍGITO DE ESTE "CONTAINER"
    ///     BASE_T ES UN ALIAS CONVENIENTE PARA EL TIPO DEL QUE DERIVA ESTA CLASE
    ///     BASE_N_T ES UN ALIAS CONVENIENTE PARA LOS TIPOS BASE_T PERO DE
    ///          LONGITUD DIFERENTE QUE L
    ///     INT_REG_N_DIGS_T ES UN ALIAS CONVENIENTE PARA LOS TIPOS QUE INSTANCIAN
    ///          ESTA MISMA CLASS TEMPLATE PERO CON LONGITUD DIFERENTE QUE L
    ///     EL VALOR DE R [LONGITUD SIN EL SIGNO] HA DE SER MAYOR QUE 0
    ///     EL VALOR DE L [LONGITUD QUE INCLUYE EL SIGNO] HA DE SER MAYOR QUE 1
    ///     EL VALOR DE B [RADIX O BASE] ADMISIBLE ES CUALQUIERA MAYOR QUE 1

    using dig_t = dig_t<B>;

    using uint_t = typename dig_t::uint_t;

    static constexpr std::size_t L{R + 1};

    using SIG_UINT_T = typename type_traits::sig_UInt_for_UInt_t<uint_t>;

    using SIG_SINT_T = typename type_traits::sig_SInt_for_UInt_t<uint_t>;

    template <std::size_t N>
      requires(N > 0)
    using base_N_t = nat_reg_digs_t<B, N>;

    using base_t = base_N_t<L>;

    template <std::size_t N>
      requires(N > 0)
    using int_reg_N_digs_t = int_reg_digs_t<B, N>;
    using base_t::regd_0;
    using base_t::regd_1;
    using base_t::regd_Bm1;

  private:
    /// MÉTODOS SOBRE EL SIGNO
    constexpr dig_t get_sign() const { return ((*this)[R]); }
    constexpr bool is_plus() const { return (((*this)[R]).is_0()); }
    constexpr bool is_minus() const { return (!(((*this)[R]).is_0())); }
    constexpr void flip_sign() { (*this)[R] = is_plus() ? dig_Bm1() : dig_0(); }
    constexpr void set_plus() { (*this)[R] = dig_0(); }
    constexpr void set_minus() { (*this)[R] = dig_Bm1(); }
    constexpr void normalize_sign()
    {
      base_t &r_base_cthis{*static_cast<base_t *>(this)};
      // Normalizar -0 a +0 (caso más común de inconsistencia)
      if (is_minus() && r_base_cthis.is_0())
      {
        set_plus();
      }
    }
    static consteval dig_t minus() { return dig_Bm1(); }
    static consteval dig_t plus() { return dig_0(); }

  public:
    /// ME LAS TRAIGO DE DÍGITO PARA PODER TRABAJAR CON ELLAS DE FORMA DIRECTA
    static constexpr dig_t dig_0() noexcept { return dig_t::dig_0(); }
    static constexpr dig_t dig_1() noexcept { return dig_t::dig_1(); }
    static constexpr dig_t dig_Bm1() noexcept { return dig_t::dig_max(); }
    static constexpr dig_t dig_max() noexcept { return dig_t::dig_max(); }
    static constexpr dig_t dig_Bm2() noexcept { return dig_t::dig_submax(); }
    static constexpr dig_t dig_submax() noexcept { return dig_t::dig_submax(); }
    static constexpr uint_t ui_0() noexcept { return dig_t::ui_0(); }
    static constexpr uint_t ui_1() noexcept { return dig_t::ui_1(); }
    static constexpr uint_t ui_Bm1() noexcept { return B - 1; }
    static constexpr uint_t ui_B() noexcept { return B; }
    static constexpr uint_t ui_submax() noexcept { return dig_t::ui_submax(); }
    static constexpr uint_t ui_max() noexcept { return dig_t::ui_max(); }
    static constexpr SIG_UINT_T sui_0() noexcept { return dig_t::sui_0(); }
    static constexpr SIG_UINT_T sui_1() noexcept { return dig_t::sui_1(); }
    static constexpr SIG_UINT_T sui_Bm1() noexcept { return dig_t::sui_B() - 1; }
    static constexpr SIG_UINT_T sui_B() noexcept { return dig_t::sui_B(); }
    static constexpr SIG_UINT_T sui_Bp1() noexcept { return dig_t::sui_B() + 1; }

    ///< FUNCIONES DE AYUDA QUE DEVUELVEN UNA CONSTANTE
    ///< CONSTANTES DE EVALUACIÓN EN TIEMPO DE COMPILACIÓN = consteval functions
    ///< CONSTANTES DE TIPO base_t
    ///< base_t : ES EL TIPO DEL nat_reg_digs_t<DIG_T<B>,L> QUE ES EL TIPO BASE

    ///< 	CONSTANTES DE TIPO BASE_T
    ///<  SE HEREDAN DEL TIPO BASE_T

    ///< 	CONSTANTES DE TIPO INT_REG_DIGS_T
    ///<  ALTERNA VALOR POSITIVO CON NEGATIVO
    ///<  ESTAS CONSTANTES SON NUEVAS PUES EN LA CLASE BASE NO HAY SIGNO

    inline static consteval int_reg_digs_t sregd_0() noexcept
    {
      // Crear usando constructor por defecto que ya inicializa en 0
      return int_reg_digs_t{};
    }

    inline static consteval int_reg_digs_t sregd_1() noexcept
    {
      return int_reg_digs_t{regd_1()};
    }

    inline static consteval int_reg_digs_t sregd_m1() noexcept
    {
      int_reg_digs_t result;
      result.fill(dig_Bm1());
      return result;
    }

    inline static consteval int_reg_digs_t sregd_Bm1() noexcept
    {
      int_reg_digs_t ret{regd_0()};
      ret[0] = dig_Bm1();
      return ret;
    }

    inline static consteval int_reg_digs_t sregd_mBp1() noexcept
    {
      using base_Rm1_t = base_N_t<R - 1>;
      using base_1_t = base_N_t<1>;
      using base_R_t = base_N_t<R>;
      base_R_t result{base_Rm1_t::regd_0().cat(base_1_t::regd_1())};
      return {result.cat(base_1_t::regd_1())};
    }

    inline static consteval int_reg_digs_t sregd_B() noexcept
    {
      using base_Lm2_t = base_N_t<R - 1>;
      using base_R_t = base_N_t<L - 1>;
      using base_1_t = base_N_t<1>;
      base_R_t result = base_Lm2_t::regd_0().cat(base_1_t::regd_1());
      return {result.cat(base_1_t::regd_1())};
    }

    ///< B = 16 ; L =3 ;
    ///< mB -16=-(FFF-010+001)=-(FEF+001)=-(FF0)

    inline static consteval int_reg_digs_t sregd_mB() noexcept
    {
      const int_reg_digs_t result{base_N_t<L>::regd_0()};
      result[L - 1] = dig_Bm1();
      result[1] = dig_1();
      return result;
    }

    template <std::size_t n>
      requires((n >= 0) && (n < L - 1))
    inline static consteval int_reg_digs_t sregd_pow_n_B() noexcept
    {
      int_reg_digs_t ret{regd_0()};
      ret[n] = dig_1();
      return ret;
    }

    template <std::size_t n>
      requires((n >= 0) && (n < L - 1))
    inline static consteval int_reg_digs_t sregd_m_pow_n_B() noexcept
    {
      int_reg_digs_t ret{regd_0()};
      ret[n] = dig_1();
      ret[L - 1] = dig_Bm1();
      return ret;
    }

    template <std::size_t n>
      requires((n >= 0) && (n < L - 1))
    inline static consteval int_reg_digs_t sregd_pow_n_B_m1() noexcept
    {
      int_reg_digs_t ret{regd_0()};
      for (std::size_t ix{0}; ix < n; ++ix)
      {
        ret[ix] = dig_Bm1();
      }
      ret[L - 1] = dig_Bm1();
      return ret;
    }

    template <std::size_t n>
      requires((n >= 0) && (n < L - 1))
    inline static consteval int_reg_digs_t sregd_m_pow_n_B_p1() noexcept
    {
      int_reg_digs_t ret{regd_0()};
      ret[n] = dig_1();
      ret[L - 1] = dig_Bm1();
      return ret;
    }

    /// 000100000
    /// 000099999
    /// 999900000
    /// 999900001
    template <std::size_t n>
      requires((n > 0) && (n < L - 1))
    inline static consteval int_reg_digs_t sregd_m_pow_n_B_m1() noexcept
    {
      int_reg_digs_t ret{regd_0()};
      ret[0] = dig_1();
      for (int ix = n; ix < L; ++ix)
        ret[ix] = dig_Bm1();
      return ret;
    }

    /****************************/
    /*						  */
    /*    CONSTRUIR NUMERO	  */
    /*						  */
    /****************************/

  public:
    /// CONSTRUCTOR POR DEFECTO
    consteval inline int_reg_digs_t() noexcept
        : base_t{}
    {
      // Inicializar como 0 positivo manualmente
      for (size_t i = 0; i < L; ++i)
      {
        (*this)[i] = dig_0();
      }
    }

    /// CONSTRUCTOR POR LISTA DE DIGITOS
    constexpr inline int_reg_digs_t(const std::initializer_list<dig_t> &arg) noexcept
        : base_t{arg}
    {
      normalize_sign();
    }

    /// CONSTRUCTOR POR ARGUMENTOS DIGITOS SIN LIMITE: DEDUCE EL TIPO
    template <typename... Ts>
      requires(std::is_same_v<Ts, dig_t> && ...)
    constexpr inline int_reg_digs_t(const Ts &...args) noexcept
        : base_t{(utilities::ugly_pack_details::pack2array<Ts...>{})(args...)}
    {
      normalize_sign();
    }

    /// BEGIN : CONSTRUCTOR COPIA/MOVIMIENTO DESDE UN ARRAY DE DIGITOS

  private:
    /// FUNCION DELEGADA PARA COPIA DE UN ARRAY DEL MISMO TAMANO
    /// O UN TAMANO CUALQUIERA "N"
    template <std::size_t N>
      requires(N > 0)
    void copy_arg_N(const base_N_t<N> &arg)
    {
      int_reg_digs_t &cthis{*this};
      if constexpr (N < L)
      {
        for (std::size_t ix{N}; ix < L; ++ix)
          cthis[ix] = dig_0();
        for (std::size_t ix{0}; ix < N; ++ix)
          cthis[ix] = arg[ix];
        normalize_sign();
      }
      else
      {
        for (std::size_t ix{0}; ix < L; ++ix)
          cthis[ix] = arg[ix];
        normalize_sign();
      }
    }

    /// FUNCION DELEGADA PARA MOVER DE UN ARRAY DEL MISMO TAMANO
    /// O UN TAMANO CUALQUIERA "N"
    template <std::size_t N>
      requires(N > 0)
    void move_arg_N(base_N_t<N> &&arg)
    {
      int_reg_digs_t &cthis{*this};
      if constexpr (N < L)
      {
        for (std::size_t ix{N}; ix < L; ++ix)
          cthis[ix] = std::move(dig_0());
        for (std::size_t ix{0}; ix < N; ++ix)
          cthis[ix] = std::move(arg[ix]);
        normalize_sign();
      }
      else if constexpr (N > L)
      {
        for (std::size_t ix{0}; ix < L; ++ix)
          cthis[ix] = std::move(arg[ix]);
        normalize_sign();
      }
      else
      {
        static_cast<base_t>(cthis) = std::move(arg);
        normalize_sign();
      }
    }

  public:
    /// CONSTRUCTOR COPIA DESDE UN ARRAY DE DIGITOS
    template <std::size_t N>
      requires(N > 0)
    constexpr inline int_reg_digs_t(const base_N_t<N> &arg) noexcept
        : base_t{copy_arg_N<N>(arg)}
    {
    }

    /// CONSTRUCTOR MOVIMIENTO DESDE UN ARRAY DE DIGITOS
    template <std::size_t N>
      requires(N > 0)
    constexpr inline int_reg_digs_t(base_N_t<N> &&arg) noexcept
        : base_t{}
    {
      move_arg_N<N>(std::move(arg));
    }
    /// END   : CONSTRUCTOR COPIA/MOVIMIENTO DESDE UN ARRAY DE DIGITOS

  private:
    /// FUNCION DE NORMALIZACION A LA BASE B DE UN ARGUMENTOS
    /// EN UN PACK DE ENTEROS CUALQUIERA
    template <type_traits::integral_c... Ints_type>
      requires((sizeof...(Ints_type)) <= L)
    static constexpr inline base_t normalize(Ints_type... digits_pow_i) noexcept
    {
      using pack_type = typename utilities::ugly_pack_details::pack2array<Ints_type...>;
      using unique_type = typename pack_type::elem_type;
      constexpr std::size_t pack_sz{pack_type::pack_size()};
      if constexpr (type_traits::unsigned_integral_c<unique_type>)
      {
        if constexpr (type_traits::gt_sz_v<uint_t, unique_type>)
        {
          using SUInt_type = type_traits::sig_UInt_for_UInt_t<uint_t>;
          std::array<SUInt_type, L> ret_array{digits_pow_i...};
          base_t ret{};
          for (std::size_t ix{0}; ix < pack_sz; ++ix)
          {
            ret[ix] = dig_t(ret_array[ix]);
          }
          for (std::size_t ix{pack_sz}; ix < L; ++ix)
          {
            ret[ix] = dig_0();
          }
          ret.normalize_sign();
          return ret;
        }
        else
        {
          using SUInt_type =
              type_traits::sig_UInt_for_UInt_t<unique_type>;
          std::array<SUInt_type, L> ret_array{digits_pow_i...};
          base_t ret{};
          for (std::size_t ix{0}; ix < pack_sz; ++ix)
          {
            ret[ix] = dig_t(ret_array[ix]);
          }
          for (std::size_t ix{pack_sz}; ix < L; ++ix)
          {
            ret[ix] = dig_t(0);
          }
          ret.normalize_sign();
          return ret;
        }
      }
      else
      {
        using temp_SUInt_type =
            type_traits::sig_UInt_for_SInt_t<unique_type>;
        if constexpr (type_traits::gt_sz_v<uint_t, temp_SUInt_type>)
        {
          using SUInt_type = type_traits::sig_UInt_for_UInt_t<uint_t>;
          std::array<SUInt_type, L> ret_array{digits_pow_i...};
          base_t ret{};
          for (std::size_t ix{0}; ix < pack_sz; ++ix)
          {
            if (ret_array[ix] < 0)
              ret[ix] = dig_t((((ret_array[ix] / (-B)) + 1) * B) +
                              ret_array[ix]);
            else
              ret[ix] = dig_t(ret_array[ix]);
          }
          for (std::size_t ix{pack_sz}; ix < L; ++ix)
          {
            ret[ix] = dig_0();
          }
          ret.normalize_sign();
          return ret;
        }
        else
        {
          using SUInt_type = temp_SUInt_type;
          std::array<SUInt_type, L> ret_array{digits_pow_i...};
          base_t ret{};
          for (std::size_t ix{0}; ix < pack_sz; ++ix)
          {
            if (ret_array[ix] < 0)
              ret[ix] = dig_t((((ret_array[ix] / (-B)) + 1) * B) +
                              ret_array[ix]);
            else
              ret[ix] = dig_t(ret_array[ix]);
          }
          for (std::size_t ix{pack_sz}; ix < L; ++ix)
          {
            ret[ix] = dig_0();
          }
          ret.normalize_sign();
          return ret;
        }
      }
    }

  public:
    /// CONSTRUCTOR COPIA DESDE ARGUMENTOS ENTEROS SIN LIMITE : DEDUCE EL TIPO
    template <type_traits::integral_c... Ints_type>
      requires((sizeof...(Ints_type)) > 1)
    constexpr inline int_reg_digs_t(Ints_type... dig_pow_i) noexcept
        : base_t(dig_pow_i...)
    {
      normalize_sign();
    }

    /// CONSTRUCTOR COPIA POR REFERENCIA CONSTANTE
    template <std::size_t N>
      requires(N > 1)
    constexpr inline int_reg_digs_t(const int_reg_N_digs_t<N> &arg) noexcept
        : base_t{copy_arg_N<N>(arg)}
    {
    }

    /// CONSTRUCTOR POR MOVIMIENTO
    template <std::size_t N>
      requires(N > 1)
    constexpr inline int_reg_digs_t(int_reg_N_digs_t<N> &&arg) noexcept
        : base_t{std::move(move_arg_N(arg))}
    {
    }

    /// OPERACION ASIGNACION POR COPIA REFERENCIA EN LA QUE SE PUEDE COPIAR
    template <std::size_t N>
      requires(N > 1)
    constexpr inline int_reg_digs_t &operator=(int_reg_N_digs_t<N> &arg) noexcept
    {
      if (this != &arg)
      {
        copy_arg_N<N>(arg);
      }
      return (*this);
    }

    /// OPERACION ASIGNACION POR MOVIMIENTO
    template <std::size_t N>
      requires(N > 1)
    constexpr inline int_reg_digs_t &operator=(int_reg_N_digs_t<N> &&arg) noexcept
    {
      if (this != &arg)
      {
        move_arg_N<N>(std::move(arg));
      }
      return (*this);
    }

    /// OPERACION ASIGNACION POR COPIA EN LA QUE  _NO_  SE PUEDE COPIAR
    template <std::size_t N>
      requires(N > 1)
    constexpr inline const int_reg_digs_t &operator=(const int_reg_N_digs_t<N> &arg) noexcept
    {
      if (this != arg)
      {
        copy_arg_N<N>(arg);
      }
      return (*this);
    }

    /// OPERACION COPIA DESDE UN DIGITO (CONVERSION)
    constexpr inline int_reg_digs_t &operator=(dig_t arg) noexcept
    {
      base_t &cthis{*static_cast<base_t *>(this)};
      if (&(cthis[0]) != (&arg))
      {
        for (dig_t &dig : cthis)
          dig = dig_0();
        cthis[0] = arg;
      }
      return (*this);
    }

    /// OPERACION COPIA DESDE UN ENTERO (CONVERSION A LA BASE B)
    template <type_traits::integral_c Int_Type>
    constexpr inline int_reg_digs_t &operator=(Int_Type arg) noexcept
    {
      base_t &cthis{*static_cast<base_t *>(this)};
      if constexpr (std::is_signed_v<Int_Type>)
      {
        if (arg >= 0)
        {
          Int_Type creg_g{arg};
          if ((static_cast<base_t *>(this)) != (&arg))
          {
            for (std::size_t k{0u}; k < L; ++k)
            {
              cthis[k] = dig_t(creg_g % B);
              creg_g /= B;
            }
          }
          set_plus();
          return (cthis);
        }
        else
        {
          Int_Type creg_g{-arg};
          if ((static_cast<base_t *>(this)) != (&arg))
          {
            for (std::size_t k{0u}; k < L; ++k)
            {
              cthis[k] = dig_t(creg_g % B);
              creg_g /= B;
            }
          }
          this->mC_B();
          set_minus();
          return (cthis);
        }
      }
      else
      {
        Int_Type creg_g{arg};
        if ((static_cast<base_t *>(this)) != (&arg))
        {
          for (std::size_t k{0u}; k < L; ++k)
          {
            cthis[k] = dig_t(creg_g % B);
            creg_g /= B;
          }
        }
        set_minus();
        return (cthis);
      }
    }

    /// FORMACION DE UN REG_M_DIGS_T<M> DESDE EL THIS REG_DIGS_T (L)
    /// Y REG_N_DIGS_T<N> EN SUS DOS FORMAS: (*THIS)SEGUIDO(ARG) Y
    /// (ARG)SEGUIDO(*THIS) : OPERACIONES DE CONCATENACION
    /// A SU VEZ PUEDEN SER POR COPIA Y POR MOVIMIENTO

    /// CAT Y CONCAT SON HEREDADAS DE LA CLASE BASE

    /**********************************/
    /*							    */
    /*       Algunas Conversiones     */
    /*							    */
    /**********************************/

    /// NO NECESARIAMENTE CABE EL ENTERO DE ESTA CLASE EN UN INT_TYPE DEL SISTEMA
    /// ADVERTIR AL USUARIO QUE SON POSIBLES LOS OVERFLOWS Y NO SON TRATADOS POR
    /// ESTA CONVERSIÓN
    template <type_traits::integral_c Int_Type>
    constexpr inline
    operator Int_Type() const noexcept
    {
      std::uint64_t retInt{0};
      std::uint64_t BasePowIx{1};
      base_t cp_this{*this};

      if (is_minus())
      {
        cp_this.mC_B();
      }

      for (std::size_t k{0}; k < L; ++k)
      {
        retInt += (cp_this[k] * BasePowIx);
        BasePowIx *= B;
      }

      if constexpr (type_traits::signed_integral_c<Int_Type>)
      {
        if (is_minus())
          return (-retInt);
        else
          return retInt;
      }
      else
      {
        return retInt;
      }
    }

    /********************************/
    /*							  */
    /* OPERADORES COMPARATIVOS	  */
    /*							  */
    /********************************/

  public:
    template <std::size_t N>
      requires(N > 0)
    constexpr inline bool operator==(const int_reg_N_digs_t<N> &arg) const noexcept
    {
      const int_reg_digs_t &cthis{*this};
      if (int_reg_digs_t::get_sign() != arg.get_sign())
        return false;
      else
      {
        const base_t &cr_base_cthis{*static_cast<const base_t *const>(this)};
        const base_t &cr_base_arg{*static_cast<const base_t *const>(&arg)};
        return (cr_base_cthis == cr_base_arg);
      }
    }

    template <std::size_t N>
      requires(N > 0)
    constexpr inline bool operator==(const base_N_t<N> &arg) const noexcept
    {
      const int_reg_digs_t &cthis{*this};
      if (is_minus())
        return false;
      else
      {
        const base_t &cr_base_cthis{*static_cast<const base_t *const>(this)};
        const base_t &cr_base_arg{*static_cast<const base_t *const>(&arg)};
        return (cr_base_cthis == cr_base_arg);
      }
    }

    template <std::size_t N>
      requires(N > 0)
    constexpr inline bool operator!=(const int_reg_N_digs_t<N> &arg) const noexcept
    {
      const int_reg_digs_t &cthis{*this};
      if (get_sign() != arg.get_sign())
        return true;
      else
      {
        const base_t &cr_base_cthis{*static_cast<const base_t *const>(this)};
        const base_t &cr_base_arg{*static_cast<const base_t *const>(&arg)};
        return (cr_base_cthis != cr_base_arg);
      }
    }

    template <std::size_t N>
      requires(N > 0)
    constexpr inline bool operator!=(const base_N_t<N> &arg) const noexcept
    {
      const int_reg_digs_t &cthis{*this};
      if (is_minus())
        return true;
      else
      {
        const base_t &cr_base_cthis{*static_cast<const base_t *const>(this)};
        const base_t &cr_base_arg{*static_cast<const base_t *const>(&arg)};
        return (cr_base_cthis != cr_base_arg);
      }
    }

    template <std::size_t N>
      requires(N > 0)
    constexpr inline bool operator<=(const int_reg_N_digs_t<N> &arg) const noexcept
    {
      if (is_plus() && arg.is_minus())
        return false;
      else if (is_minus() && arg.is_plus())
        return true;
      else
      {
        const base_t &cr_base_cthis{*static_cast<const base_t *const>(this)};
        const base_t &cr_base_arg{*static_cast<const base_t *const>(&arg)};
        // Para números negativos: mayor magnitud = menor número (invertir comparación)
        // Para números positivos: mayor magnitud = mayor número (comparación normal)
        if (is_minus())
          return (cr_base_cthis >= cr_base_arg); // Invertido para negativos
        else
          return (cr_base_cthis <= cr_base_arg); // Normal para positivos
      }
    }

    template <std::size_t N>
      requires(N > 0)
    constexpr inline bool operator<=(const base_N_t<N> &arg) const noexcept
    {
      if (is_minus())
        return true;
      else
      {
        const base_t &cr_base_cthis{*static_cast<const base_t *const>(this)};
        const base_t &cr_base_arg{*static_cast<const base_t *const>(&arg)};
        return (cr_base_cthis <= cr_base_arg);
      }
    }

    template <std::size_t N>
      requires(N > 0)
    constexpr inline bool operator>=(const int_reg_N_digs_t<N> &arg) const noexcept
    {
      if (is_plus() && arg.is_minus())
        return true;
      else if (is_minus() && arg.is_plus())
        return false;
      else
      {
        const base_t &cr_base_cthis{*static_cast<const base_t *const>(this)};
        const base_t &cr_base_arg{*static_cast<const base_t *const>(&arg)};
        // Para números negativos: mayor magnitud = menor número (invertir comparación)
        // Para números positivos: mayor magnitud = mayor número (comparación normal)
        if (is_minus())
          return (cr_base_cthis <= cr_base_arg); // Invertido para negativos
        else
          return (cr_base_cthis >= cr_base_arg); // Normal para positivos
      }
    }

    template <std::size_t N>
      requires(N > 0)
    constexpr inline bool operator>=(const base_N_t<N> &arg) const noexcept
    {
      if (is_minus())
        return false;
      else
      {
        const base_t &cr_base_cthis{*static_cast<const base_t *const>(this)};
        const base_t &cr_base_arg{*static_cast<const base_t *const>(&arg)};
        return (cr_base_cthis >= cr_base_arg);
      }
    }

    template <std::size_t N>
      requires(N > 0)
    constexpr inline bool operator<(const int_reg_N_digs_t<N> &arg) const noexcept
    {
      if (is_minus() && arg.is_plus())
        return true;
      else if (is_plus() && arg.is_minus())
        return false;
      else
      {
        const base_t &cr_base_cthis{*static_cast<const base_t *const>(this)};
        const base_t &cr_base_arg{*static_cast<const base_t *const>(&arg)};
        // Para números negativos: mayor magnitud = menor número (invertir comparación)
        // Para números positivos: mayor magnitud = mayor número (comparación normal)
        if (is_minus())
          return (cr_base_cthis > cr_base_arg); // Invertido para negativos
        else
          return (cr_base_cthis < cr_base_arg); // Normal para positivos
      }
    }

    template <std::size_t N>
      requires(N > 0)
    constexpr inline bool operator<(const base_N_t<N> &arg) const noexcept
    {
      if (is_minus())
        return true;
      else
      {
        const base_t &cr_base_cthis{*static_cast<const base_t *const>(this)};
        const base_t &cr_base_arg{*static_cast<const base_t *const>(&arg)};
        return (cr_base_cthis < cr_base_arg);
      }
    }

    template <std::size_t N>
      requires(N > 0)
    constexpr inline bool operator>(const int_reg_N_digs_t<N> &arg) const noexcept
    {
      if (is_plus() && arg.is_minus())
        return true;
      else if (is_minus() && arg.is_plus())
        return false;
      else
      {
        const base_t &cr_base_cthis{*static_cast<const base_t *const>(this)};
        const base_t &cr_base_arg{*static_cast<const base_t *const>(&arg)};
        // Para números negativos: mayor magnitud = menor número (invertir comparación)
        // Para números positivos: mayor magnitud = mayor número (comparación normal)
        if (is_minus())
          return (cr_base_cthis < cr_base_arg); // Invertido para negativos
        else
          return (cr_base_cthis > cr_base_arg); // Normal para positivos
      }
    }

    template <std::size_t N>
      requires(N > 0)
    constexpr inline bool operator>(const base_N_t<N> &arg) const noexcept
    {
      if (is_minus())
        return false;
      else
      {
        const base_t &cr_base_cthis{*static_cast<const base_t *const>(this)};
        const base_t &cr_base_arg{*static_cast<const base_t *const>(&arg)};
        return (cr_base_cthis > cr_base_arg);
      }
    }

    /// CONSTRUCTOR COMPARACION OPERADOR SPACESHIP C++20

    template <std::size_t N>
      requires((N > 0) && (N < L))
    constexpr inline std::strong_ordering operator<=>(const int_reg_N_digs_t<N> &arg) const
        noexcept
    {
      if (is_plus() && arg.is_minus())
        return std::strong_ordering::greater;
      else if (is_minus() && arg.is_plus())
        return std::strong_ordering::less;
      else
      {
        const base_t &cr_base_cthis{*static_cast<const base_t *const>(this)};
        const base_t &cr_base_arg{*static_cast<const base_t *const>(&arg)};
        // Para números negativos: mayor magnitud = menor número (invertir comparación)
        // Para números positivos: mayor magnitud = mayor número (comparación normal)
        if (is_minus())
        {
          // Para negativos: invertir orden
          return (cr_base_arg <=> cr_base_cthis);
        }
        else
        {
          // Para positivos: orden normal
          return (cr_base_cthis <=> cr_base_arg);
        }
      }
    }

    template <std::size_t N>
      requires((N > 0) && (N < L))
    constexpr inline std::strong_ordering operator<=>(const base_N_t<N> &arg) const
        noexcept
    {
      const int_reg_digs_t &cr_base_cthis{*static_cast<const base_t *const>(this)};
      if (is_minus())
        return std::strong_ordering::less;
      else
      {
        return (cr_base_cthis() <=> arg);
      }
    }

    /********************************/
    /*							  */
    /* 		  PRIMER DIGITO	 	  */
    /*		  SEGUNDO DIGITO	  */
    /*							  */
    /********************************/

    template <std::size_t I>
      requires(I < L)
    inline constexpr const dig_t &get() const noexcept
    {
      const base_t &cr_base_cthis{*static_cast<const base_t *const>(this)};
      return (cr_base_cthis[I]);
    }

    template <std::size_t I>
      requires(I < L)
    inline constexpr void set(dig_t arg) noexcept
    {
      base_t &base_cthis{static_cast<base_t>(*this)};
      if constexpr (I == L - 1)
        base_cthis[I] = arg.is_0() ? dig_0() : dig_Bm1();
      else
        base_cthis[I] = arg;
      return;
    }

    template <std::size_t I>
      requires(I < L)
    inline constexpr void set(dig_t &&arg) noexcept
    {
      base_t &base_cthis{static_cast<base_t>(*this)};
      if constexpr (I == L - 1)
        base_cthis[I] = arg.is_0() ? dig_0() : dig_Bm1();
      else
        base_cthis[I] = std::move(arg);
      return;
    }

    inline constexpr void set(std::size_t index, dig_t arg) noexcept
    {
      base_t &base_cthis{static_cast<base_t>(*this)};
      if (index == L - 1)
        base_cthis[index] = arg.is_0() ? dig_0() : dig_Bm1();
      else
        base_cthis[index] = arg;
      return;
    }

    inline constexpr uint_t operator()(std::size_t idx) const noexcept
    {
      const base_t &cr_base_cthis{*static_cast<const base_t *const>(this)};
      return (cr_base_cthis(idx));
    }
    /// std::istream& operator>>
    /******************************/
    /*							*/
    /* OPERADORES ARITMETICOS		*/
    /*	POSTINCREMENTO ++(int)  */
    /*	PREINCREMENTO  ++()		*/
    /*	POSTDECREMENTO --(int)  */
    /*	PREDECREMENTO  --()		*/
    /*                            */
    /******************************/

    constexpr inline const int_reg_digs_t &operator++() noexcept
    {
      base_t &r_cthis(*static_cast<base_t *>(this));
      const bool is_Bm1_filled{r_cthis.is_filled_of_Bm1()};
      if (is_plus() && is_Bm1_filled)
      {
        r_cthis.set_0();
        set_minus();
      }
      else if (is_minus() && is_Bm1_filled)
      {
        r_cthis.set_0();
        set_plus(); /// SOBRA, FIN DE LECTURA HUMANA: SUPRIMIBLE
      }
      else
      {
        ++r_cthis;
      }
      return (*this);
    }

    constexpr inline int_reg_digs_t operator++(int) noexcept
    {
      int_reg_digs_t cp_cthis{*this};
      ++(*this);
      return cp_cthis;
    }

    constexpr inline const int_reg_digs_t &operator--() noexcept
    {
      base_t &r_cthis(*static_cast<base_t *>(this));
      const bool is_Bm1_filled{r_cthis.is_filled_of_Bm1()};
      const bool is_0_filled{r_cthis.is_0()};

      if (is_plus() && is_0_filled)
      {
        r_cthis.set_fill_Bm1();
        set_minus(); /// SOBRA, FIN DE LECTURA HUMANA: SUPRIMIBLE
      }
      else if (is_minus() && is_0_filled)
      {
        r_cthis.set_fill_Bm1();
        set_plus();
      }
      else
      {
        --r_cthis;
      }
      return (*this);
    }

    constexpr inline int_reg_digs_t operator--(int) noexcept
    {
      int_reg_digs_t cp_cthis{*this};
      --(*this);
      return cp_cthis;
    }

    /// CONSTEXPR DIG_T M_INCR() NOEXCEPT
    constexpr inline dig_t m_incr()
    {
      return m_incr(*this);
    }

    /// CONSTEXPR STD::TUPLE<INT_REG_DIGS_T,DIG_T> INCR() CONST NOEXCEPT
    constexpr inline std::tuple<int_reg_digs_t, dig_t> incr() const
    {
      int_reg_digs_t cp_this{*this};
      dig_t carry{m_incr(cp_this)};
      return std::make_tuple(cp_this, carry);
    }

    /// CONSTEXPR DIG_T DECR() NOEXCEPT
    constexpr inline dig_t m_decr()
    {
      return m_decr(*this);
    }

    /// CONSTEXPR STD::TUPLE<INT_REG_DIGS_T,DIG_T> M_DECR() CONST NOEXCEPT
    constexpr inline std::tuple<int_reg_digs_t, dig_t> decr() const
    {
      int_reg_digs_t cp_this{*this};
      dig_t borrow{m_decr(cp_this)};
      return std::make_tuple(cp_this, borrow);
    }

    /// EN LAS ANTERIORES EXPRESIONES DIG_T SIGNIFICA CARRY O BORROW

    /******************************/
    /*							*/
    /* OPERADORES ARITMETICOS		*/
    /*	 C_B()  C_Bm1()  		*/
    /*	mC_B() mC_Bm1()		    */
    /*	operator!() operator-() */
    /*                            */
    /******************************/

    constexpr inline const int_reg_digs_t &mC_Bm1() noexcept
    {
      base_t &r_base_cthis{*static_cast<base_t *>(this)};
      r_base_cthis.normalize_sign();
      return (r_base_cthis.mC_Bm1());
    }

    constexpr inline const int_reg_digs_t &mC_B() noexcept
    {
      base_t &r_base_cthis{*static_cast<base_t *>(this)};
      r_base_cthis.mC_B();
      if (is_plus())
        set_minus();
      else
        set_plus();
      return (*this);
    }

    constexpr inline int_reg_digs_t C_Bm1() const noexcept
    {
      int_reg_digs_t cpthis(*this);
      cpthis.mC_Bm1();
      return cpthis;
    }

    constexpr inline int_reg_digs_t C_B() const noexcept
    {
      int_reg_digs_t cpthis(*this);
      cpthis.mC_B();
      return cpthis;
    }

    constexpr inline int_reg_digs_t operator~() const noexcept
    {
      int_reg_digs_t cp_cthis(*this);
      cp_cthis.mC_Bm1();
      return cp_cthis;
    }

    constexpr inline int_reg_digs_t operator-() const noexcept
    {
      int_reg_digs_t cp_cthis(*this);
      cp_cthis.mC_B();
      return cp_cthis;
    }

    /***************************************/
    /* OPERADORES ARITMETICOS BASICOS	 	 */
    /*	  int_reg_digs_t  @  dig_t       */
    /*      int_reg_digs_t  @= dig_t       */
    /*      int_reg_digs_t  @  10B^n       */
    /*      int_reg_digs_t  @= 10B^n       */
    /***************************************/

    /***************************************/
    /*								   	 */
    /*  ARITMETICOS CON ASIGNACION		 */
    /*	int_reg_digs_t @= dig_t		     */
    /*                                     */
    /***************************************/

    constexpr inline const int_reg_digs_t &operator+=(dig_t arg) noexcept
    {
      int_reg_digs_t &cthis{*this};
      cthis += arg;
      cthis.normalize_sign();
      return cthis;
    }

    constexpr inline const int_reg_digs_t &operator-=(dig_t arg) noexcept
    {
      int_reg_digs_t &cthis(*this);
      int_reg_digs_t cp_arg{};
      cp_arg[0] = arg;
      cp_arg.mC_B();
      cthis += cp_arg;
      cthis.normalize_sign();
      return (cthis);
    }

    constexpr inline const int_reg_digs_t &operator*=(dig_t arg) const noexcept
    {
      base_t &base_cthis{*static_cast<base_t *>(this)};
      if (is_minus())
      {
        base_cthis.mC_B();
        base_cthis *= arg;
        base_cthis.mC_B();
      }
      else
      {
        base_cthis *= arg;
      }
      const_cast<int_reg_digs_t *>(this)->normalize_sign();
      return (*this);
    }

    constexpr inline const int_reg_digs_t &operator/=(dig_t arg) const noexcept
    {
      base_t &base_cthis{*static_cast<base_t *>(this)};
      if (is_minus())
      {
        base_cthis.mC_B();
        base_cthis /= arg;
        base_cthis.mC_B();
      }
      else
      {
        base_cthis /= arg;
      }
      return (*this);
    }

    constexpr inline const int_reg_digs_t &operator%=(dig_t arg) const noexcept
    {
      base_t &base_cthis{*static_cast<base_t *>(this)};
      if (is_minus())
      {
        base_cthis.mC_B();
        base_cthis %= arg;
        base_cthis.mC_B();
      }
      else
      {
        base_cthis %= arg;
      }
      return (*this);
    }

    /// CON DEVOLUCIÓN DE DÍGITO DE CARRY
    constexpr inline dig_t m_addition(dig_t arg) noexcept
    {
      return m_incr_by_digit(*this, arg);
    }

    /// CON DEVOLUCIÓN DE DÍGITO DE BORROW
    constexpr inline dig_t m_subtract(dig_t arg) noexcept
    {
      return m_decr_by_digit(*this, arg);
    }

    /// '@' = EUCLID_DIV -> (COC,REM,ERR_DIV_BY_0)
    constexpr inline std::tuple<int_reg_digs_t, int_reg_digs_t, bool>
    fediv(const int_reg_digs_t &rarg) const noexcept
    {
      base_t cplarg{*static_cast<base_t *>(this)};
      base_t cprarg{static_cast<const base_t &>(rarg)};
      const bool larg_neg{is_minus()};
      const bool rarg_neg{rarg.is_minus()};
      if (larg_neg)
      {
        cplarg.mC_B();
      }
      if (rarg_neg)
      {
        cprarg.mC_B();
      }
      auto ret{cplarg.fediv(cprarg)};
      if (larg_neg != rarg_neg)
      {
        std::get<0>(ret).mC_B();
        std::get<1>(ret).mC_B();
      }
      return ret;
    }

    /********************************/
    /*							  */
    /*    OPERADORES ARITMETICOS	  */
    /*    int_reg_digs_t @ dig_t    */
    /* 							  */
    /********************************/

    constexpr inline int_reg_digs_t operator+(dig_t arg) const noexcept
    {
      return (int_reg_digs_t{*this} += arg);
    }

    constexpr inline auto operator-(dig_t arg) const noexcept
    {
      return (int_reg_digs_t{*this} -= arg);
    }

    constexpr inline auto operator*(dig_t arg) const noexcept
    {
      return (int_reg_digs_t{*this} *= arg);
    }

    constexpr inline auto operator/(dig_t arg) const noexcept
    {
      return (int_reg_digs_t{*this} /= arg);
    }

    constexpr inline auto operator%(dig_t arg) const noexcept
    {
      return (int_reg_digs_t{*this} %= arg);
    }

    /// MEJORES OPCIONES PARA += -= *= /= %=
    /// BEGIN M_ASSITION M_SUBTRACT M_MULTIPLICATION
    /// M_FEDIV NO ES CONVENIENTE Y ES CAMBIADA POR FEDIV

    /// HEREDADAS DE NAT_REG_DIGS_T
    /// HEREDADAS TAL CUAL
    /// dig_t m_addition(const nat_reg_digs_t& rarg) noexcept
    /// dig_t m_subtract(const nat_reg_digs_t& rarg) noexcept

    constexpr inline dig_t m_multiplication(dig_t rarg) noexcept
    {
      if (is_minus())
      {
        mC_B();
        m_mult(*this, rarg);
        mC_B();
      }
      else
      {
        m_mult(*this, rarg);
      }
      return (*this);
    }

    /// BEGIN : OPERATORS >> >>= << <<=
    ///

    /// MULTIPLY BY THE BASE B(10) AND ASSIGN
    /// RETURN A REFERENCE TO SELF POINTER
    /// OVERRIDING THE INHERED OPERATOR<<=(STD::SIZE_T N)
    constexpr inline const int_reg_digs_t &operator<<=(std::size_t n) noexcept
    {
      const bool is_neg{is_minus()};
      base_t *base_this{static_cast<base_t *>(this)};
      if (is_neg)
      {
        mC_B();
      }
      base_this->base_t::operator<<=(n);
      if (is_neg)
      {
        mC_B();
      }
      return (*this);
    }

    /// MULTIPLY BY THE BASE B(10)
    /// RETURN A COPY OF CONTENT OF SELF
    /// OVERRIDING THE INHERED OPERATOR<<(STD::SIZE_T N)
    /// USING THE OVERRIDEN OPERATOR<<=
    constexpr inline int_reg_digs_t operator<<(std::size_t n) const noexcept
    {
      return (int_reg_digs_t{*this} <<= n);
    }

    /// DIVIDE BY THE BASE B(10) AND ASSIGN
    /// RETURN A REFERENCE TO SELF POINTER
    /// OVERRIDING THE INHERED OPERATOR>>=(STD::SIZE_T N)
    constexpr inline const int_reg_digs_t &operator>>=(std::size_t n) noexcept
    {
      const bool is_neg{is_minus()};
      base_t *base_this{static_cast<base_t *>(this)};
      if (is_neg)
      {
        mC_B();
      }
      base_this->base_t::operator>>=(n);
      if (is_neg)
      {
        mC_B();
      }
      return (*this);
    }

    /// DIVIDE BY THE BASE B (10)
    /// RETURN A COPY OF CONTENT OF SELF
    /// OVERRIDING THE INHERED OPERATOR>>(STD::SIZE_T N)
    /// USING THE OVERRIDEN OPERATOR>>=
    constexpr inline int_reg_digs_t operator>>(std::size_t n) const noexcept
    {
      return (int_reg_digs_t{*this} >>= n);
    }

    ///
    /// END   : OPERATORS >> >>= << <<=

    /// BEGIN : "OPERATORS" REM_B^n AND M_REM_B^n
    ///

    /// REMAINDER BY THE BASE B (10) WITH ASSIGN
    /// RETURN A REFERENCE BY POINTER OF SELF
    /// OVERRIDING THE INHERED M_REM_B(STD::SIZE_T N)
    constexpr inline const int_reg_digs_t &m_rem_B(std::size_t n) noexcept
    {
      const bool is_neg{is_minus()};
      base_t *base_this{static_cast<base_t *>(this)};

      if (is_neg)
      {
        mC_B();
      }
      base_this->base_t::m_rem_B(n);
      if (is_neg)
      {
        mC_B();
      }

      return (*this);
    }

    /// REMAINDER BY THE BASE B (10)
    /// RETURN A COPY OF CONTENT OF SELF
    /// OVERRIDING THE INHERED REM_B(STD::SIZE_T N)
    /// USING THE OVERRIDEN M_RE_B(STD::SIZE_T N)
    constexpr inline int_reg_digs_t rem_B(std::size_t n) const noexcept
    {
      return (int_reg_digs_t{*this}.m_rem_B(n));
    }

    ///
    /// END   : "OPERATORS" REM_B^n AND M_REM_B^n

    /// BEGIN : OPERATORS | & |= &=
    ///

    /// WE HAVE THESE OPERATORS / METHODS BY INHERENCE [CONSTEXPR INLINE]
    /// const int_reg_digs_t& operator|=(const int_reg_digs_t& arg) noexcept
    /// int_reg_digs_t operator|(const int_reg_digs_t &arg) noexcept
    /// const int_reg_digs_t & operator&=(const int_reg_digs_t &arg) noexcept
    /// int_reg_digs_t operator&(const int_reg_digs_t& arg) noexcept

    ///
    /// END   : OPERATORS | & |= &=

    /**************************************/
    /*									*/
    /* OPERADORES ARITMETICOS		  		*/
    /* int_reg_digs_t @ int_reg_digs_t	*/
    /*                            		*/
    /**************************************/

    /// INHERED
    /// const int_reg_digs_t& operator+=(const base_N_t<N> &arg) noexcept

    /// INHERED
    /// const int_reg_digs_t& operator+=(const int_reg_N_digs_t<N> &arg) noexcept

    /// INHERED
    /// const int_reg_digs_t& operator-=(const base_N_t<N> &arg) noexcept

    /// INHERED
    /// const int_reg_digs_t& operator-=(const int_reg_N_digs_t<N> &arg) noexcept

    /// INHERED
    /// int_reg_digs_t& operator*=(const base_N_t<N> &arg) noexcept

    /// INHERED
    /// const int_reg_digs_t& operator*=(const int_reg_N_digs_t<N> &arg) noexcept

    /// INHERED
    /// const int_reg_digs_t & operator /= (const int_reg_digs_t & arg) noexcept

    /// INHERED
    /// const int_reg_digs_t & operator %= (const int_reg_digs_t & arg) noexcept

    /// INHERED
    /// int_reg_digs_t operator+(const base_N_t<N> &arg) const noexcept

    /// INHERED
    /// int_reg_digs_t operator-(const base_N_t<N> &arg) const noexcept

    /// INHERED
    /// int_reg_digs_t operator*(const base_N_t<N> &arg) const noexcept

    /// INHERED
    /// int_reg_digs_t operator+(const int_reg_N_digs_t<N> &arg) const noexcept

    /// INHERED
    /// int_reg_digs_t operator-(const int_reg_N_digs_t<N> &arg) const noexcept

    /// INHERED
    /// int_reg_digs_t operator*(const int_reg_N_digs_t<N> &arg) const noexcept

    /// INHERED
    /// int_reg_digs_t operator/(const int_reg_digs_t &arg) const noexcept

    /// INHERED
    /// int_reg_digs_t operator%(const int_reg_digs_t &arg) const noexcept

    ///  dig_t   DISTINTAS REPRESENTACIONES
    /// "dig_t,dig_t,..,dig_t" L digitos int_reg_digs_t
    /// "dig_t,dig_t,...,dig_t,sep,dig_t,dig_t,...,dig_t" LE SEP LF
    /// pf_int_reg_digs_t  C_B "dig_t,dig_t,...,dig_t,sep,dig_t,dig_t,...,dig_t"
    /// LE SEP LF pf_int_reg_digs_t  MS     ***
    /// "dig_t,dig_t,...,dig_t,sep,dig_t,dig_t,...,dig_t" LE SEP LF
    /// pf_int_reg_digs_t  EXC_N  *** "signo,dig_t,dig_t,..,dig_t" L digitos
    /// int_reg_digs_t  C_B "signo,dig_t,dig_t,..,dig_t" L digitos int_reg_digs_t
    /// MS     *** "signo,dig_t,dig_t,..,dig_t" L digitos int_reg_digs_t  EXC_N
    /// *** "signo,dig_t,dig_t,...,dig_t,sep,dig_t,dig_t,...,dig_t" LE SEP LF
    /// pf_int_reg_digs_t  C_B
    /// "signo,dig_t,dig_t,...,dig_t,sep,dig_t,dig_t,...,dig_t" LE SEP LF
    /// pf_int_reg_digs_t  MS    ***
    /// "signo,dig_t,dig_t,...,dig_t,sep,dig_t,dig_t,...,dig_t" LE SEP LF
    /// pf_int_reg_digs_t  EXC_N *** "dig_t,dig_t,...dig_t" numero variable de
    /// digitos nat_num_t "signo,dig_t,dig_t,...dig_t" numero variable de digitos
    /// int_num_t C_B "dig_t,dig_t,...dig_t/dig_t,dig_t,...dig_t" numero variable
    /// de digitos en denominador y numerador *** "signo,dig_t,dig_t,...dig_t"
    /// numero variable de digitos int_num_t C_B "signo,dig_t,dig_t,...dig_t"
    /// numero variable de digitos int_num_t C_B
    ///                                             numero variable de digitos en
    ///                                             denominador y numerador ***
    /// EXTENSIONES P-ADICAS              ***
    /// EXTENSIONES FRACCIONES CONTINUAS  ***

    /***************************************/
    /*								   	 */
    /*  MÉTODOS MATEMÁTICOS ADICIONALES	 */
    /*								   	 */
    /***************************************/

    /// VALOR ABSOLUTO - RETORNA UNA COPIA SIN SIGNO (SIEMPRE POSITIVA)
    constexpr inline int_reg_digs_t abs() const noexcept
    {
      int_reg_digs_t result{*this};
      if (result.is_minus())
      {
        result.set_plus();
      }
      return result;
    }

    /// VALOR ABSOLUTO EN LUGAR (MODIFICA EL OBJETO ACTUAL)
    constexpr inline const int_reg_digs_t &abs_in_place() noexcept
    {
      if (is_minus())
      {
        set_plus();
      }
      return (*this);
    }

    /// SIGNO DEL NÚMERO (-1, 0, +1)
    constexpr inline int sign() const noexcept
    {
      base_t r_base_cthis{*static_cast<const base_t *>(this)};
      if (r_base_cthis.is_0())
      {
        return 0; // Cero
      }
      else if (is_minus())
      {
        return -1; // Negativo
      }
      else
      {
        return 1; // Positivo
      }
    }

    /// VERIFICAR SI ES CERO
    constexpr inline bool is_zero() const noexcept
    {
      base_t r_base_cthis{*static_cast<const base_t *>(this)};
      return r_base_cthis.is_0();
    }

    /// VERIFICAR SI ES POSITIVO (MAYOR QUE CERO)
    constexpr inline bool is_positive() const noexcept
    {
      return is_plus() && !is_zero();
    }

    /// VERIFICAR SI ES NEGATIVO (MENOR QUE CERO)
    constexpr inline bool is_negative() const noexcept
    {
      return is_minus() && !is_zero();
    }

    /// MÁXIMO VALOR REPRESENTABLE PARA ESTA CONFIGURACIÓN
    static consteval int_reg_digs_t max_value() noexcept
    {
      int_reg_digs_t result;
      // Llenar todos los dígitos con el valor máximo (B-1)
      for (std::size_t i = 0; i < R; ++i)
      {
        result[i] = dig_Bm1();
      }
      result.set_plus(); // Signo positivo
      return result;
    }

    /// MÍNIMO VALOR REPRESENTABLE PARA ESTA CONFIGURACIÓN
    static consteval int_reg_digs_t min_value() noexcept
    {
      int_reg_digs_t result;
      // Llenar todos los dígitos con el valor máximo (B-1)
      for (std::size_t i = 0; i < R; ++i)
      {
        result[i] = dig_Bm1();
      }
      result.set_minus(); // Signo negativo
      return result;
    }

    /// INTERCAMBIAR CONTENIDO CON OTRO OBJETO
    constexpr inline void swap(int_reg_digs_t &other) noexcept
    {
      base_t &this_base = *static_cast<base_t *>(this);
      base_t &other_base = *static_cast<base_t *>(&other);

      // Intercambiar todos los dígitos incluido el signo
      for (std::size_t i = 0; i <= R; ++i)
      {
        auto temp = this_base[i];
        this_base[i] = other_base[i];
        other_base[i] = temp;
      }
    }

    /***************************************/
    /*								   	 */
    /*  MÉTODOS DE CONVERSIÓN ADICIONALES  */
    /*								   	 */
    /***************************************/

    /// CONVERSIÓN A STRING CON FORMATO PERSONALIZABLE
    std::string to_string_formatted(const std::string &separator = ",",
                                    bool show_sign = true,
                                    bool show_base_info = false) const noexcept
    {
      std::stringstream sstr_os{};

      if (show_sign)
      {
        sstr_os << (is_minus() ? "-" : "+");
      }
      else if (is_minus())
      {
        sstr_os << "-";
      }

      // Mostrar dígitos desde el más significativo
      bool first = true;
      for (int i = static_cast<int>(R) - 1; i >= 0; --i)
      {
        if (!first && !separator.empty())
        {
          sstr_os << separator;
        }
        sstr_os << (*this)[i].get();
        first = false;
      }

      if (show_base_info)
      {
        sstr_os << "_B" << B;
      }

      return sstr_os.str();
    }

    std::string to_string() const noexcept
    {
      std::stringstream sstr_os{};
      auto cparg{*this};
      sstr_os << "int_reg_dig#";
      sstr_os << (is_minus() ? "-" : "+") << "#";
      cparg = is_minus() ? cparg.mC_B() : cparg;
      for (std::int64_t ix{L - 1}; ix > 0; --ix)
      {
        sstr_os << static_cast<SIG_UINT_T>(cparg(ix)) << ':';
      }
      sstr_os << static_cast<SIG_UINT_T>(cparg(0));
      sstr_os << "#B";
      sstr_os << static_cast<SIG_UINT_T>(B);
      return sstr_os.str();
    }

    /***************************************/
    /*								   	 */
    /*  FUNCIONES FRIEND ADICIONALES      */
    /*								   	 */
    /***************************************/

    /// FUNCIÓN abs() GLOBAL PARA COMPATIBILIDAD CON STD
    friend constexpr int_reg_digs_t abs(const int_reg_digs_t &x) noexcept
    {
      return x.abs();
    }

    /// FUNCIÓN swap() GLOBAL PARA COMPATIBILIDAD CON STD
    friend constexpr void swap(int_reg_digs_t &a, int_reg_digs_t &b) noexcept
    {
      a.swap(b);
    }

    /// FUNCIÓN min() PARA DOS NÚMEROS
    friend constexpr const int_reg_digs_t &min(const int_reg_digs_t &a,
                                               const int_reg_digs_t &b) noexcept
    {
      return (a < b) ? a : b;
    }

    /// FUNCIÓN max() PARA DOS NÚMEROS
    friend constexpr const int_reg_digs_t &max(const int_reg_digs_t &a,
                                               const int_reg_digs_t &b) noexcept
    {
      return (a > b) ? a : b;
    }

    /// FUNCIÓN clamp() - LIMITAR VALOR ENTRE MIN Y MAX
    friend constexpr int_reg_digs_t clamp(const int_reg_digs_t &value,
                                          const int_reg_digs_t &min_val,
                                          const int_reg_digs_t &max_val) noexcept
    {
      return max(min_val, min(value, max_val));
    }

    /// FUNCIÓN sign() GLOBAL
    friend constexpr int sign(const int_reg_digs_t &x) noexcept
    {
      return x.sign();
    }

    // template<uint64_t Base,size_t LongSinSigno>
    // friend std::istream& operator>>(std::istream&,int_reg_digs_t<Base,LongSinSigno>&);
  };

  /****************************/
  /*							*/
  /* 	   ISTREAM Y OSTREAM	*/
  /*							*/
  /****************************/

  /// ENTRADA DIRECTAMENTE EN COMPLEMENTO A LA BASE
  template <std::uint64_t Base, std::size_t Length>
  std::istream &operator>>(std::istream &is, int_reg_digs_t<Base, Length> &arg)
  {
    enum estado_e
    {
      e0ini,
      em3ini,
      em2ini,
      em1ini,
      em0ini,
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
    using Int_Type = dig_t::UINT_T;
    using inttype = typename type_traits::sig_UInt_for_UInt_t<Int_Type>;
    using int_reg_digs_t = int_reg_digs_t<Base, Length>;
    /// INDICE QUE RECORRE EL STRING RECOGIDO DE ENTRADA
    std::size_t indice{0};
    /// VARIABLES PARA ACCIONES EN LOS ESTADOS
    Int_Type digito{0};
    std::size_t longitud{0};
    inttype numero_base_recogido{0};
    /// VARIABLE DE RETORNO (BINDED TO THE ARG BY REFERENCE)
    int_reg_digs_t &numero_ret{arg};
    /// ESTADO ACTUAL
    estado_e est_act = e0ini;
    /// CARACTER QUE GUARDARA EL INDICADO POR EL INDICE DEL STRING RECOGIDO DESDE
    /// CIN
    char c{type_traits::nullchar<char>};
    /// RECOGEMOS DESDE LA ENTRADA EL STRING CON LA INFORMACION CODIFICADA
    is >> sds;
    /// MAQUINA DE ESTADOS FINITOS QUE HACE EL PARSE() DE LA ENTRADA
    do
    {
      c = sds[indice];
      switch (est_act)
      {

      case em3ini:
      {
        if (c == 'i')
        {
          est_act = em2ini;
        }
        else
        {
          est_act = em3ini;
        }
      }
      break;
      case em2ini:
      {
        if (c == 'n')
        {
          est_act = em1ini;
        }
        else
        {
          est_act = em3ini;
        }
      }
      break;
      case em1ini:
      {
        if (c == 't')
        {
          est_act = em0ini;
        }
        else
        {
          est_act = em3ini;
        }
      }
      break;
      case em0ini:
      {
        if (c == '_')
        {
          est_act = e1r;
        }
        else
        {
          est_act = em3ini;
        }
      }
      break;
      case e0ini:
      {
        if (c == 'r')
        {
          est_act = e1r;
        }
        else
        {
          est_act = em3ini;
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
            est_act = em3ini;
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
          est_act = em3ini;
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
          est_act = em3ini;
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
          est_act = em3ini;
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
          est_act = em3ini;
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
          est_act = em3ini;
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
          est_act = em3ini;
        }
      }
      break;
      case e2start:
      {
        if (((c >= '0') && (c <= '9')) &&
            (std::abs(c - '0') < Base))
        {
          est_act = e2dig;
          digito = (c - '0');
          longitud = 0;
          numero_base_recogido = 0;
          numero_ret = int_reg_digs_t::regd_0();
        }
        else
        {
          est_act = em3ini;
          digito = 0;
          longitud = 0;
          numero_base_recogido = 0;
          numero_ret = int_reg_digs_t::regd_0();
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
          est_act = em3ini;
          digito = 0;
          longitud = 0;
          numero_base_recogido = 0;
          numero_ret = int_reg_digs_t::regd_0();
        }
      }
      break;
      case e2dp:
      {
        const bool dig_c{(c >= '0') && (c <= '9')};
        const bool c_es_dig_B{(std::abs(c - '0') < Base) &&
                              (digito < Base)};
        const bool dig_B_cdl{dig_c && c_es_dig_B &&
                             (longitud < Length - 1)};
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
          est_act = em3ini;
          digito = 0;
          longitud = 0;
          numero_base_recogido = 0;
          indice = 0;
          numero_ret = int_reg_digs_t::regd_0();
        }
      }
      break;
      case e2end:
      {
        const bool c_es_sepfin{c == 'B'};
        const bool d_es_digB{digito < Base};
        const bool l_coincide{longitud ==
                              Length - 1}; // llega al cero ?
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
          est_act = em3ini;
          indice = 0;
          digito = 0;
          longitud = 0;
          numero_ret = int_reg_digs_t::regd_0();
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
          est_act = em3ini;
          digito = 0;
          longitud = 0;
          numero_base_recogido = 0;
          numero_ret = int_reg_digs_t::regd_0();
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
          est_act = em3ini;
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

  template <std::uint64_t Base, std::size_t Long>
  std::ostream &operator<<(
      std::ostream &os, const int_reg_digs_t<Base, Long> &arg)
  {
    os << arg.to_string();
    return (os);
  }

  /***************************************/
  /*								   	 */
  /*  FUNCIONES UTILITARIAS GLOBALES     */
  /*								   	 */
  /***************************************/

  /// FUNCIÓN GCD (MÁXIMO COMÚN DIVISOR) USANDO ALGORITMO DE EUCLIDES
  template <std::uint64_t Base, std::size_t Length>
  constexpr int_reg_digs_t<Base, Length> gcd(int_reg_digs_t<Base, Length> a,
                                             int_reg_digs_t<Base, Length> b)
  {
    // Trabajar con valores absolutos
    a = a.abs();
    b = b.abs();

    // Algoritmo de Euclides
    while (!b.is_zero())
    {
      auto temp = b;
      b = a % b;
      a = temp;
    }
    return a;
  }

  /// FUNCIÓN LCM (MÍNIMO COMÚN MÚLTIPLO)
  template <std::uint64_t Base, std::size_t Length>
  constexpr int_reg_digs_t<Base, Length> lcm(const int_reg_digs_t<Base, Length> &a,
                                             const int_reg_digs_t<Base, Length> &b)
  {
    if (a.is_zero() || b.is_zero())
    {
      return int_reg_digs_t<Base, Length>{}; // LCM(0, x) = 0
    }

    auto abs_a = a.abs();
    auto abs_b = b.abs();
    return (abs_a * abs_b) / gcd(abs_a, abs_b);
  }

  /// FUNCIÓN POWER (POTENCIACIÓN ENTERA)
  template <std::uint64_t Base, std::size_t Length>
  constexpr int_reg_digs_t<Base, Length> power(const int_reg_digs_t<Base, Length> &base,
                                               std::size_t exponent)
  {
    if (exponent == 0)
    {
      return int_reg_digs_t<Base, Length>::sregd_1();
    }

    if (exponent == 1)
    {
      return base;
    }

    // Exponenciación por cuadrados
    auto result = int_reg_digs_t<Base, Length>::sregd_1();
    auto current_base = base;

    while (exponent > 0)
    {
      if (exponent & 1)
      {
        result *= current_base;
      }
      current_base *= current_base;
      exponent >>= 1;
    }

    return result;
  }

  /// FUNCIÓN PARA CREAR DESDE STRING
  template <std::uint64_t Base, std::size_t Length>
  int_reg_digs_t<Base, Length> from_string(const std::string &str)
  {
    int_reg_digs_t<Base, Length> result;
    std::istringstream iss(str);
    iss >> result;
    return result;
  }

  /// FUNCIÓN PARA VERIFICAR SI ES POTENCIA DE LA BASE
  template <std::uint64_t Base, std::size_t Length>
  constexpr bool is_power_of_base(const int_reg_digs_t<Base, Length> &num)
  {
    if (num.is_zero() || num.is_negative())
    {
      return false;
    }

    // Verificar si exactamente un dígito es 1 y el resto son 0
    bool found_one = false;
    for (std::size_t i = 0; i < Length; ++i)
    {
      auto digit_val = num[i].get();
      if (digit_val == 1)
      {
        if (found_one)
          return false; // Más de un 1
        found_one = true;
      }
      else if (digit_val != 0)
      {
        return false; // Dígito diferente de 0 o 1
      }
    }
    return found_one;
  }

} // END OF NAMESPACE NUMREPR
#endif
