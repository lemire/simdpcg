# simdpcg

PCG is family of pseudo-random number generators (RNG) invented by
 Melissa O'Neill.



We present   a vectorized version of PCG32, a popular random-number generator
part of this family. It is written in C. The implementation uses Intel's SIMD
instructions and is based on O'Neill's original (pure C) implementation.


## Prerequisite

You should have a recent Intel processor (Skylake-X or better) support AVX-512 instructions
(we need the AVX512DQ instructions). 

## Code sample

```C
#include "simdpcg.h"

// create a new key
avx512_pcg32_random_t key = {
    .state = _mm512_set1_epi64(1111), 
    .inc = _mm512_set_epi64(15,13,11,9,7,5,3,1),
    .multiplier = _mm512_set1_epi64(0x5851f42d4c957f2d)};

// generate 32 random bytes, do this as many times as you want
__m256i randomstuff =  avx512_pcg32_random_r(&mykey);
```

## Usage 

```bash
$ make
$ ./fillarray
```


