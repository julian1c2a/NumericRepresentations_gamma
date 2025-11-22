#include "../include/core/internal/lookup_tables/primes_ct.hpp"

static_assert(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct<2>(), "2 debe ser primo");
static_assert(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct<3>(), "3 debe ser primo");
static_assert(NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct<65521>(), "65521 debe ser primo");
static_assert(!NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct<0>(), "0 no debe ser primo");
static_assert(!NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct<1>(), "1 no debe ser primo");
static_assert(!NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct<4>(), "4 no debe ser primo");
static_assert(!NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct<65535>(), "65535 no debe ser primo");
static_assert(!NumRepr::AuxFunc::LUT::is_prime_lt_65537_ct<65536>(), "65536 no debe ser primo");
