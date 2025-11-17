#ifndef REGISTER_VARIANT_T_HPP_INCLUDED
#define REGISTER_VARIANT_T_HPP_INCLUDED

#include <variant>
#include "core/internal/auxiliary_functions.hpp"
#include "digit_variant_t.hpp"
#include "reg_digs_t.hpp"

namespace NumRepr {

using ullint_t = unsigned long long int;
using ulint_t = unsigned long int;

static constexpr ulint_t lvar_sz =  8ul;
static constexpr ulint_t bvar_sz = 64ul;

template <template <std::uint64_t I,std::uint64_t J> class TC, class>
struct TC2p_variant;

template <template <std::uint64_t I,std::uint64_t J> class TC2p, std::uint64_t... Ks>
struct TC2p_variant<TC2p, std::index_sequence<Ks...>> {
  using type = typename std::variant<TC2p<(Ks/lvar_sz)+2ull, auxiliary_functions::int_pow2_ct<Ks%lvar_sz>()>...>;
};

template <template <std::uint64_t I,std::uint64_t J> class TC2p, class TSeq>
using TC2p_variant_t = typename TC2p_variant<TC2p, TSeq>::type;

using register_variant = TC2p_variant_t<reg_digs_t, std::make_index_sequence<bvar_sz*lvar_sz>>;

struct register_variant_t;
///                                      base          longitud      valor
register_variant_t make_register_variant(std::uint64_t,std::uint64_t,std::int64_t);
///                                      base          longitud      valor

struct register_variant_t : public register_variant {

  static constexpr std::uint64_t max_base = bvar_sz;
  static constexpr std::uint64_t max_long = lvar_sz;

  using UINT_T = reg_digs_t<255ull,128ull>::UINT_T;
  using SIG_UINT_T = reg_digs_t<255ull,128ull>::SIG_UINT_T;
  using SIG_SINT_T = reg_digs_t<255ull,128ull>::SIG_SINT_T;

  constexpr UINT_T gbase() const noexcept;
  constexpr UINT_T glong() const noexcept;
  constexpr digit_variant dig_0(std::uint64_t)   const noexcept;
  constexpr digit_variant dig_1(std::uint64_t)   const noexcept;
  constexpr digit_variant dig_Bm1(std::uint64_t) const noexcept;
  constexpr digit_variant dig_Bm2(std::uint64_t) const noexcept;
  constexpr digit_variant dig_0()    const noexcept;
  constexpr digit_variant dig_1()    const noexcept;
  constexpr digit_variant dig_Bm1()  const noexcept;
  constexpr digit_variant dig_Bm2()  const noexcept;


  /// <summary>
  /// IDEM QUE ANTES PERO CON LA CLASE DERIVADA, LA ACTUAL
  /// TENEMOS CUIDADO DE CREAR UN RVALUE TEMPORAL
  /// </summary>

  constexpr register_variant_t regd_0() const noexcept;
  constexpr register_variant_t regd_1() const noexcept;
  constexpr register_variant_t regd_Bm1() const noexcept;
  constexpr register_variant_t regd_B() const noexcept;
  constexpr register_variant_t regd_Bp1() const noexcept;
  template<std::uint64_t>
  constexpr register_variant_t regd_pow_n_B() const noexcept;
  template<std::uint64_t>
  constexpr register_variant_t regd_pow_n_B_m1() const noexcept;

  /// <summary>
  /// En heredamos como clase derivada de base_t == std::array<B,L>
  /// Tenemos todos los métodos de la STL
  /// </summary>

  constexpr decltype(auto) begin()   		      noexcept;
  constexpr decltype(auto) cbegin()  		const noexcept;
  constexpr decltype(auto) end()     		      noexcept;
  constexpr decltype(auto) cend()    		const noexcept;
  constexpr decltype(auto) rbegin()  		      noexcept;
  constexpr decltype(auto) crbegin() 		const noexcept;
  constexpr decltype(auto) rend()    		      noexcept;
  constexpr decltype(auto) crend()   		const noexcept;

  constexpr size_t size()            		const noexcept;
  constexpr size_t max_size()        		const noexcept;
  constexpr bool empty()             		const noexcept;
  constexpr digit_variant& front()    		      noexcept;
  constexpr digit_variant const& front()  	const noexcept;
  constexpr digit_variant& back()          	      noexcept;
  constexpr digit_variant const& back()   	const noexcept;
  constexpr void fill(const digit_variant&)		  noexcept;
  constexpr void swap(register_variant_t&) 		  noexcept;

