#ifndef DIG_STRING_T_HPP_INCLUDED
#define DIG_STRING_T_HPP_INCLUDED

#include <string>
#include "int_reg_digs_t.hpp"

namespace NumRepr {

template <std::uint64_t B>
  requires(B > 1)
struct dig_string_t : public basic_string<dig_t<B>> {
public:

  using dig_t = dig_t<B>;
  using UINT_T = typename dig_t::UINT_T;
  using base_t = basic_string<dig_t>;
  using str_iterator = typename base_t::iterator;
  using c_str_iterator = typename base_t::const_iterator;
  using r_str_iterator = typename base_t::reverse_iterator;
  using cr_str_iterator = typename base_t::const_reverse_iterator;

public:
  inline constexpr std::size_t size() const noexcept {
    return (static_cast<const base_t *const>(this)->size());
  }

  inline constexpr void resize(std::size_t arg) noexcept {
    (static_cast<base_t *const>(this)->resize());
  }

  inline constexpr dig_t operator[](std::size_t arg) const noexcept {
    return (static_cast<const base_t *const>(this)->operator[](arg));
  }

  inline constexpr dig_t &operator[](std::size_t arg) noexcept {
    return (static_cast<base_t *const>(this)->operator[](arg));
  }

  inline constexpr void clear() noexcept {
    static_cast<base_t *const>(this)->clear();
  }

  inline constexpr void push_back(dig_t arg) noexcept {
    static_cast<base_t *const>(this)->push_back(arg);
  }

  inline constexpr void insert(size_t pos, size_t tam, dig_t parg) noexcept {
    static_cast<base_t *const>(this)->insert(pos, tam, parg);
  }

  inline constexpr num_basic_t substr(size_t pos, size_t n) const noexcept {
    static_cast<const base_t *const>(this)->substr(pos, n);
    return (*this);
  }

  inline constexpr void push_front(dig_t parg) noexcept {
    static_cast<const base_t *const>(this)->insert(0, 1, parg);
  }

  inline constexpr str_iterator begin() noexcept {
    return static_cast<base_t *const>(this)->begin();
  }

  inline constexpr c_str_iterator end() const noexcept {
    return static_cast<const base_t *const>(this)->end();
  }

  inline constexpr r_str_iterator rbegin() noexcept {
    return static_cast<base_t *const>(this)->rbegin();
  }

  inline constexpr cr_str_iterator rend() const noexcept {
    return static_cast<const base_t *const>(this)->rbegin();
  }

  inline constexpr void erase(size_t pos, size_t tam) noexcept {
    return static_cast<base_t *const>(this)->erase(pos, tam);
  }

  // OPERADOR DE CONCATENACION
  inline constexpr const dig_string_t &
  operator&=(const dig_string_t &arg) noexcept {
    (*(static_cast<base_t *const>(this))) += arg;
    return (*this);
  }
};

template <std::uint64_t B>
inline constexpr ullint pot_max_base_B_en_Int_T() noexcept {
  using Int_T = typename dig_t<B>::UINT_T;
  constexpr long double max =
      static_cast<long double>(type_traits::maxbase<Int_T>());
  constexpr long double max_unit =
      static_cast<long double>(numeric_limits<char>::max());
  constexpr long double nb_unit =
      static_cast<long double>(1 + (log(max_unit) / log(2)));
  constexpr long double n_units_Int_T = static_cast<long double>(sizeof(Int_T));
  constexpr long double nb_Int_T = nb_unit * n_units_Int_T;
  constexpr long double log_2_b_B = std::log(static_cast<long double>(2)) /
                                    std::log(static_cast<long double>(B));
  constexpr long double ld_expB = (std::floor(nb_Int_T * log_2_b_B));
  // long double ld_potB = (floor(pow(static_cast<long double>(B),ld_expB)));
  constexpr ullint ret = static_cast<ullint>(ld_expB);
  return ret;
}

template <std::uint64_t B>
inline constexpr ullint num_digs_max_base_B_en_Int_T() noexcept {
  using Int_T = typename dig_t<B>::UINT_T;
  constexpr long double max =
      static_cast<long double>(numeric_limits<Int_T>::max() + 1);
  constexpr long double max_unit =
      2 * static_cast<long double>(numeric_limits<char>::max() + 1);
  constexpr long double nb_unit =
      static_cast<long double>((log(max_unit) / log(2)));
  constexpr long double n_units_Int_T = static_cast<long double>(sizeof(Int_T));
  constexpr long double nb_Int_T = nb_unit * n_units_Int_T;
  constexpr long double log_2_b_B =
      log(static_cast<long double>(2)) / log(static_cast<long double>(B));
  long double ld_expB = (floor(nb_Int_T * log_2_b_B));
  long double ld_potB = (floor(pow(static_cast<long double>(B), ld_expB)));
  // bool B_es_pot_de_2 = false;
  ullint pot_2 = 2;
  for (ullint b = 1; b < max_unit; b++) {
    if (B == pot_2) {
      ld_expB += static_cast<long double>(1);
      // B_es_pot_de_2 = true;
      break;
    }
    pot_2 *= 2;
  }
  constexpr long double max_pot_B_Int_T =
      static_cast<long double>(pot_max_base_B_en_Int_T<Int_T, B>());
  long double multiplo = 1;
  ullint ind = 0;
  while ((multiplo < max) and (ind < B)) {
    multiplo = floor(multiplo + max_pot_B_Int_T);
    if (multiplo > max) {
      multiplo = floor(multiplo - max_pot_B_Int_T);
      ullint ret = static_cast<ullint>(multiplo - 1.0);
      return ret;
    }
    ++ind;
  }
  constexpr ullint ret = ullint(ld_expB);
  return ret;
}

template <std::uint64_t B>
inline constexpr ullint max_base_B_en_Int_T() noexcept {
  using Int_T = typename dig_t<B>::UINT_T;
  constexpr long double max =
      static_cast<long double>(numeric_limits<Int_T>::max());
  constexpr long double n_digs_base_B =
      static_cast<long double>(num_digs_max_base_B_en_Int_T<Int_T, B>());
  constexpr long double dos = static_cast<long double>(2);
  constexpr long double base = static_cast<long double>(B);
  constexpr ullint ret =
      static_cast<ullint>(ceil((n_digs_base_B * log(dos)) / log(base)));
  return ret;
}
} // namespace NumRepr
#endif //  DIG_STRING_T__HPP__INCLUDED
