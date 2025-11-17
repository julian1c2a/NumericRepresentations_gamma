#ifndef BASIC_TYPES_HPP_INCLUDED
#define BASIC_TYPES_HPP_INCLUDED

#include <cstdint>

#include <compare>

#include <type_traits>

#include <concepts>

#include <optional>
#include <expected>
#if __has_include(<expected>)
#  include <expected>
#else
#  include <variant>
#  include <utility>
namespace std {
  template <typename E>
  struct unexpected {
    E value_;
    explicit unexpected(E v) : value_(v) {}
    E error() const noexcept { return value_; }
  };

  template <typename T, typename E>
  class expected {
    std::variant<T, E> v_;
  public:
    expected(const T &t) : v_(t) {}
    expected(T &&t) : v_(std::move(t)) {}
    expected(std::unexpected<E> ue) : v_(ue.value_) {}
    bool has_value() const noexcept { return std::holds_alternative<T>(v_); }
    explicit operator bool() const noexcept { return has_value(); }
    T &value() { return std::get<T>(v_); }
    const T &value() const { return std::get<T>(v_); }
    E error() const { return std::get<E>(v_); }
  };

  template <typename E>
  std::unexpected<E> unexpected(E e) { return std::unexpected<E>(e); }
}
#endif

#include <cstring>
#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

#include <array>
#include <list>
#include <map>
#include <vector>
#include <tuple>

#include <climits>
#include <cmath>
#include <limits>
#include <numeric>

#include <functional>
#include <iterator>

/**
 * @file basic_types.hpp
 * @brief Tipos básicos y meta-funciones para el sistema de representaciones numéricas
 */
namespace NumRepr {
  using chint_t = char;
  using schint_t = signed char;
  using ssint_t = signed short int;
  using sint_t = signed int;
  using slint_t = signed long int;
  using sllint_t = signed long long int;
  using uchint_t = unsigned char;
  using usint_t = unsigned short int;
  using uint_t = unsigned int;
  using ulint_t = unsigned long int;
  using ullint_t = unsigned long long int;

  /* Compatibility aliases (old names without _t) */
  using schint = schint_t;
  using ssint = ssint_t;
  using sint = sint_t;
  using slint = slint_t;
  using sllint = sllint_t;
  using uchint = uchint_t;
  using usint = usint_t;
  using uint = uint_t;
  using ulint = ulint_t;
  using ullint = ullint_t;

#if defined(__GNUC__) && !defined(__clang__)
  using uint128_t = __uint128_t;
  using sint128_t = __int128_t;
#elif defined(__clang__)
#if __has_extension(int128)
  using uint128_t = __uint128_t;
  using sint128_t = __int128_t;
#else
  struct uint128_t { std::uint64_t low, high; };
  struct sint128_t { std::uint64_t low; std::int64_t high; };
#endif
#elif defined(_MSC_VER)
  struct uint128_t { std::uint64_t low, high; };
  struct sint128_t { std::uint64_t low; std::int64_t high; };
#else
  struct uint128_t { std::uint64_t low, high; };
  struct sint128_t { std::uint64_t low; std::int64_t high; };
#endif

  using size_t = std::size_t;
  using sint8_t = std::int8_t;
  using sint16_t = std::int16_t;
  using sint32_t = std::int32_t;
  using sint64_t = std::int64_t;
  using int8_t = std::int8_t;
  using int16_t = std::int16_t;
  using int32_t = std::int32_t;
  using int64_t = std::int64_t;
  using fast8_t = std::int_fast8_t;
  using fast16_t = std::int_fast16_t;
  using fast32_t = std::int_fast32_t;
  using fast64_t = std::int_fast64_t;
  using least8_t = std::int_least8_t;
  using least16_t = std::int_least16_t;
  using least32_t = std::int_least32_t;
  using least64_t = std::int_least64_t;
  using uint8_t = std::uint8_t;
  using uint16_t = std::uint16_t;
  using uint32_t = std::uint32_t;
  using uint64_t = std::uint64_t;
  using ufast8_t = std::uint_fast8_t;
  using ufast16_t = std::uint_fast16_t;
  using ufast32_t = std::uint_fast32_t;
  using ufast64_t = std::uint_fast64_t;
  using uleast8_t = std::uint_least8_t;
  using uleast16_t = std::uint_least16_t;
  using uleast32_t = std::uint_least32_t;
  using uleast64_t = std::uint_least64_t;
  using intmax_t = std::intmax_t;
  using uintmax_t = std::uintmax_t;

  /**
   * @brief Representación del signo de un número.
   * @details Enumeración actualmente utilizada en operaciones aritméticas y comparaciones.
   *          Valores: vzero (0), vminus (-1), vplus (+1).
   */
  enum class sign_funct_e : char {
    vzero = 0,
    vminus = -1,
    vplus = +1
  };

  /**
   * @brief Formatos de representación de dígitos.
   * @details [EXTENSIÓN FUTURA] Reservado para implementaciones futuras de diferentes
   *          sistemas de codificación de dígitos (BCD, exceso-3, Johnson, biquinario, etc.).
   *          Actualmente no utilizado en el proyecto.
   * @note Definido para extensibilidad futura del sistema de representaciones numéricas.
   */
  enum class dig_format_e : unsigned char {
    BINnat = 0,      ///< Binario natural
    BCDnat = 1,      ///< BCD natural (8421)
    BCDexc3 = 2,     ///< BCD exceso-3
    BCDaitk = 3,     ///< BCD Aiken (2421)
    johnson_n = 4,   ///< Código Johnson
    biquinario = 5   ///< Código biquinario
  };

  /**
   * @brief Clasificación de tipos numéricos.
   * @details [PARCIALMENTE IMPLEMENTADO] Define la jerarquía de tipos numéricos.
   *          Actualmente usado solo en tests de demostración (test_auxiliary_types.cpp).
   * @note natural=0: Números naturales (ya implementado: nat_reg_digs_t)
   * @note entero=1: Enteros con signo (ya implementado: int_reg_digs_t)
   * @note racional=2: Números racionales (pendiente)
   * @note real=3: Números reales (pendiente)
   */
  enum class num_type_e : unsigned char {
    natural = 0,   ///< Naturales (ℕ) - IMPLEMENTADO
    entero = 1,    ///< Enteros (ℤ) - IMPLEMENTADO
    racional = 2,  ///< Racionales (ℚ) - PENDIENTE
    real = 3       ///< Reales (ℝ) - PENDIENTE
  };

  /**
   * @brief Formatos de representación de números naturales.
   * @details [EXTENSIÓN FUTURA] Reservado para diferentes representaciones de naturales.
   *          Actualmente no utilizado. El sistema usa representación natural directa.
   * @note natural=false: Representación estándar
   * @note exc_n=true: Representación en exceso-n (para futuras implementaciones)
   */
  enum class nat_num_format_e : bool {
    natural = false,  ///< Representación natural estándar
    exc_n = true      ///< Representación en exceso-n (futuro)
  };

  /**
   * @brief Formatos de representación de números enteros.
   * @details [PARCIALMENTE IMPLEMENTADO] Define diferentes sistemas de representación
   *          de enteros con signo. Actualmente usado solo en tests de demostración.
   * @note int_CB: Complemento a la base (usado en int_reg_digs_t)
   * @note int_CBm1: Complemento a base-1 (pendiente)
   * @note int_MS: Magnitud y signo (pendiente)
   * @note int_EXC_n: Exceso-n (pendiente)
   */
  enum class int_num_format_e : unsigned char {
    int_CB = 0,      ///< Complemento a la base - IMPLEMENTADO
    int_CBm1 = 1,    ///< Complemento a base-1 - PENDIENTE
    int_MS = 2,      ///< Magnitud y signo - PENDIENTE
    int_EXC_n = 3    ///< Exceso-n - PENDIENTE
  };