  constexpr void reverse() 		 				  noexcept;

  /// <summary>
  ///
  ///
  ///
  ///    Constructores de reg_digs_t
  ///
  ///
  ///
  /// </summary>


  /// <summary>
  /// Constructor por defecto (rellena de dig_t(0) todo el array)
  /// </summary>
  template<size_t B,size_t N>
  constexpr register_variant_t() noexcept : register_variant{reg_digs_t<B,N>{regd_base_0()}} {}

  /// <summary>
  /// Constructor por argumentos tipo dig_t: deduce el tipo
  /// </summary>
  template <size_t B,typename... Ts>
    requires(std::is_same_v<Ts, dig_t<B>>&&  ...)
  constexpr register_variant_t(const Ts& ...args) noexcept
      : register_variant{typename reg_digs_t<B,sizeof...(Ts)>::base_t{(utilities::ugly_pack_details::pack2array<const Ts& ...>{}}(args...)}) {}

  /// CONSTRUCTOR COPIA DESDE EL TIPO BASE
  template<size_t B,size_t N>
  constexpr register_variant_t(const reg_digs_t<B,N>& rarg) noexcept : register_variant{typename reg_digs_t<B,N>::base_t{rarg}} {}

  /// CONSTRUCTOR MOVIMIENTO DESDE EL TIPO BASE
  template<size_t B,size_t N>
  constexpr register_variant_t(reg_digs_t<B,N>&& rarg) noexcept
      : register_variant(typename reg_digs_t<B,N>::base_t{std::move(rarg)}) {}

  /// <summary>
  /// Constructor por Copia/Movimiento desde una
  /// sucesión variadica de dígitos dig_t
  /// </summary>

  /// <summary>
  /// Constructor copia desde un array cualquiera de dígitos dig_t
  /// (usando copy_arg_N<N>)
  /// </summary>
  template <size_t B,size_t N>
  constexpr register_variant_t(const reg_digs_t<B,N>& arg) noexcept
      : register_variant{typename reg_digs_t<B,N>::base_t{reg_digs_t<B,N>::copy_arg_N<B,N>(arg)}} {}

  /// <summary>
  /// Constructor por movimiento desde un array cualquiera de dígitos dig_t
  /// (usando move_arg_N<N>)
  /// </summary>
  template <size_t B,size_t N>
  constexpr register_variant_t(reg_digs_t<B,N>&& arg) noexcept
      : register_variant{reg_digs_t<B,N>::base_t{std::move(reg_digs_t<B,N>::move_arg_N<B,N>(std::move(arg)))}} {}

  /// <summary="Constructor copia desde una sucesión de objetos enteros
  /// variádica, normalizándolos">
  /// </summary>

  template <size_t B,type_traits::integral_c... Ints_type>
    requires((sizeof...(Ints_type)) == L)
  constexpr register_variant_t(Ints_type... dig_pow_i) noexcept
      : register_variant{base_t((normalize<Ints_type...>((dig_t<B>(dig_pow_i))()...)).reverse())} {}

  /// <summary>
  /// Sobrecarga del operador copia
  /// </summary>

  /// OPERACION ASIGNACION POR COPIA REFERENCIA
  /// CONST _NO_ COPIABLE DESDE REG_N_DIGS_T EN LA IZQUIERDA
  template <std::size_t B,std::size_t N>
  constexpr const register_variant_t&
  operator=(const reg_digs_t<B,N>& arg) noexcept;

  /// OPERACION ASIGNACION POR COPIA REFERENCIA
  /// DESDE REG_N_DIGS_T EN LA IZQUIERDA
  template <std::size_t B,std::size_t N>
  constexpr register_variant_t& operator=(reg_digs_t<B,N>& arg) noexcept;

  /// OPERACION ASIGNACION POR MOVIMIENTO
  /// DESDE REGS_N_DIGS_T EN LA QUE NO SE PUEDE COPIAR
  template <size_t B,size_t N>
  constexpr const register_variant_t& operator=(reg_digs_t<B,N>&& arg) noexcept;

  /// OPERACION COPIA DESDE UN DIGITO (CONVERSION)
  constexpr register_variant_t& operator=(const dig_t& arg) noexcept;

  /// OPERACION COPIA DESDE UN ENTERO (CONVERSION A LA BASE B)
  /// A UN REG_DIGS_T
  template <type_traits::integral_c Int_Type>
  constexpr const register_variant_t& operator=(Int_Type arg) noexcept;

  ///
  ///       Algunas Conversiones
  ///

