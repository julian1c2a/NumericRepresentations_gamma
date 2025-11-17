#ifndef LOOKUP_TABLES_HPP
#define LOOKUP_TABLES_HPP

#include <cstdint>
#include <array>
#include <algorithm>
#include <limits>

namespace NumRepr {

namespace auxiliary_functions {
	
namespace LUT {

  /**
   * @brief Calcula en tiempo de compilación el exponente máximo para una base dada que no desborda un uint64_t.
   * @details El valor se determina a partir de una tabla precalculada de valores.
   * @tparam base La base para la cual calcular el exponente máximo.
   * @return El exponente máximo seguro.
   * @note La tabla de valores fue obtenida con Maxima CAS. Ejemplos:
   * - [2,63],[3,40],[10,19],[16,15]
   */
  template <std::uint64_t base>
  consteval std::size_t max_exponent_for_base_ct() noexcept {
      if constexpr (base < 2) return std::numeric_limits<std::uint64_t>::max();
      else if constexpr (base == 2) return 63;
      else if constexpr (base == 3) return 40;
      else if constexpr (base == 4) return 31;
      else if constexpr (base == 5) return 27;
      else if constexpr (base == 6) return 24;
      else if constexpr (base == 7) return 22;
      else if constexpr (base == 8) return 21;
      else if constexpr (base == 9) return 20;
      else if constexpr (base == 10) return 19;
      else if constexpr (base == 11) return 18;
      else if constexpr (base >= 12 && base < 14) return 17;
      else if constexpr (base >= 14 && base < 16) return 16;
      else if constexpr (base >= 16 && base < 20) return 15;
      else if constexpr (base >= 20 && base < 24) return 14;
      else if constexpr (base >= 24 && base < 31) return 13;
      else if constexpr (base >= 31 && base < 41) return 12;
      else if constexpr (base >= 41 && base < 57) return 11;
      else if constexpr (base >= 57 && base < 85) return 10;
      else if constexpr (base >= 85 && base < 139) return 9;
      else if constexpr (base >= 139 && base < 256) return 8;
      else if constexpr (base >= 256ull && base < 566ull) return 7;
      else if constexpr (base >= 566ull && base < 1626ull) return 6;
      else if constexpr (base >= 1626ull && base < 7132ull) return 5;
      else if constexpr (base >= 7132ull && base < 65536ull) return 4;
      else if constexpr (base >= 65536ull && base < 2642246ull) return 3;
      else if constexpr (base >= 2642246ull && base < 4294967296ull) return 2;
      else return 1;
  } // END OF FUNCTION max_exponent_for_base_ct compiletime

  /**
   * @brief Calcula en tiempo de ejecución el exponente máximo para una base dada que no desborda un uint64_t.
   * @param base La base para la cual calcular el exponente máximo.
   * @return El exponente máximo seguro.
   */
  constexpr std::size_t max_exponent_for_base(std::uint64_t base) noexcept {
      if (base < 2) return std::numeric_limits<std::uint64_t>::max();
      else if (base == 2) return 63;
      else if (base == 3) return 40;
      else if (base == 4) return 31;
      else if (base == 5) return 27;
      else if (base == 6) return 24;
      else if (base == 7) return 22;
      else if (base == 8) return 21;
      else if (base == 9) return 20;
      else if (base == 10) return 19;
      else if (base == 11) return 18;
      else if (base >= 12 && base < 14) return 17;
      else if (base >= 14 && base < 16) return 16;
      else if (base >= 16 && base < 20) return 15;
      else if (base >= 20 && base < 24) return 14;
      else if (base >= 24 && base < 31) return 13;
      else if (base >= 31 && base < 41) return 12;
      else if (base >= 41 && base < 57) return 11;
      else if (base >= 57 && base < 85) return 10;
      else if (base >= 85 && base < 139) return 9;
      else if (base >= 139 && base < 256) return 8;
      else if (base >= 256 && base < 566) return 7;
      else if (base >= 566 && base < 1626) return 6;
      else if (base >= 1626 && base < 7132) return 5;
      else if (base >= 7132 && base < 65536) return 4;
      else if (base >= 65536 && base < 2642246) return 3;
      else if (base >= 2642246 && base < 4294967296) return 2;
      else return 1;
  } // END OF FUNCTION max_exponent_for_base runtime
  