  /**
   * @brief Formatos de representación de números racionales.
   * @details [EXTENSIÓN FUTURA] Reservado para futuras implementaciones de racionales.
   *          Actualmente no utilizado en el proyecto.
   * @note pair_num_den: Par numerador/denominador
   * @note fxd_pt: Punto fijo
   * @note flt_pt: Punto flotante
   */
  enum class rat_num_format_e : unsigned char {
    pair_num_den = 0,  ///< Par (numerador, denominador)
    fxd_pt = 1,        ///< Punto fijo
    flt_pt = 2         ///< Punto flotante
  };

  /**
   * @brief Tipos de operaciones binarias.
   * @details [PARCIALMENTE IMPLEMENTADO] Enumeración de operaciones aritméticas binarias.
   *          Usado principalmente en tests de demostración. Las operaciones están
   *          implementadas directamente como métodos de las clases numéricas.
   * @note add, sub, mult: Operaciones básicas (implementadas)
   * @note div, rem: División y resto (implementados)
   * @note fediv: División entera con redondeo hacia abajo (floor division)
   * @note other: Operaciones extendidas (futuro)
   */
  enum class binop_e : unsigned char {
    add = 0,     ///< Adición
    sub = 1,     ///< Sustracción
    mult = 2,    ///< Multiplicación
    div = 3,     ///< División
    rem = 4,     ///< Resto/módulo
    fediv = 5,   ///< Floor division (futuro)
    other = 6    ///< Otras operaciones (futuro)
  };

  /**
   * @brief Convierte sign_funct_e a int.
   * @param sign El signo a convertir.
   * @return -1 para vminus, 0 para vzero, +1 para vplus.
   */
  constexpr inline int to_int(sign_funct_e sign) noexcept { return static_cast<int>(sign); }
  
  /**
   * @brief Verifica si el signo es positivo.
   * @param sign El signo a verificar.
   * @return true si sign == vplus, false en caso contrario.
   */
  constexpr inline bool is_positive(sign_funct_e sign) noexcept { return sign == sign_funct_e::vplus; }
  
  /**
   * @brief Verifica si el signo es negativo.
   * @param sign El signo a verificar.
   * @return true si sign == vminus, false en caso contrario.
   */
  constexpr inline bool is_negative(sign_funct_e sign) noexcept { return sign == sign_funct_e::vminus; }
  
  /**
   * @brief Verifica si el signo es cero.
   * @param sign El signo a verificar.
   * @return true si sign == vzero, false en caso contrario.
   */
  constexpr inline bool is_zero(sign_funct_e sign) noexcept { return sign == sign_funct_e::vzero; }
  
  /**
   * @brief Calcula el signo opuesto.
   * @param sign El signo original.
   * @return vminus si sign era vplus, vplus si era vminus, vzero si era vzero.
   */
  constexpr inline sign_funct_e opposite_sign(sign_funct_e sign) noexcept { 
    return (sign == sign_funct_e::vplus) ? sign_funct_e::vminus : 
           (sign == sign_funct_e::vminus) ? sign_funct_e::vplus : 
           sign_funct_e::vzero; 
  }

  namespace type_traits {

    template <typename IntT>
    concept arith_integral_c =
        std::is_arithmetic_v<IntT> && std::is_integral_v<IntT>;

    template <typename NatIntT>
    concept arith_natural_c =
        std::is_arithmetic_v<NatIntT> && std::is_unsigned_v<NatIntT> &&
        std::is_integral_v<NatIntT>;

    template <typename CharT>
    concept char_type_c =
        std::is_same_v<CharT, char> || std::is_same_v<CharT, signed char> ||
        std::is_same_v<CharT, unsigned char> || std::is_same_v<CharT, wchar_t>;

    template <char_type_c CharT>
    constexpr inline CharT nullchar{CharT('\0')};

    /**
     * @brief Convierte string ASCII decimal a uint64_t sin validación.
     * @details Conversión rápida sin verificar overflow, dígitos inválidos o string vacío.
     *          Usa algoritmo i = i*10 + digit equivalente a i = (i << 3) + (i << 1) + digit.
     * 
     * @param text String C terminado en null con dígitos decimales '0'-'9'.
     * @return Valor convertido. Comportamiento indefinido si hay overflow o caracteres no dígitos.
     * 
     * @pre text != nullptr
     * @pre Todos los caracteres en [text, text+strlen(text)) son dígitos '0'-'9'
     * @pre El valor resultante <= ULLONG_MAX (18446744073709551615)
     * 
     * @warning Sin validación. Para entrada no confiable, usa atoull_checked.
     * @note Complejidad: O(n) donde n = strlen(text)
     * @note constexpr: Puede evaluarse en compile-time si text es constexpr
     * 
     * @code
     * constexpr auto val1 = atoull("12345");              // val1 == 12345
     * constexpr auto val2 = atoull("18446744073709551615"); // uint64_t max
     * // auto bad = atoull("99999999999999999999");       // OVERFLOW - comportamiento indefinido
     * // auto bad2 = atoull("123abc");                    // INVÁLIDO - comportamiento indefinido
     * @endcode
     */
    inline constexpr ullint_t atoull(const char *text) noexcept {
      ullint_t i = 0;
      while (*text) {
        i = (i << 3) + (i << 1) + static_cast<ullint_t>(*text - '0');
        ++text;
      }
      return i;
    }

    /**
     * @brief Convierte string_view ASCII decimal a uint64_t sin validación.
     * @details Igual que atoull(const char*) pero con string_view.
     * 
     * @param sv Vista de string con dígitos decimales '0'-'9'.
     * @return Valor convertido. Comportamiento indefinido si hay overflow o caracteres no dígitos.
     * 
     * @pre sv.data() != nullptr
     * @pre Todos los caracteres en sv son dígitos '0'-'9'
     * @pre El valor resultante <= ULLONG_MAX
     * 
     * @warning Sin validación. Para entrada no confiable, usa atoull_checked.
     */
    inline constexpr ullint_t atoull(std::string_view sv) noexcept {
      ullint_t i = 0;
      for (char c : sv) {
        i = (i << 3) + (i << 1) + static_cast<ullint_t>(c - '0');
      }
      return i;
    }

    /**
     * @brief Códigos de error para conversión string→uint64_t.
     */
    enum class atoull_err_t : int { 
      empty_str, ///< String vacío o nullptr
      no_digit,  ///< Carácter no numérico encontrado
      overflow,  ///< Valor excede ULLONG_MAX
      unknown    ///< Error desconocido (no usado actualmente)
    };