  /// PUEDE SER QUE NO QUEPA EN EL TIPO ENTERO
  template <type_traits::integral_c Int_Type>
  /// SE PODRÍA HACER PARA UINT128_T
  constexpr explicit operator Int_Type() const noexcept;

  ///< summary="Sobrecarga del const dig_t&  operator[](size_t)
  ///< const"></summary>
  /// HEREDADA DE LA CLASE STD::ARRAY DE LA STL
  /// constexpr const dig_t& operator[](size_t ix) const noexcept;

  ///< summary="Sobrecarga del dig_t&  operator[](size_t)"></summary>
  /// HEREDADA DE LA CLASE STD::ARRAY DE LA STL
  /// constexpr dig_t& operator[](size_t ix) noexcept;

  ///< summary="Sobrecarga del const dig_t&  operator[](size_t)
  ///< const"></summary>
  constexpr auto operator()(size_t ix) const noexcept;

  ///< summary>
  /// Funciones que ponen a constantes (constexpr) los objetos base_t
  ///</summary>
  ///< typeparam name="size_t"></typeparam>
  ///< nontypeparam name="N"></nontypeparam>
  ///< param name="this"></param>
  ///< returns name="void"></returns>

  constexpr void set_0()               		noexcept;

  constexpr void set_1()               		noexcept;

  constexpr void set_Bm1()             		noexcept;
  template<size_t B>
  constexpr void set_dig(dig_t<B> d)      	noexcept;
  template<size_t B>
  constexpr void set_fill_dig(dig_t<B> d) 	noexcept;

  constexpr void set_fill_1()          		noexcept;

  constexpr void set_fill_Bm1()        		noexcept;

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
  constexpr void set_interval_0() noexcept;

  /// OPERACION DE PONER A VALOR DIG_Bm1 DEL ARRAY
  ///	DESDE [N_0 , N_1) EN BASE_N_T<N>
  template <size_t N, size_t N_i, size_t N_pf> // i  es inicio
                                               // pf es pasado el final
    requires((N_i < N_pf) && (N_pf <= L))
  constexpr void set_interval_Bm1() noexcept;

  /// OPERACION DE PONER A VALOR DIG DEL ARRAY
  ///	DESDE [N_0 , N_1) EN BASE_T
  template <size_t N_i, size_t N_pf> // i  es inicio
                                     // pf es pasado el final
    requires((N_i < N_pf) && (N_pf <= L))
  constexpr void set_interval_dig(dig_t dig) noexcept;

