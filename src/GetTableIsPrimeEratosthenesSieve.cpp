#include <bitset>
#include <fstream>
#include <iostream>

constexpr size_t N = 65536;

// Sieve of Eratosthenes (iterative)
std::bitset<N> make_prime_bitset() {
    std::bitset<N> is_prime;
    is_prime.set();
    is_prime[0] = is_prime[1] = 0;
    for (size_t i = 2; i * i < N; ++i) {
        if (is_prime[i]) {
            for (size_t j = i * i; j < N; j += i) {
                is_prime[j] = 0;
            }
        }
    }
    return is_prime;
}

static const std::bitset<N> is_prime_lt_65537 = make_prime_bitset();

// Write the bitset as a C++ header
int main() {
    std::ofstream out("PrimeEratostenesSieve_lookup.hpp");
    out << "#pragma once\n";
    out << "#include <bitset>\n\n";
    out << "constexpr std::bitset<" << N << "> is_prime_lt_65537 = {\n";
    for (size_t i = 0; i < N; i += 64) {
        out << "    \"";
        for (size_t j = 0; j < 64 && (i + j) < N; ++j) {
            out << (is_prime_lt_65537[i + j] ? '1' : '0');
        }
        out << "\"\n";
    }
    out << "};\n";
    out.close();
    std::cout << "Header PrimeEratostenesSieve_lookup.hpp generado.\n";
    return 0;
}