    /**
     * @brief Convierte string ASCII decimal a uint64_t CON validación completa.
     * @details Versión segura de atoull que detecta:
     *          - String vacío o nullptr
     *          - Caracteres no dígitos
     *          - Overflow (valor > ULLONG_MAX = 18446744073709551615)
     * 
     * @param text String C terminado en null.
     * @return std::expected con:
     *         - Éxito: valor convertido
     *         - Error: empty_str, no_digit, o overflow
     * 
     * @note Complejidad: O(n) donde n = strlen(text)
     * @note Verificación de overflow: i > (ULLONG_MAX - digit) / 10
     * @note Rango válido: [0, 18446744073709551615]
     * 
     * @code
     * auto r1 = atoull_checked("12345");
     * if (r1) std::cout << *r1;  // 12345
     * 
     * auto r2 = atoull_checked("99999999999999999999");  // overflow
     * assert(!r2 && r2.error() == atoull_err_t::overflow);
     * 
     * auto r3 = atoull_checked("123abc");  // no_digit
     * assert(!r3 && r3.error() == atoull_err_t::no_digit);
     * 
     * auto r4 = atoull_checked("");  // empty_str
     * assert(!r4 && r4.error() == atoull_err_t::empty_str);
     * @endcode
     */
    inline std::expected<ullint_t, atoull_err_t> atoull_checked(const char *text) noexcept {
      if (text == nullptr) return std::unexpected(atoull_err_t::empty_str);
      ullint_t i = 0;
      bool any = false;
      constexpr ullint_t maxv = std::numeric_limits<ullint_t>::max();
      while (*text) {
        char c = *text;
        if (c < '0' || c > '9') return std::unexpected(atoull_err_t::no_digit);
        unsigned digit = static_cast<unsigned>(c - '0');
        if (i > (maxv - digit) / 10) return std::unexpected(atoull_err_t::overflow);
        i = i * 10 + digit;
        any = true;
        ++text;
      }
      if (!any) return std::unexpected(atoull_err_t::empty_str);
      return std::expected<ullint_t, atoull_err_t>(i);
    }

    /**
     * @brief Convierte string_view ASCII decimal a uint64_t CON validación completa.
     * @details Sobrecarga de atoull_checked para string_view. Misma semántica que la versión const char*.
     * 
     * @param sv Vista de string con dígitos decimales.
     * @return std::expected con valor o error (empty_str, no_digit, overflow).
     * 
     * @see atoull_checked(const char*) para documentación detallada
     */
    inline 
    std::expected<ullint_t, atoull_err_t> atoull_checked(std::string_view sv) noexcept {
      if (sv.data() == nullptr || sv.size() == 0) 
            return std::unexpected(atoull_err_t::empty_str);
      ullint_t i = 0;
      bool any = false;
      constexpr ullint_t maxv = std::numeric_limits<ullint_t>::max();
      for (char c : sv) {
        if (c < '0' || c > '9') return std::unexpected(atoull_err_t::no_digit);
        unsigned digit = static_cast<unsigned>(c - '0');
        if (i > (maxv - digit) / 10) return std::unexpected(atoull_err_t::overflow);
        i = i * 10 + digit;
        any = true;
      }
      if (!any) return std::unexpected(atoull_err_t::empty_str);
      return std::expected<ullint_t, atoull_err_t>(i);
    }

    /**
     * @brief Parsea dígitos decimales desde el inicio de un string y retorna valor + cantidad consumida.
     * @details Lee dígitos consecutivos hasta encontrar no-dígito o fin de string.
     *          Útil para parseo de números embebidos en texto (ej: "123abc" → {123, 3}).
     * 
     * @param text String C terminado en null.
     * @return std::expected con:
     *         - Éxito: pair{valor, cantidad_de_caracteres_consumidos}
     *         - Error: no_digit (si empieza con no-dígito) u overflow
     * 
     * @note Diferencia con atoull_checked: Este se detiene en el primer no-dígito SIN error.
     * @note Si text empieza con no-dígito, retorna no_digit (no empty_str).
     * @note Complejidad: O(n) donde n = cantidad de dígitos consecutivos
     * 
     * @code
     * auto r1 = atoull_consume("123abc");
     * if (r1) {
     *   auto [val, consumed] = *r1;
     *   // val == 123, consumed == 3
     *   // Continuar parseando desde text + 3
     * }
     * 
     * auto r2 = atoull_consume("999");
     * // {999, 3}
     * 
     * auto r3 = atoull_consume("abc123");
     * // Error: no_digit (empieza con 'a')
     * @endcode
     */
    inline 
    std::expected<std::pair<ullint_t, size_t>, atoull_err_t> 
    atoull_consume(const char *text) noexcept {
      if (text == nullptr) return std::unexpected(atoull_err_t::empty_str);
      ullint_t i = 0;
      size_t idx = 0;
      constexpr ullint_t maxv = std::numeric_limits<ullint_t>::max();
      while (text[idx]) {
        char c = text[idx];
        if (c < '0' || c > '9') break;
        unsigned digit = static_cast<unsigned>(c - '0');
        if (i > (maxv - digit) / 10) return std::unexpected(atoull_err_t::overflow);
        i = i * 10 + digit;
        ++idx;
      }
      if (idx == 0) return std::unexpected(atoull_err_t::no_digit);
      return std::expected<std::pair<ullint_t, size_t>, atoull_err_t> (
          std::pair<ullint_t, size_t>{i, idx}
      );
    }

    /**
     * @brief Parsea dígitos decimales desde el inicio de un string_view.
     * @details Sobrecarga de atoull_consume para string_view. Misma semántica que const char*.
     * 
     * @param sv Vista de string a parsear.
     * @return std::expected con pair{valor, cantidad_consumida} o error.
     * 
     * @see atoull_consume(const char*) para documentación detallada
     */
    inline 
    std::expected<std::pair<ullint_t, size_t>, atoull_err_t> 
    atoull_consume(std::string_view sv) noexcept {
      if (sv.data() == nullptr || sv.size() == 0) 
            return std::unexpected(atoull_err_t::empty_str);
      ullint_t i = 0;
      size_t idx = 0;
      constexpr ullint_t maxv = std::numeric_limits<ullint_t>::max();
      while (idx < sv.size()) {
        char c = sv[idx];
        if (c < '0' || c > '9') break;
        unsigned digit = static_cast<unsigned>(c - '0');
        if (i > (maxv - digit) / 10) return std::unexpected(atoull_err_t::overflow);
        i = i * 10 + digit;
        ++idx;
      }
      if (idx == 0) return std::unexpected(atoull_err_t::no_digit);
      return std::expected<std::pair<ullint_t, size_t>, atoull_err_t>
        (
            std::pair<ullint_t, size_t>{i, idx}
        );
    }

    /**
     * @brief Convierte string literal a ullint_t en tiempo de compilación.
     * 
     * @details Esta función consteval actúa como un "literal de usuario" para convertir
     *          strings a ullint_t durante la compilación. Usa un array de caracteres
     *          como parámetro de template non-type (C++20).
     * 
     *          Implementa la misma lógica de overflow checking que atoull_checked():
     *          verifica i > (ULLONG_MAX - digit) / 10 antes de cada multiplicación.
     * 
     * @tparam STR Array de caracteres con el literal (automáticamente deducido)
     * 
     * @return ullint_t con el valor parseado
     * 
     * @throws Compilation error si:
     *         - String está vacío
     *         - Contiene caracteres no-dígito
     *         - Valor excede ULLONG_MAX
     * 
     * @note USAGE: RECOMMENDED - Para constantes numéricas grandes definidas como strings
     * @note Límite: ULLONG_MAX = 18,446,744,073,709,551,615
     * 
     * @code
     * constexpr auto max64 = atoull_ct<"18446744073709551615">(); // ULLONG_MAX
     * constexpr auto val = atoull_ct<"1234567890">();
     * // atoull_ct<"18446744073709551616">(); // ERROR: overflow en compilación
     * // atoull_ct<"12abc34">(); // ERROR: carácter inválido en compilación
     * @endcode
     * 
     * @see atoull_checked() para versión runtime con std::expected
     */
    template <const auto& STR>
    consteval ullint_t atoull_ct() {
      constexpr std::string_view sv{STR.data(), STR.size()};
      
      if (sv.empty()) {
        throw "atoull_ct: empty string";
      }
      
      ullint_t i = 0;
      constexpr ullint_t maxv = std::numeric_limits<ullint_t>::max();
      
      for (size_t idx = 0; idx < sv.size(); ++idx) {
        char c = sv[idx];
        
        if (c < '0' || c > '9') {
          throw "atoull_ct: non-digit character found";
        }
        
        unsigned digit = static_cast<unsigned>(c - '0');
        
        // Overflow check: i > (maxv - digit) / 10
        if (i > (maxv - digit) / 10) {
          throw "atoull_ct: overflow detected";
        }
        
        i = i * 10 + digit;
      }
      
      if (i == 0 && sv.size() > 0 && sv[0] != '0') {
        throw "atoull_ct: no digits parsed";
      }
      
      return i;
    }