  // La siguiente tabla la ha impreso Maxima CAS con el siguiente programa
  // print_first_primes_formatted(k, cols) :=
  //     block(
  //         count: 0,
  //         n: 2,
  //         while(count < k) do (
  //             if (primep(n)) then (
  //                 printf(true, "~6d , ", n), 
  //                 count: count + 1,
  //                 if (mod(count, cols) = 0) then (
  //                     printf(true, "~%    ") 
  //                 ),
  //                 n: n + 1
  //             ) else (
  //                 n: n + 1
  //             )
  //         )
  //     );
  // print_first_primes_formatted(1024, 16); 
  static constexpr std::array<std::uint64_t,1024> primes{
       2ull,   3ull,   5ull,   7ull,  11ull,  13ull,  17ull,  19ull,  23ull,  29ull,  31ull,  37ull,  41ull,  43ull,  47ull,  53ull,
      59ull,  61ull,  67ull,  71ull,  73ull,  79ull,  83ull,  89ull,  97ull, 101ull, 103ull, 107ull, 109ull, 113ull, 127ull, 131ull,
     137ull, 139ull, 149ull, 151ull, 157ull, 163ull, 167ull, 173ull, 179ull, 181ull, 191ull, 193ull, 197ull, 199ull, 211ull, 223ull,
     227ull, 229ull, 233ull, 239ull, 241ull, 251ull, 257ull, 263ull, 269ull, 271ull, 277ull, 281ull, 283ull, 293ull, 307ull, 311ull,
     313ull, 317ull, 331ull, 337ull, 347ull, 349ull, 353ull, 359ull, 367ull, 373ull, 379ull, 383ull, 389ull, 397ull, 401ull, 409ull,
     419ull, 421ull, 431ull, 433ull, 439ull, 443ull, 449ull, 457ull, 461ull, 463ull, 467ull, 479ull, 487ull, 491ull, 499ull, 503ull,
     509ull, 521ull, 523ull, 541ull, 547ull, 557ull, 563ull, 569ull, 571ull, 577ull, 587ull, 593ull, 599ull, 601ull, 607ull, 613ull,
     617ull, 619ull, 631ull, 641ull, 643ull, 647ull, 653ull, 659ull, 661ull, 673ull, 677ull, 683ull, 691ull, 701ull, 709ull, 719ull,
     727ull, 733ull, 739ull, 743ull, 751ull, 757ull, 761ull, 769ull, 773ull, 787ull, 797ull, 809ull, 811ull, 821ull, 823ull, 827ull,
     829ull, 839ull, 853ull, 857ull, 859ull, 863ull, 877ull, 881ull, 883ull, 887ull, 907ull, 911ull, 919ull, 929ull, 937ull, 941ull,
     947ull, 953ull, 967ull, 971ull, 977ull, 983ull, 991ull, 997ull,1009ull,1013ull,1019ull,1021ull,1031ull,1033ull,1039ull,1049ull,
    1051ull,1061ull,1063ull,1069ull,1087ull,1091ull,1093ull,1097ull,1103ull,1109ull,1117ull,1123ull,1129ull,1151ull,1153ull,1163ull,
    1171ull,1181ull,1187ull,1193ull,1201ull,1213ull,1217ull,1223ull,1229ull,1231ull,1237ull,1249ull,1259ull,1277ull,1279ull,1283ull,
    1289ull,1291ull,1297ull,1301ull,1303ull,1307ull,1319ull,1321ull,1327ull,1361ull,1367ull,1373ull,1381ull,1399ull,1409ull,1423ull,
    1427ull,1429ull,1433ull,1439ull,1447ull,1451ull,1453ull,1459ull,1471ull,1481ull,1483ull,1487ull,1489ull,1493ull,1499ull,1511ull,
    1523ull,1531ull,1543ull,1549ull,1553ull,1559ull,1567ull,1571ull,1579ull,1583ull,1597ull,1601ull,1607ull,1609ull,1613ull,1619ull,
    1621ull,1627ull,1637ull,1657ull,1663ull,1667ull,1669ull,1693ull,1697ull,1699ull,1709ull,1721ull,1723ull,1733ull,1741ull,1747ull,
    1753ull,1759ull,1777ull,1783ull,1787ull,1789ull,1801ull,1811ull,1823ull,1831ull,1847ull,1861ull,1867ull,1871ull,1873ull,1877ull,
    1879ull,1889ull,1901ull,1907ull,1913ull,1931ull,1933ull,1949ull,1951ull,1973ull,1979ull,1987ull,1993ull,1997ull,1999ull,2003ull,
    2011ull,2017ull,2027ull,2029ull,2039ull,2053ull,2063ull,2069ull,2081ull,2083ull,2087ull,2089ull,2099ull,2111ull,2113ull,2129ull,
    2131ull,2137ull,2141ull,2143ull,2153ull,2161ull,2179ull,2203ull,2207ull,2213ull,2221ull,2237ull,2239ull,2243ull,2251ull,2267ull,
    2269ull,2273ull,2281ull,2287ull,2293ull,2297ull,2309ull,2311ull,2333ull,2339ull,2341ull,2347ull,2351ull,2357ull,2371ull,2377ull,
    2381ull,2383ull,2389ull,2393ull,2399ull,2411ull,2417ull,2423ull,2437ull,2441ull,2447ull,2459ull,2467ull,2473ull,2477ull,2503ull,
    2521ull,2531ull,2539ull,2543ull,2549ull,2551ull,2557ull,2579ull,2591ull,2593ull,2609ull,2617ull,2621ull,2633ull,2647ull,2657ull,
    2659ull,2663ull,2671ull,2677ull,2683ull,2687ull,2689ull,2693ull,2699ull,2707ull,2711ull,2713ull,2719ull,2729ull,2731ull,2741ull,
    2749ull,2753ull,2767ull,2777ull,2789ull,2791ull,2797ull,2801ull,2803ull,2819ull,2833ull,2837ull,2843ull,2851ull,2857ull,2861ull,
    2879ull,2887ull,2897ull,2903ull,2909ull,2917ull,2927ull,2939ull,2953ull,2957ull,2963ull,2969ull,2971ull,2999ull,3001ull,3011ull,
    3019ull,3023ull,3037ull,3041ull,3049ull,3061ull,3067ull,3079ull,3083ull,3089ull,3109ull,3119ull,3121ull,3137ull,3163ull,3167ull,
    3169ull,3181ull,3187ull,3191ull,3203ull,3209ull,3217ull,3221ull,3229ull,3251ull,3253ull,3257ull,3259ull,3271ull,3299ull,3301ull,
    3307ull,3313ull,3319ull,3323ull,3329ull,3331ull,3343ull,3347ull,3359ull,3361ull,3371ull,3373ull,3389ull,3391ull,3407ull,3413ull,
    3433ull,3449ull,3457ull,3461ull,3463ull,3467ull,3469ull,3491ull,3499ull,3511ull,3517ull,3527ull,3529ull,3533ull,3539ull,3541ull,
    3547ull,3557ull,3559ull,3571ull,3581ull,3583ull,3593ull,3607ull,3613ull,3617ull,3623ull,3631ull,3637ull,3643ull,3659ull,3671ull,
    3673ull,3677ull,3691ull,3697ull,3701ull,3709ull,3719ull,3727ull,3733ull,3739ull,3761ull,3767ull,3769ull,3779ull,3793ull,3797ull,
    3803ull,3821ull,3823ull,3833ull,3847ull,3851ull,3853ull,3863ull,3877ull,3881ull,3889ull,3907ull,3911ull,3917ull,3919ull,3923ull,
    3929ull,3931ull,3943ull,3947ull,3967ull,3989ull,4001ull,4003ull,4007ull,4013ull,4019ull,4021ull,4027ull,4049ull,4051ull,4057ull,
    4073ull,4079ull,4091ull,4093ull,4099ull,4111ull,4127ull,4129ull,4133ull,4139ull,4153ull,4157ull,4159ull,4177ull,4201ull,4211ull,
    4217ull,4219ull,4229ull,4231ull,4241ull,4243ull,4253ull,4259ull,4261ull,4271ull,4273ull,4283ull,4289ull,4297ull,4327ull,4337ull,
    4339ull,4349ull,4357ull,4363ull,4373ull,4391ull,4397ull,4409ull,4421ull,4423ull,4441ull,4447ull,4451ull,4457ull,4463ull,4481ull,
    4483ull,4493ull,4507ull,4513ull,4517ull,4519ull,4523ull,4547ull,4549ull,4561ull,4567ull,4583ull,4591ull,4597ull,4603ull,4621ull,
    4637ull,4639ull,4643ull,4649ull,4651ull,4657ull,4663ull,4673ull,4679ull,4691ull,4703ull,4721ull,4723ull,4729ull,4733ull,4751ull,
    4759ull,4783ull,4787ull,4789ull,4793ull,4799ull,4801ull,4813ull,4817ull,4831ull,4861ull,4871ull,4877ull,4889ull,4903ull,4909ull,
    4919ull,4931ull,4933ull,4937ull,4943ull,4951ull,4957ull,4967ull,4969ull,4973ull,4987ull,4993ull,4999ull,5003ull,5009ull,5011ull,
    5021ull,5023ull,5039ull,5051ull,5059ull,5077ull,5081ull,5087ull,5099ull,5101ull,5107ull,5113ull,5119ull,5147ull,5153ull,5167ull,
    5171ull,5179ull,5189ull,5197ull,5209ull,5227ull,5231ull,5233ull,5237ull,5261ull,5273ull,5279ull,5281ull,5297ull,5303ull,5309ull,
    5323ull,5333ull,5347ull,5351ull,5381ull,5387ull,5393ull,5399ull,5407ull,5413ull,5417ull,5419ull,5431ull,5437ull,5441ull,5443ull,
    5449ull,5471ull,5477ull,5479ull,5483ull,5501ull,5503ull,5507ull,5519ull,5521ull,5527ull,5531ull,5557ull,5563ull,5569ull,5573ull,
    5581ull,5591ull,5623ull,5639ull,5641ull,5647ull,5651ull,5653ull,5657ull,5659ull,5669ull,5683ull,5689ull,5693ull,5701ull,5711ull,
    5717ull,5737ull,5741ull,5743ull,5749ull,5779ull,5783ull,5791ull,5801ull,5807ull,5813ull,5821ull,5827ull,5839ull,5843ull,5849ull,
    5851ull,5857ull,5861ull,5867ull,5869ull,5879ull,5881ull,5897ull,5903ull,5923ull,5927ull,5939ull,5953ull,5981ull,5987ull,6007ull,
    6011ull,6029ull,6037ull,6043ull,6047ull,6053ull,6067ull,6073ull,6079ull,6089ull,6091ull,6101ull,6113ull,6121ull,6131ull,6133ull,
    6143ull,6151ull,6163ull,6173ull,6197ull,6199ull,6203ull,6211ull,6217ull,6221ull,6229ull,6247ull,6257ull,6263ull,6269ull,6271ull,
    6277ull,6287ull,6299ull,6301ull,6311ull,6317ull,6323ull,6329ull,6337ull,6343ull,6353ull,6359ull,6361ull,6367ull,6373ull,6379ull,
    6389ull,6397ull,6421ull,6427ull,6449ull,6451ull,6469ull,6473ull,6481ull,6491ull,6521ull,6529ull,6547ull,6551ull,6553ull,6563ull,
    6569ull,6571ull,6577ull,6581ull,6599ull,6607ull,6619ull,6637ull,6653ull,6659ull,6661ull,6673ull,6679ull,6689ull,6691ull,6701ull,
    6703ull,6709ull,6719ull,6733ull,6737ull,6761ull,6763ull,6779ull,6781ull,6791ull,6793ull,6803ull,6823ull,6827ull,6829ull,6833ull,
    6841ull,6857ull,6863ull,6869ull,6871ull,6883ull,6899ull,6907ull,6911ull,6917ull,6947ull,6949ull,6959ull,6961ull,6967ull,6971ull,
    6977ull,6983ull,6991ull,6997ull,7001ull,7013ull,7019ull,7027ull,7039ull,7043ull,7057ull,7069ull,7079ull,7103ull,7109ull,7121ull,
    7127ull,7129ull,7151ull,7159ull,7177ull,7187ull,7193ull,7207ull,7211ull,7213ull,7219ull,7229ull,7237ull,7243ull,7247ull,7253ull,
    7283ull,7297ull,7307ull,7309ull,7321ull,7331ull,7333ull,7349ull,7351ull,7369ull,7393ull,7411ull,7417ull,7433ull,7451ull,7457ull,
    7459ull,7477ull,7481ull,7487ull,7489ull,7499ull,7507ull,7517ull,7523ull,7529ull,7537ull,7541ull,7547ull,7549ull,7559ull,7561ull,
    7573ull,7577ull,7583ull,7589ull,7591ull,7603ull,7607ull,7621ull,7639ull,7643ull,7649ull,7669ull,7673ull,7681ull,7687ull,7691ull,
    7699ull,7703ull,7717ull,7723ull,7727ull,7741ull,7753ull,7757ull,7759ull,7789ull,7793ull,7817ull,7823ull,7829ull,7841ull,7853ull,
    7867ull,7873ull,7877ull,7879ull,7883ull,7901ull,7907ull,7919ull,7927ull,7933ull,7937ull,7949ull,7951ull,7963ull,7993ull,8009ull,
    8011ull,8017ull,8039ull,8053ull,8059ull,8069ull,8081ull,8087ull,8089ull,8093ull,8101ull,8111ull,8117ull,8123ull,8147ull,8161ull
  }; // END OF CONSTEXPR ARRAY primes
  
  // Esta función espera un número entre el 1 y el 1024
  consteval std::uint64_t get_nth_prime(std::size_t pos) noexcept {
	  if ((pos > 0)&&(pos <= 1024)) return primes[pos-1];
	  else                          return 0ull;
  } // END OF FUNCTION get_nth_prime compiletime
  
  
  constexpr bool is_prime_ct(std::uint64_t n) {
    // std::binary_search funciona en tiempo de compilación con C++20
    return std::binary_search(primes.begin(), primes.end(), n);
  } // END OF FUNCTION is_prime_ct compiletime
	
} // CLOSE NAMESPACE LUT
	
} // CLOSE NAMESPACE AUXILIARY_FUNCTIONS

} // CLOSE NAMESPACE NUMREPR

#endif // LOOKUP_TABLES_HPP