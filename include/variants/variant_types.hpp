#ifndef VARIANT_TYPES_HPP_INCLUDED
#define VARIANT_TYPES_HPP_INCLUDED

#include <variant>
#include <string>
#include <ostream>
#include "core/dig_t.hpp"

namespace NumRepr {
namespace memory_handler {// tipos variant por ahora
// genRadixInt32L_t;
// genRadixNat32L_t;
// genRadixDig_t;
//template<std::uint64_t B>
//using nat_reg_32digs_t = nat_reg_digs_t<B,32U>;
//
//template<std::uint64_t B>
//using int_reg_32digs_t = int_reg_digs_t<B,32U>;

//class int_var_list;
//
//class nat_var_list;

using false_type = typename std::false_type;
using true_type = typename std::true_type;

using genRadixDig_t =
	typename std::variant<
		false_type,true_type,dig_t<2>, dig_t<3>, dig_t<4>, dig_t<5>, dig_t<6>, dig_t<7>, dig_t<8>, dig_t<9>,
		dig_t<10>,dig_t<11>,dig_t<12>,dig_t<13>,dig_t<14>,dig_t<15>,dig_t<16>,dig_t<17>,dig_t<18>,dig_t<19>,
		dig_t<20>,dig_t<21>,dig_t<22>,dig_t<23>,dig_t<24>,dig_t<25>,dig_t<26>,dig_t<27>,dig_t<28>,dig_t<29>,
		dig_t<30>,dig_t<31>,dig_t<32>,dig_t<33>,dig_t<34>,dig_t<35>,dig_t<36>,dig_t<37>,dig_t<38>,dig_t<39>,
		dig_t<40>,dig_t<41>,dig_t<42>,dig_t<43>,dig_t<44>,dig_t<45>,dig_t<46>,dig_t<47>,dig_t<48>,dig_t<49>,
		dig_t<50>,dig_t<51>,dig_t<52>,dig_t<53>,dig_t<54>,dig_t<55>,dig_t<56>,dig_t<57>,dig_t<58>,dig_t<59>,
		dig_t<60>,dig_t<61>,dig_t<62>,dig_t<63>
	>;

struct varRadixDig_t : public genRadixDig_t {
private:
	template <type_traits::integral_c Int_t>
	constexpr static inline decltype(auto) normaliza(Int_t arg) noexcept;
public:
	varRadixDig_t() noexcept : genRadixDig_t(dig_t<2>(0)){}
	varRadixDig_t(const genRadixDig_t& obj) noexcept : genRadixDig_t(obj) {}
	varRadixDig_t(genRadixDig_t&& obj) noexcept : genRadixDig_t(std::move(obj)) {}
	template<std::size_t k>
	varRadixDig_t(const dig_t<k>& obj) noexcept : genRadixDig_t(obj) {}
	template<std::size_t k>
	varRadixDig_t(dig_t<k>&& obj) noexcept : genRadixDig_t(std::move(obj)) {}
	template<std::size_t k>
	varRadixDig_t& operator=(const dig_t<k>& otro) noexcept {(*this)=otro; return (*this);}
	template<std::size_t k>
	varRadixDig_t& operator=(dig_t<k>&& otro) noexcept {(*this)=std::move(otro); return (*this);}
	varRadixDig_t& operator=(const genRadixDig_t& otro) noexcept {(*this)=otro; return (*this);}
	varRadixDig_t& operator=(genRadixDig_t&& otro) noexcept {(*this)=std::move(otro); return (*this);}
	const varRadixDig_t& operator++() noexcept;
	const varRadixDig_t& operator--() noexcept;
	varRadixDig_t operator++(int) noexcept;
	varRadixDig_t operator--(int) noexcept;
	const varRadixDig_t& operator+=(varRadixDig_t right) noexcept;
	const varRadixDig_t& operator-=(varRadixDig_t right) noexcept;
	const varRadixDig_t& operator*=(varRadixDig_t right) noexcept;
	const varRadixDig_t& operator/=(varRadixDig_t right) noexcept;
	const varRadixDig_t& operator%=(varRadixDig_t right) noexcept;
	varRadixDig_t operator+(varRadixDig_t right) const noexcept;
	varRadixDig_t operator-(varRadixDig_t right) const noexcept;
	varRadixDig_t operator*(varRadixDig_t right) const noexcept;
	varRadixDig_t operator/(varRadixDig_t right) const noexcept;
	varRadixDig_t operator%(varRadixDig_t right) const noexcept;
	std::string to_string() const noexcept;
	void parse_from_string(const std::string&);
	friend std::ostream& operator<<(std::ostream&,const varRadixDig_t&);
	friend std::istream& operator>>(std::istream&,varRadixDig_t&);
}; /// END OF CLASS VARIANT TYPE VARRADIXDIG_T


} /// END OF NAMESPACE MEMORY_HANDLER
} /// END OF NAMESPACE NUMREPR

#endif // VARIANT_TYPES_HPP_INCLUDED