    //================================================
    // TYPE TRAITS: Concepts and Type Deduction
    //================================================
    
    /**
     * @brief Concept para tipos unsigned válidos para radix (excluye tipos de 64 bits).
     * 
     * @details Este concept identifica tipos unsigned que pueden usarse como radix
     *          en representaciones numéricas. Excluye tipos de 64 bits o mayores
     *          para mantener compatibilidad con sistemas que no tienen 128 bits nativos.
     * 
     * @tparam UINT_T Tipo a verificar
     * 
     * @note USAGE: INTERNALLY USED - Para selección de tipos en templates
     * @note Satisface: uchint_t (8 bits), usint_t (16 bits), uint_t (32 bits)
     * @note Excluye: ulint_t en Linux (64 bits), ullint_t (64 bits), ulint64_t (64 bits)
     */
    template <typename UINT_T>
    constexpr 
    bool is_uint_type_for_radix_v =
             std::is_unsigned_v<UINT_T> && 
            (sizeof(UINT_T) < 8);

    template <typename UINT_T>
    concept uint_type_for_radix_c = is_uint_type_for_radix_v<UINT_T>;

    /**
     * @brief Concept para cualquier tipo unsigned (incluye uint64_t).
     * 
     * @details Extiende uint_type_for_radix_c para incluir también std::uint64_t.
     *          Usado para validaciones generales de tipos unsigned.
     * 
     * @tparam UINT_T Tipo a verificar
     * 
     * @note USAGE: ACTIVELY USED - Para constrains en funciones genéricas
     */
    template <typename UINT_T>
    constexpr bool is_unsigned_type_v =
        is_uint_type_for_radix_v<UINT_T> || std::is_same_v<UINT_T, std::uint64_t> || 
        std::is_same_v<UINT_T, ullint_t> || std::is_same_v<UINT_T, ulint_t>;

    template <typename UINT_T>
    concept unsigned_integral_c = is_unsigned_type_v<UINT_T>;

    /**
     * @brief Concept para tipos signed (incluye sint64_t custom).
     * 
     * @details Valida tipos signed estándar y el tipo custom sint64_t.
     * 
     * @tparam SINT_T Tipo a verificar
     * 
     * @note USAGE: ACTIVELY USED - Para operaciones con enteros con signo
     */
    template <typename SINT_T>
    constexpr bool is_signed_type_v =
        std::is_signed_v<SINT_T> || std::is_same_v<SINT_T, sint64_t>;

    template <typename SINT_T>
    concept signed_integral_c = is_signed_type_v<SINT_T>;

    /**
     * @brief Concept genérico para tipos integrales (signed o unsigned).
     * 
     * @tparam INT_T Tipo a verificar
     * 
     * @note USAGE: ACTIVELY USED - Para algoritmos que trabajan con cualquier integral
     */
    template <typename INT_TYPE>
    inline constexpr bool is_integral_type_v = std::is_integral_v<INT_TYPE>;
    template <typename INT_T>
    concept integral_c = is_integral_type_v<INT_T>;

    //================================================
    // SIZE COMPARISON UTILITIES
    //================================================
    
    /**
     * @brief Utilidades compile-time para comparar tamaños de tipos.
     * 
     * @details Proporcionan predicados constexpr para comparar sizeof(T) vs sizeof(S).
     *          Usadas internamente para selección de tipos en metaprogramación.
     * 
     * @tparam T Primer tipo
     * @tparam S Segundo tipo
     * 
     * @note eq_sz_v: sizeof(T) == sizeof(S)
     * @note gt_sz_v: sizeof(T) > sizeof(S)
     * @note lt_sz_v: sizeof(T) < sizeof(S)
     * @note ge_sz_v: sizeof(T) >= sizeof(S)
     * @note le_sz_v: sizeof(T) <= sizeof(S)
     * 
     * @note USAGE: INTERNALLY USED - Para ugly_details_* namespaces
     */
    template <typename T, typename S>
    constexpr bool eq_sz_v = (sizeof(T) == sizeof(S));
    template <typename T, typename S>
    constexpr bool gt_sz_v = (sizeof(T) > sizeof(S));
    template <typename T, typename S>
    constexpr bool lt_sz_v = (sizeof(T) < sizeof(S));
    template <typename T, typename S>
    constexpr bool ge_sz_v = gt_sz_v<T, S> || eq_sz_v<T, S>;
    template <typename T, typename S>
    constexpr bool le_sz_v = lt_sz_v<T, S> || eq_sz_v<T, S>;

    //================================================
    // TYPE DEDUCTION: UInt → UInt Conversion
    //================================================
    
    /**
     * @brief Namespace interno para deducción de tipos unsigned → unsigned.
     * 
     * @details Implementa metaprogramación para encontrar el tipo unsigned estándar
     *          equivalente a un tipo unsigned custom (ullint_t → uint64_t, etc.).
     *          
     *          Usa una cascada de std::conditional_t para seleccionar el tipo más
     *          pequeño que puede representar el custom type sin pérdida.
     * 
     * @note USAGE: INTERNAL IMPLEMENTATION DETAIL - No usar directamente
     * @note Acceso público via: sig_UInt_for_UInt_t<UInt_t>
     * 
     * @warning Nombre "ugly" indica que es implementación interna, no API pública
     */
    namespace ugly_details_UInt_for_UInt {

      template <unsigned_integral_c UInt_t>
      struct __sig_UInt_for_UInt_t { 
        // Mapeo genérico multiplataforma: unsigned → SIGUIENTE unsigned estándar (mayor capacidad)
        using type = std::conditional_t<
            sizeof(UInt_t) < 2, std::uint16_t,   // 1 byte  → uint16_t (siguiente)
            std::conditional_t<
                sizeof(UInt_t) < 4, std::uint32_t,   // 2 bytes → uint32_t (siguiente)
                std::conditional_t<
                    sizeof(UInt_t) < 8, std::uint64_t,   // 4 bytes → uint64_t (siguiente)
                    void  // 8 bytes no tiene siguiente tipo estándar
                >>>;
        
        static_assert(!std::is_same_v<type, void>,
                      "No larger standard unsigned type exists for this type size. "
                      "64-bit unsigned types have no standard larger type.");
      };
      
      // Especialización para uint64_t: usa el mismo tipo (no hay mayor)
      template <>
      struct __sig_UInt_for_UInt_t<std::uint64_t> {
        using type = std::uint64_t;  // Mismo tipo, no hay siguiente
      };

    } // namespace ugly_details_UInt_for_UInt