  ///< summary="Funciones comparativas con constantes tipo constexpr"></summary>
  ///< param name="this"></param>
  ///< returns="bool"></returns>

//  constexpr bool is_0() const noexcept;
//
//  constexpr bool is_1() const noexcept;
//
//  constexpr bool is_Bm1() const noexcept;
//
//  constexpr bool is_B() const noexcept;
//
//  constexpr bool is_Bp1() const noexcept;
//
//  template <size_t n>
//  constexpr bool is_B_pow_m1() const noexcept;
//
//  template <size_t n>
//  constexpr bool is_B_pow() const noexcept;
//
//  constexpr bool is_any_B_pow() const noexcept;
//
//  constexpr bool is_filled_of_1() const noexcept;
//
//  constexpr bool is_filled_of_Bm1() const noexcept;
//
//  constexpr bool is_filled_of(dig_t d) const noexcept;
//
//  ///		STATIC FUNCTIONS :  CONCATENATE REGISTERS AND DIGITS
//  /// 		reg_digs_t<1>		cat(dig_t)
//  /// 		reg_digs_t<1+1>		cat(dig_t,dig_t)
//  /// 		reg_digs_t			cat(reg_digs_t)
//  ///   <M> reg_N_digs_t<L+M>	cat(reg_digs_t,reg_N_digs_t<M>)
//  /// 		reg_N_digs_t<L+1>	cat(reg_digs_t,dig_t)
//  /// 		reg_N_digs_t<1+L>	cat(dig_t,reg_digs_t)
//  /// 		reg_N_digs_t<sizeof...(dig_pack)>
//  ///							cat(dig_t ... dig_pack) VARIADIC PACK
//
//  /// STATIC REG_DIGS_T CAT(REG_DIGS_T)
//  constexpr
//  register_variant_t cat(const register_variant_t& arg) const noexcept;
//
//  /// STATIC <M> REG_N_DIGS_T<L+M> CAT(REG_DIGS_T,REG_N_DIGS_T<M>)
//  constexpr register_variant_t
//  cat(const register_variant_t& larg, const register_variant_t& rarg) const noexcept;
//
//  /// STATIC REG_N_DIGS_T<1+1> CAT(DIG_T,DIG_T)
//  template<size_t B>
//  constexpr register_variant_t cat(dig_t<B> larg, dig_t<B> rarg) const noexcept;
//
//  /// STATIC REG_N_DIGS_T<SIZEOF...(DIG_PACK)>
//  /// CAT(DIG_T,DIG_T ... DIG_PACK) VARIADIC
//  template <size_t B, typename T0, typename... Ts>
//    requires(((std::is_same_v<Ts, dig_t<B>>)&& ...) && (std::is_same_v<T0, dig_t<B>>))
//  constexpr register_variant_t<1 + (sizeof...(Ts))> cat(
//      T0 dig0, Ts... dig_pack) noexcept {
//    return cat(dig_t<B>::dig_0, dig_pack...);
//  }
//
//  /// STATIC REG_N_DIGS_T<L+1+(SIZEOF...(DIG_PACK))>
//  /// CAT(REG_DIGS_T,DIG_T,DIG_T ... DIG_PACK) VARIADIC
//  template <size_t B,typename T, typename... Ts>
//    requires(((std::is_same_v<Ts, dig_t<B>>)&& ...) && (std::is_same_v<T, dig_t<B>>))
//  static constexpr
//  register_variant_t cat(
//      reg_digs_t larg, T dig, Ts... dig_pack) noexcept {
//    return cat(larg, cat(dig, dig_pack...));
//  }
//
//  /// STATIC REG_N_DIGS_T<L+1+(SIZEOF...(DIG_PACK))>
//  /// CAT(DIG_T,DIG_T ... DIG_PACK,REG_DIGS_T) VARIADIC
//  template <size_t B,typename T, typename... Ts>
//    requires(((std::is_same_v<Ts, dig_t<B>>)&& ...) && (std::is_same_v<T, dig_t<B>>))
//  static constexpr register_variant_t cat(
//      T dig, Ts... dig_pack, reg_digs_t rarg) noexcept {
//    return cat(cat(dig, dig_pack...), rarg);
//  }
//
//  /// STATIC REG_N_DIGS_T<SIZE_T N,SIZE_T ... N_PACK>
//  /// CAT(
//  ///           REG_N_DIGS_T<N> LARG,
//  ///			REG_N_DIGS_T<N_PACK> ... RARG_PACK
//  /// ) VARIADIC PACK
//  template <size_t N, size_t... N_pack>
//    requires((N > 0) && ((N_pack > 0)&&  ...))
//  static constexpr register_variant_t<N + (... + (N_pack))> cat(
//      register_variant_t<N> larg, register_variant_t<N_pack>... rarg_pack) noexcept {
//    return cat(larg, rarg_pack...);
//  }
//
//  /// TAKE A SUBREGISTER OF A REGISTER OF DIGITS
//  template <size_t ibegin, size_t iend>
//    requires((iend <= L) && (ibegin < L) && (ibegin != iend))
//  constexpr register_variant_t  subregister() const noexcept;
//
//  /// OPERADORES ARITMETICOS
//  ///	C_B()  C_Bm1()
//  ///	mC_B() mC_Bm1()
//  ///	operator~() operator-()
//
//  constexpr const register_variant_t& mC_Bm1() noexcept;
//
//  constexpr const register_variant_t& mC_B() noexcept;
//
//  constexpr register_variant_t C_Bm1() const noexcept;
//
//  constexpr register_variant_t C_B() const noexcept;
//
//  /// A partir de aquí no he hecho las sustituciones de
//  /// const reg_digs_t&  cthis{*this}; / reg_digs_t&  cthis{*this}; / reg_digs_t
//  /// cthis{*this}; Por las correspondientes funciones cr_cthis() / r_cthis() /
//  /// cp_cthis()
//
//  constexpr register_variant_t operator~() const noexcept;
//
//  constexpr register_variant_t operator-() const noexcept;
//
//public:
//  /// OPERATORS >> >>= << <<= REM_B^n M_REM_B^n MER_B_N M_MER_B_N
//
//  /// MULTIPLY BY THE BASE B (10) << <<=
//  /// DIVIDE BY THE BASE B (10)	  >> >>=
//  /// REMAINDER BY THE BASE B (10) REM_B_N M_REM_B_N
//  /// MULTIPLICATIVE CARRY BY THE BASE B (10) MER_B_N M_MER_B_N
//  constexpr register_variant_t operator<<(size_t n) const noexcept;
//
//  constexpr const register_variant_t& operator<<=(size_t n) noexcept;
//
//  constexpr const register_variant_t& operator>>=(size_t n) noexcept;
//
//  constexpr register_variant_t operator>>(size_t n) const noexcept;
//
//  constexpr register_variant_t rem_B(size_t n) const noexcept;
//
//  constexpr const register_variant_t& m_rem_B(size_t n) noexcept;
//
//  constexpr register_variant_t mer_B(size_t n) const noexcept;
//
//  constexpr const register_variant_t& m_mer_B(size_t n) noexcept;
//
//  /// OPERATOR BIT A BIT BITWISE | |= & &=
//
//  constexpr const register_variant_t& operator|=(const register_variant_t& rarg) const noexcept;
//
//  constexpr register_variant_t operator|(const register_variant_t& rarg) const noexcept;
//
//  constexpr const register_variant_t& operator&=(const register_variant_t& rarg) noexcept;
//
//  constexpr register_variant_t operator&(const register_variant_t& rarg) noexcept;
//
//  //  PRIVATE
//  /// NOS DEVUELVE EL ÍNDICE DEL DÍGITO NO 0 DE POTENCIA DE B MAS GRANDE
//  /// NOS DEVUELVE 0 SI ES UN DÍGITO NO 0
//  /// NOS DEVUELVE -1 SI ES EL DÍGITO 0
//  /// CUALQUIER OTRO CASO NOS DARÁ MAYOR QUE 0
//  constexpr std::int64_t index_of_MSDig() const noexcept;
//
//  ///
//  ///
//  /// SOBRECARGA DE LOS OPERADORES DE COMPARACION Y ORDEN
//  ///
//  ///
//
//  /// COMPARACIONES ENTRE BASE_T Y DIG_T EN FORMA BASE_T @ DIG_T
//  template<size_t B>
//  constexpr bool operator==(const dig_t<B>& rarg) const noexcept;
//  template<size_t B>
//  constexpr bool operator!=(const dig_t<B>& rarg) const noexcept;
//  template<size_t B>
//  constexpr bool operator>(const dig_t<B>& rarg) const noexcept;
//  template<size_t B>
//  constexpr bool operator<(const dig_t<B>& rarg) const noexcept;
//  template<size_t B>
//  constexpr bool operator>=(const dig_t<B>& rarg) const noexcept;
//  template<size_t B>
//  constexpr bool operator<=(const dig_t<B>& rarg) const noexcept;
//
//  /// COMPARACIONES ENTRE REG_DIGS_T Y REG_N_DIGS_T
//  ///	HETEROGENEOS EN GENERAL
//  ///	EN FORMA REG_DIGS_T @ REG_N_DIGS_T<M>
//  template <size_t B,size_t M>
//  constexpr bool operator==(const reg_digs_t<B,M>& rarg) const noexcept;
//
//  template <size_t B,size_t M>
//  constexpr bool operator!=(const reg_digs_t<B,M>& rarg) const noexcept;
//
//  template <size_t B,size_t M>
//  constexpr bool operator>(const reg_digs_t<B,M>& rarg) const noexcept;
//
//  template <size_t B,size_t M>
//  constexpr bool operator<(const reg_digs_t<B,M>& rarg) const noexcept;
//
//  template <size_t B,size_t M>
//  constexpr bool operator>=(const reg_digs_t<B.M>& rarg) const noexcept;
//
//  template <size_t B,size_t M>
//  constexpr bool operator<=(const reg_digs_t<B,M>& rarg) const noexcept;
//
//  /// OPERADOR COMPARACION SPACESHIP C++20
//
//  template <size_t B,size_t M>
//  constexpr std::strong_ordering
//  operator<=>(const reg_digs_t<B,M>& arg) const noexcept;
//
//  /// OPERADOR COMPARACION SPACESHIP C++20
//  template<size_t B>
//  constexpr std::weak_ordering
//  operator<=>(const dig_t<B>& arg) const noexcept;
//
//  std::string to_string() const noexcept;

}; /// END OF CLASS REGISTER_VARIANT

/************************************/
/*                                  */
/*         ISTREAM Y OSTREAM        */
/*                                  */
/************************************/

std::istream &operator>>(std::istream &,register_variant_t&);

std::ostream &operator<<(std::ostream &,const register_variant_t&);

} // END OF NAMESPACE NUMREPR IN THIS TRANSLATION UNIT



#endif // REGISTER_VARIANT_T_HPP_INCLUDED