    /**
     * @brief Deduce el SIGUIENTE tipo unsigned estándar (mayor capacidad) para un unsigned.
     * 
     * @details Mapeo genérico multiplataforma basado en sizeof (siguiente tamaño):
     *          - sizeof < 2 bytes → std::uint16_t  (uint8_t  → uint16_t)
     *          - sizeof < 4 bytes → std::uint32_t  (uint16_t → uint32_t)
     *          - sizeof < 8 bytes → std::uint64_t  (uint32_t → uint64_t)
     *          - sizeof >= 8 bytes → error (no hay tipo mayor)
     * 
     *          Devuelve el tipo unsigned **del siguiente tamaño mayor**, no del mismo.
     *          Útil para operaciones que necesitan más capacidad (multiplicaciones, etc.).
     * 
     * @tparam UInt_t Tipo unsigned (hasta 4 bytes; 8 bytes no tiene siguiente)
     * 
     * @return Type alias al tipo unsigned del SIGUIENTE tamaño mayor
     * 
     * @note USAGE: ACTIVELY USED - Para operaciones que requieren mayor capacidad
     * @note Portabilidad: Basado en sizeof, funciona en todas las plataformas
     * 
     * @code
     * using T1 = sig_UInt_for_UInt_t<uint8_t>;   // std::uint16_t (siguiente)
     * using T2 = sig_UInt_for_UInt_t<uint16_t>;  // std::uint32_t (siguiente)
     * using T3 = sig_UInt_for_UInt_t<uint32_t>;  // std::uint64_t (siguiente)
     * // sig_UInt_for_UInt_t<uint64_t> → error (no hay siguiente)
     * @endcode
     */
    template <typename UInt_t>
    using sig_UInt_for_UInt_t =
        typename ugly_details_UInt_for_UInt::__sig_UInt_for_UInt_t<UInt_t>::type;

    //================================================
    // TYPE DEDUCTION: UInt → SInt Conversion
    //================================================
    
    /**
     * @brief Namespace interno para deducción unsigned → signed.
     * 
     * @details Encuentra el tipo signed correspondiente a un tipo unsigned dado.
     *          Ejemplo: ullint_t → sint64_t, uint_t → sint_t
     * 
     * @note USAGE: INTERNAL IMPLEMENTATION DETAIL
     * @note Acceso público via: sig_SInt_for_UInt_t<UInt>
     */
    namespace ugly_details_sig_SInt_for_UInt {
      template <typename UInt>
      struct __sig_SInt_for_UInt_t { 
        // Mapeo genérico multiplataforma: unsigned → signed del siguiente tamaño
        // Necesario porque signed del mismo tamaño no puede contener todos los valores unsigned
        using type = std::conditional_t<
            sizeof(UInt) == 1, std::int16_t,   // uint8_t  → int16_t
            std::conditional_t<
                sizeof(UInt) == 2, std::int32_t,   // uint16_t → int32_t
                std::conditional_t<
                    sizeof(UInt) == 4, std::int64_t,   // uint32_t → int64_t
                    std::conditional_t<
                        sizeof(UInt) == 8, sint64_t,   // uint64_t → sint64_t (custom 64-bit)
                        void  // Tipos > 64 bits no soportados
                    >>>>;
        
        static_assert(!std::is_same_v<type, void>,
                      "No standard signed type mapping exists for this unsigned type size. "
                      "Only 8, 16, 32, and 64-bit unsigned types are supported.");
      };

    } // namespace ugly_details_sig_SInt_for_UInt

    /**
     * @brief Deduce el tipo signed con capacidad suficiente para un tipo unsigned.
     * 
     * @details Mapeo genérico multiplataforma basado en sizeof:
     *          - sizeof == 1 byte  → std::int16_t  (uint8_t  → int16_t)
     *          - sizeof == 2 bytes → std::int32_t  (uint16_t → int32_t)
     *          - sizeof == 4 bytes → std::int64_t  (uint32_t → int64_t)
     *          - sizeof == 8 bytes → sint64_t      (uint64_t → sint64_t custom)
     * 
     *          El tipo signed resultante es del **siguiente tamaño mayor** porque
     *          un signed del mismo tamaño no puede representar todos los valores
     *          del unsigned (el bit de signo reduce el rango positivo).
     * 
     * @tparam UInt_t Tipo unsigned (cualquier tamaño: 1, 2, 4, u 8 bytes)
     * 
     * @return Type alias al tipo signed con capacidad suficiente
     * 
     * @note USAGE: ACTIVELY USED - Para conversiones signed/unsigned seguras
     * @note Portabilidad: Basado en sizeof, funciona en todas las plataformas
     * 
     * @warning El tipo signed resultante puede ser más grande que el unsigned
     *          de entrada para garantizar capacidad completa
     * 
     * @code
     * using S1 = sig_SInt_for_UInt_t<uint8_t>;   // std::int16_t (16 bits)
     * using S2 = sig_SInt_for_UInt_t<uint16_t>;  // std::int32_t (32 bits)
     * using S3 = sig_SInt_for_UInt_t<uint32_t>;  // std::int64_t (64 bits)
     * @endcode
     */
    template <typename UInt_t>
    using sig_SInt_for_UInt_t =
        typename ugly_details_sig_SInt_for_UInt::__sig_SInt_for_UInt_t<UInt_t>::type;

    //================================================
    // TYPE DEDUCTION: SInt → UInt Conversion
    //================================================
    
    /**
     * @brief Namespace interno para deducción signed → unsigned.
     * 
     * @details Encuentra el tipo unsigned correspondiente a un tipo signed.
     *          Mapeo directo: sint64_t → uint64_t, schint_t → uchint_t, etc.
     * 
     * @note USAGE: INTERNAL IMPLEMENTATION DETAIL
     * @note Acceso público via: sig_UInt_for_SInt_t<SInt>
     */
    namespace ugly_details_UInt_for_SInt {
      template <typename SInt>
      struct __sig_UInt_for_SInt_t { 
        // Mapeo genérico multiplataforma: signed → unsigned del mismo tamaño
        using type = std::conditional_t<
            sizeof(SInt) == 1, std::uint8_t,
            std::conditional_t<
                sizeof(SInt) == 2, std::uint16_t,
                std::conditional_t<
                    sizeof(SInt) == 4, std::uint32_t,
                    std::conditional_t<
                        sizeof(SInt) == 8, std::uint64_t,
                        void  // Tipos > 64 bits no soportados
                    >>>>;
        
        static_assert(!std::is_same_v<type, void>,
                      "No standard unsigned type mapping exists for this signed type size. "
                      "Only 8, 16, 32, and 64-bit signed types are supported.");
      };

    } // namespace ugly_details_UInt_for_SInt

    /**
     * @brief Deduce el tipo unsigned correspondiente a un tipo signed.
     * 
     * @details Mapeo genérico multiplataforma basado en sizeof (mismo tamaño):
     *          - sizeof == 1 byte  → std::uint8_t   (int8_t  → uint8_t)
     *          - sizeof == 2 bytes → std::uint16_t  (int16_t → uint16_t)
     *          - sizeof == 4 bytes → std::uint32_t  (int32_t → uint32_t)
     *          - sizeof == 8 bytes → std::uint64_t  (int64_t → uint64_t)
     * 
     *          El unsigned resultante tiene **el mismo tamaño** que el signed de entrada,
     *          permitiendo operaciones bitwise y conversiones directas.
     * 
     * @tparam Int_t Tipo signed (cualquier tamaño: 1, 2, 4, u 8 bytes)
     * 
     * @return Type alias al tipo unsigned del mismo tamaño
     * 
     * @note USAGE: ACTIVELY USED - Para operaciones que requieren unsigned temporalmente
     * @note Portabilidad: Basado en sizeof, funciona en todas las plataformas
     * @note El tipo resultante tiene sizeof(unsigned) == sizeof(signed)
     * 
     * @code
     * using U1 = sig_UInt_for_SInt_t<int8_t>;   // std::uint8_t (mismo tamaño)
     * using U2 = sig_UInt_for_SInt_t<int32_t>;  // std::uint32_t (mismo tamaño)
     * using U3 = sig_UInt_for_SInt_t<int64_t>;  // std::uint64_t (mismo tamaño)
     * @endcode
     */
    template <typename Int_t>
    using sig_UInt_for_SInt_t =
        typename ugly_details_UInt_for_SInt::__sig_UInt_for_SInt_t<Int_t>::type;

    //================================================
    // TYPE DEDUCTION: SInt → SInt Conversion
    //================================================
    
    /**
     * @brief Namespace interno para deducción signed → signed estándar.
     * 
     * @details Encuentra el tipo signed estándar equivalente a un signed custom.
     *          Similar a UInt_for_UInt pero para tipos con signo.
     * 
     * @note USAGE: INTERNAL IMPLEMENTATION DETAIL
     * @note Acceso público via: sig_SInt_for_SInt_t<SInt>
     */
    namespace ugly_details_SInt_for_SInt {

      template <typename SInt>
      struct __sig_SInt_for_SInt_t {
        // Mapeo genérico multiplataforma: signed → SIGUIENTE signed estándar (mayor capacidad)
        using type = std::conditional_t<
            sizeof(SInt) < 2, std::int16_t,   // 1 byte  → int16_t (siguiente)
            std::conditional_t<
                sizeof(SInt) < 4, std::int32_t,   // 2 bytes → int32_t (siguiente)
                std::conditional_t<
                    sizeof(SInt) < 8, std::int64_t,   // 4 bytes → int64_t (siguiente)
                    void  // 8 bytes no tiene siguiente tipo estándar
                >>>;
        
        static_assert(!std::is_same_v<type, void>,
                      "No larger standard signed type exists for this type size. "
                      "64-bit signed types have no standard larger type.");
      };
      
      // Especialización para int64_t: usa el mismo tipo (no hay mayor)
      template <>
      struct __sig_SInt_for_SInt_t<std::int64_t> {
        using type = std::int64_t;  // Mismo tipo, no hay siguiente
      };

    } // namespace ugly_details_SInt_for_SInt

    /**
     * @brief Deduce el SIGUIENTE tipo signed estándar (mayor capacidad) para un signed.
     * 
     * @details Mapeo genérico multiplataforma basado en sizeof (siguiente tamaño):
     *          - sizeof < 2 bytes → std::int16_t  (int8_t  → int16_t)
     *          - sizeof < 4 bytes → std::int32_t  (int16_t → int32_t)
     *          - sizeof < 8 bytes → std::int64_t  (int32_t → int64_t)
     *          - sizeof >= 8 bytes → error (no hay tipo mayor)
     * 
     *          Devuelve el tipo signed **del siguiente tamaño mayor**, no del mismo.
     *          Funciona independientemente de la plataforma (Windows/Linux).
     * 
     * @tparam SInt_t Tipo signed custom (hasta 4 bytes; 8 bytes no tiene siguiente)
     * 
     * @return Type alias al tipo signed del SIGUIENTE tamaño mayor
     * 
     * @note USAGE: ACTIVELY USED - Para operaciones que requieren mayor capacidad
     * @note Portabilidad: Basado en sizeof, funciona en todas las plataformas
     * 
     * @code
     * using S1 = sig_SInt_for_SInt_t<int8_t>;    // std::int16_t (siguiente)
     * using S2 = sig_SInt_for_SInt_t<int16_t>;   // std::int32_t (siguiente)
     * using S3 = sig_SInt_for_SInt_t<int32_t>;   // std::int64_t (siguiente)
     * // sig_SInt_for_SInt_t<int64_t> → error (no hay siguiente)
     * @endcode
     */
    template <typename SInt_t>
    using sig_SInt_for_SInt_t =
        typename ugly_details_SInt_for_SInt::__sig_SInt_for_SInt_t<SInt_t>::type;

    //================================================
    // BASE AND DIGIT UTILITIES
    //================================================
    
    /**
     * @brief Utilidades compile-time para valores especiales de base y dígitos.
     * 
     * @details Proporciona funciones consteval para obtener valores característicos:
     *          - maxbase(): Valor máximo representable (UINT_T::max())
     *          - submaxbase(): maxbase() - 1
     *          - minbase(): Base mínima válida (2)
     *          - subminbase(): 3
     *          - monobase(): 1 (base monádica, no estándar)
     *          - nobase(): 0 (valor nulo)
     *          - maxdigit(): Dígito máximo (submaxbase)
     *          - submaxdigit(): maxdigit() - 1
     *          - digit_0(), digit_1(): Dígitos 0 y 1
     *          - base_2(): Base binaria (2)
     * 
     * @tparam UINT_T Tipo unsigned para el cual calcular valores
     * 
     * @return Valores consteval del tipo UINT_T
     * 
     * @note USAGE: ACTIVELY USED - En validaciones de base y aritmética modular
     * 
     * @note Todas las funciones son consteval (evaluadas en compile-time)
     * 
     * @code
     * constexpr auto max_val = maxbase<uint8_t>();     // 255
     * constexpr auto min_b = minbase<uint8_t>();       // 2
     * constexpr auto max_d = maxdigit<uint16_t>();     // 65534
     * static_assert(maxdigit<uint8_t>() == 254);
     * @endcode
     */
    template <typename UINT_T>
    consteval UINT_T maxbase() { return (static_cast<UINT_T>(std::numeric_limits<UINT_T>::max())); }
    template <typename UINT_T>
    consteval UINT_T submaxbase() { return static_cast<UINT_T>(maxbase<UINT_T>() - 1); }
    template <typename UINT_T>
    consteval UINT_T minbase() { return static_cast<UINT_T>(2); }
    template <typename UINT_T>
    consteval UINT_T subminbase() { return static_cast<UINT_T>(3); }
    template <typename UINT_T>
    consteval UINT_T monobase() { return static_cast<UINT_T>(1); }
    template <typename UINT_T>
    consteval UINT_T nobase() { return static_cast<UINT_T>(0); }
    template <typename UINT_T>
    consteval UINT_T maxdigit() { return static_cast<UINT_T>(submaxbase<UINT_T>()); }
    template <typename UINT_T>
    consteval UINT_T submaxdigit() { return static_cast<UINT_T>(submaxbase<UINT_T>() - 1); }
    template <typename UINT_T>
    consteval UINT_T digit_0() { return static_cast<UINT_T>(0u); }
    template <typename UINT_T>
    consteval UINT_T digit_1() { return static_cast<UINT_T>(1u); }
    template <typename UINT_T>
    consteval UINT_T base_2() { return static_cast<UINT_T>(2u); }

  /**
   * @brief Valida que la base B sea >= 2.
   * 
   * @tparam UINT_T Tipo del valor de base
   * @tparam B Valor de base a validar
   * 
   * @return true si B >= 2, false en caso contrario
   * 
   * @note USAGE: INTERNALLY USED - Para static_assert en templates
   */
  template <typename UINT_T, UINT_T B>
  consteval bool base_geqt_2() { return (B >= base_2<UINT_T>()); }

  /**
   * @brief Valida que la base B sea <= maxbase.
   * 
   * @tparam UINT_T Tipo del valor de base
   * @tparam B Valor de base a validar
   * 
   * @return true si B <= maxbase<UINT_T>(), false en caso contrario
   * 
   * @note USAGE: INTERNALLY USED - Para static_assert en templates
   */
  template <typename UINT_T, UINT_T B>
  consteval bool base_leqt_max() { return (B <= maxbase<UINT_T>()); }

  /**
   * @brief Valida que la base B esté en el rango válido [2, maxbase] y quepa en uint32_t.
   * 
   * @tparam UINT_T Tipo del valor de base
   * @tparam B Valor de base a validar
   * 
   * @return true si 2 <= B <= maxbase<UINT_T>() AND B <= UINT32_MAX, false en caso contrario
   * 
   * @note USAGE: ACTIVELY USED - En static_assert para validar bases en templates
   * @note RESTRICCIÓN IMPORTANTE: Ninguna base puede ser mayor que UINT32_MAX
   * 
   * @warning Esta limitación aplica a dig_t y todas las estructuras derivadas
   * 
   * @code
   * template <typename UINT_T, UINT_T B>
   *   requires (suitable_base<UINT_T, B>())
   * void process() { ... }
   * 
   * // Ejemplo de error:
   * // suitable_base<uint64_t, 5000000000ULL>() → false (> UINT32_MAX)
   * @endcode
   */
  template <typename UINT_T, UINT_T B>
  consteval 
  bool suitable_base() { 
    return (base_geqt_2<UINT_T, B>() && 
            base_leqt_max<UINT_T, B>() && 
            B <= static_cast<UINT_T>(std::numeric_limits<std::uint32_t>::max())); 
  }

    /**
     * @brief Calcula el punto medio del rango [0, maxbase].
     * 
     * @details Retorna (maxbase + 1) / 2, útil para algoritmos de búsqueda binaria
     *          y división de rangos.
     * 
     * @tparam UINT_T Tipo unsigned
     * 
     * @return Punto medio del rango representable
     * 
     * @note USAGE: INTERNALLY USED - Para algoritmos de optimización
     * 
     * @note Usa sig_UInt_for_UInt_t para evitar overflow en (max + 1)
     */
    template <typename UINT_T>
    consteval UINT_T middle_max() {
      using SIG_UINT_T = sig_UInt_for_UInt_t<UINT_T>;
      constexpr SIG_UINT_T maximo = maxbase<UINT_T>();
      constexpr SIG_UINT_T uno{1};
      constexpr SIG_UINT_T dos{2};
      return static_cast<UINT_T>((maximo + uno) / dos);
    }

    /**
     * @brief Calcula floor(sqrt(maxbase<UINT_T>() + 1)) en tiempo de compilación.
     * 
     * @details Usa método de Newton-Raphson con aritmética entera pura en consteval
     *          para encontrar sqrt(maxbase + 1). Útil para determinar límites en
     *          multiplicaciones que no causen overflow.
     * 
     * @tparam UINT_T Tipo unsigned
     * 
     * @return floor(sqrt(maxbase + 1)) redondeado hacia abajo
     * 
     * @note USAGE: INTERNALLY USED - Para detección de overflow en multiplicaciones
     * 
     * @note Usa aritmética entera pura, más preciso y eficiente que long double
     * 
     * @code
     * constexpr auto limit = sqrt_max<uint32_t>(); // exactamente 65536
     * // Si a * b < (sqrt_max)^2, entonces a * b no causa overflow en uint32_t
     * @endcode
     */
    template <typename UINT_T>
    consteval UINT_T sqrt_max() {
      using SIG_UINT_T = sig_UInt_for_UInt_t<UINT_T>;
      constexpr SIG_UINT_T n = static_cast<SIG_UINT_T>(maxbase<UINT_T>()) + SIG_UINT_T{1};
      
      if constexpr (n == 0) return static_cast<UINT_T>(0);
      if constexpr (n == 1) return static_cast<UINT_T>(1);
      
      // Newton-Raphson con aritmética entera: x_{k+1} = (x_k + n/x_k) / 2
      // Estimación inicial: x0 = n / 2
      SIG_UINT_T x = n / 2;
      if (x == 0) x = 1;
      
      // Iterar hasta convergencia
      for (int i = 0; i < 50; ++i) {
        SIG_UINT_T x_new = (x + n / x) / 2;
        if (x_new >= x) break;  // Convergencia alcanzada
        x = x_new;
      }
      
      return static_cast<UINT_T>(x);
    }

    namespace ugly_details_for_suitable_type_deduction {
      template <typename T, T Radix>
      struct UIntTypeForRadix;
      template <integral_c T, T Radix>
      struct UIntTypeForRadix<T, Radix> {
        using UIntType = std::conditional_t<
            static_cast<uint64_t>(Radix) <= maxbase<uint8_t>(),
            uint8_t,
            std::conditional_t<
                static_cast<uint64_t>(Radix) <= maxbase<uint16_t>(),
                uint16_t,
                std::conditional_t<
                    static_cast<uint64_t>(Radix) <= maxbase<uint32_t>(),
                    uint32_t,
                    uint64_t>>>;
      };
    }
    
    using namespace ugly_details_for_suitable_type_deduction;

    template <ullint_t Radix>
    using TypeFromIntNumber_t = typename UIntTypeForRadix<decltype(Radix), Radix>::UIntType;

    namespace ugly_details_for_greater_suitable_type_deduction {
      
      template <typename T, T Radix>
      struct UIntTypeForRadixContainsMultResult;
      template <integral_c T, T Radix>
        requires(Radix > 1)
      struct UIntTypeForRadixContainsMultResult<T, Radix> {
        // Usa tipos de tamaño fijo para portabilidad multiplataforma
        static constexpr 
        ullint_t uint_value_0_max = static_cast<ullint_t>(sqrt_max<uint8_t>());
        static constexpr 
        ullint_t uint_value_1_max = static_cast<ullint_t>(sqrt_max<uint16_t>());
        static constexpr 
        ullint_t uint_value_2_max = static_cast<ullint_t>(sqrt_max<uint32_t>());
        static constexpr 
        ullint_t uint_value_3_max = static_cast<ullint_t>(sqrt_max<uint64_t>());
        static constexpr 
        ullint_t uint_value = static_cast<ullint_t>(Radix);
        
        // Mapea el tipo entero tradicional al tipo de tamaño fijo correspondiente
        template <typename TRAD_T>
        using fixed_size_type = std::conditional_t<
            sizeof(TRAD_T) == 1, uint8_t,
            std::conditional_t<
                sizeof(TRAD_T) == 2, uint16_t,
                std::conditional_t<
                    sizeof(TRAD_T) == 4, uint32_t,
                    uint64_t>>>;
        
        using UIntType = std::conditional_t<
            uint_value <= uint_value_0_max, uint8_t,
            std::conditional_t<
                uint_value <= uint_value_1_max, uint16_t,
                std::conditional_t<
                    uint_value <= uint_value_2_max, uint32_t,
                    uint64_t>>>;
      };
    } // namespace ugly_details_for_greater_suitable_type_deduction
    using namespace ugly_details_for_greater_suitable_type_deduction;

  } // namespace type_traits

  template <type_traits::unsigned_integral_c UINT_T>
  using uintspair = typename std::array<UINT_T, 2>;
  
  template <type_traits::unsigned_integral_c UINT_T, UINT_T B>
  using uintspairlist = typename std::array<uintspair<UINT_T>, B>;
  
  template <type_traits::unsigned_integral_c UINT_T, UINT_T B>
  using uintspairtbl = typename std::array<uintspairlist<UINT_T, B>, B>;

  namespace ugly_pack2tuple_details { /* simplified in new location */ }

  namespace ugly_details { 
    struct local_void_t {}; 
    
    template <class Head_t, class... Tail_t> 
    struct for_each_same_type { 
        using second_t = typename std::tuple_element<0, std::tuple<Tail_t...>>::type; 
        constexpr static bool are_same_type_v = (
            (std::is_same_v<Head_t, second_t>) && 
            (for_each_same_type<Tail_t...>::are_same_type_v)
        ); 
    }; 
    
    template <class Head_t> 
    struct for_each_same_type<Head_t> { 
        constexpr static bool are_same_type_v = true; 
    }; 
  } // close namespace ugly_details

  template <typename... Ts>
  concept all_are_the_same_type_c = requires(Ts...) { 
    ugly_details::for_each_same_type<Ts...>::are_same_type_v; 
  };

  template <typename... Ts>
  concept there_is_one_or_more_c = requires(Ts...) { ((sizeof...(Ts)) > 0); };

  /**
   * @brief Namespace interno para utilidades de conversión de parameter packs a arrays.
   */
  namespace ugly_pack_details {
     
    /**
     * @brief Convierte un parameter pack variádico en std::array en tiempo de compilación.
     * 
     * @details Esta estructura template proporciona utilidades para trabajar con parameter packs
     *          variádicos del mismo tipo y convertirlos en std::array. Requiere que todos los
     *          tipos sean iguales y que haya al menos un parámetro.
     * 
     * @tparam Ts... Parameter pack de tipos (todos deben ser del mismo tipo)
     * 
     * @note USAGE: INTERNALLY USED - Para conversiones compile-time de packs a arrays
     * @note Requiere: all_are_the_same_type_c<Ts...> && there_is_one_or_more_c<Ts...>
     * 
     * @par Tipos públicos
     * - `size`: Número de elementos en el pack (constexpr std::size_t)
     * - `inner_type`: Tipo común de todos los elementos (std::common_type_t)
     * - `array_type`: Tipo del array resultante (std::array<inner_type, size>)
     * - `elem_type`: Alias para inner_type
     * 
     * @par Métodos públicos
     * - `operator()`: Convierte pack a array en tiempo de compilación
     * - `get<J>`: Obtiene el J-ésimo elemento del pack
     * - `for_each`: Asigna elementos del pack a un array existente
     * 
     * @code
     * using P2A = pack2array<int, int, int>;
     * constexpr auto arr = P2A{}(10, 20, 30);          // arr = {10, 20, 30}
     * constexpr auto elem = P2A::get<1>(10, 20, 30);   // elem = 20
     * @endcode
     */
    template <typename... Ts> 
        requires(
            all_are_the_same_type_c<Ts...> && 
            there_is_one_or_more_c<Ts...>
        ) 
    struct pack2array { 
        
        /// @brief Número de elementos en el parameter pack
        static constexpr std::size_t size = sizeof...(Ts); 
        
        /// @brief Tipo común de todos los elementos del pack (sin referencias)
        using inner_type = std::remove_cvref_t<std::common_type_t<Ts...>>; 
        
        /// @brief Tipo del array resultante
        using array_type = std::array<inner_type, size>; 
        
        /// @brief Alias para el tipo de elemento individual
        using elem_type = inner_type; 
        
        /**
         * @brief Convierte parameter pack a std::array con perfect forwarding.
         * 
         * @tparam Us... Tipos deducidos con referencias (para forwarding)
         * @param args... Elementos del parameter pack a convertir
         * @return array_type Array con los elementos convertidos
         * 
         * @note Usa perfect forwarding: lvalues se copian, rvalues se mueven
         * @note constexpr - Evaluable en tiempo de compilación
         * @note noexcept - No lanza excepciones
         * 
         * @par Compatibilidad
         * Acepta tanto pack2array<int, int> como pack2array<const int&, const int&>
         */
        template <typename... Us> constexpr 
        array_type operator()(Us&&... args) const noexcept { 
            return array_type{static_cast<inner_type>(std::forward<Us>(args))...}; 
        } // end function operator ()
        
        /**
         * @brief Obtiene el J-ésimo elemento del parameter pack.
         * 
         * @tparam J Índice del elemento a obtener (0-based)
         * @param args... Elementos del parameter pack
         * @return elem_type El J-ésimo elemento
         * 
         * @note constexpr - Evaluable en tiempo de compilación
         * @note noexcept - No lanza excepciones
         */
        template <std::size_t J> static constexpr 
        elem_type get(Ts... args) noexcept { 
            array_type content{static_cast<inner_type>(args)...}; 
            return content[J]; 
        } // end function get
        
        /**
         * @brief Implementación interna de for_each usando fold expressions.
         * 
         * @tparam I... Secuencia de índices
         * @param iarray Array destino donde asignar elementos
         * @param args... Elementos del parameter pack a asignar
         * 
         * @note constexpr - Evaluable en tiempo de compilación
         * @note noexcept - No lanza excepciones
         * @note INTERNALLY USED - No llamar directamente, usar for_each()
         */
        template <std::size_t... I> static constexpr 
        void for_each_impl(array_type &iarray, const Ts... args, std::index_sequence<I...>) noexcept { 
            ((iarray[I] = static_cast<inner_type>(args)), ...); 
        } // end function for_each_impl
        
        /**
         * @brief Asigna elementos del pack a un array existente.
         * 
         * @param iarray Array destino (modificado por referencia)
         * @param args... Elementos del parameter pack a asignar
         * 
         * @note constexpr - Evaluable en tiempo de compilación
         * @note noexcept - No lanza excepciones
         * 
         * @code
         * std::array<int, 3> arr;
         * pack2array<int, int, int>::for_each(arr, 10, 20, 30);
         * // arr ahora contiene {10, 20, 30}
         * @endcode
         */
        static constexpr 
        void for_each(array_type &iarray, const Ts... args) noexcept { 
            for_each_impl(iarray, args..., std::make_index_sequence<sizeof...(args)>{}); 
        } // end function for_each
    }; 
  } // namespace ugly_pack_details

  /**
   * @brief Asigna elementos de un parameter pack a un contenedor manteniendo el orden.
   * 
   * @details Utilidad de alto nivel para asignar múltiples valores del mismo tipo
   *          a un contenedor (típicamente std::array) preservando el orden de los
   *          argumentos. Internamente usa pack2array para la conversión.
   * 
   * @tparam Ts... Tipos de los elementos a asignar (todos deben ser iguales)
   * @param dest Contenedor destino (modificado por referencia)
   * @param args... Elementos a asignar al contenedor
   * 
   * @note USAGE: ACTIVELY USED - Para asignaciones ordenadas de packs
   * @note Requiere: all_are_the_same_type_c<Ts...> && there_is_one_or_more_c<Ts...>
   * @note noexcept - No lanza excepciones
   * 
   * @code
   * std::array<int, 4> arr;
   * assign_with_order(arr, 10, 20, 30, 40);
   * // arr = {10, 20, 30, 40}
   * @endcode
   */
    template <class... Ts>
        requires(
            all_are_the_same_type_c<Ts...> && 
            there_is_one_or_more_c<Ts...>
        )
    void assign_with_order(auto &dest, const Ts &...args) noexcept {
        using type = typename ugly_pack_details::pack2array<Ts...>;
        type::for_each(dest, args...);
        return;
    } // end function assign_with_order

} // namespace NumRepr

#endif // BASIC_TYPES_HPP_INCLUDED
