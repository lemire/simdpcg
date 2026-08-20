# simdpcg

PCG is family of pseudo-random number generators (RNG) invented by
 Melissa O'Neill.



We present   a vectorized version of PCG32, a popular random-number generator
part of this family. It is written in C. The implementation uses Intel's SIMD
instructions and is based on O'Neill's original (pure C) implementation.

The vectorized version is at least twice a fast on a recent Intel processor.
In some tests, it is twice as fast as a [vectorized version xorshift128+](https://github.com/lemire/SIMDxorshift), but it uses wider registers and fancier instructions.


## Prerequisite

You should have a recent Intel processor (Skylake-X or better) support AVX-512 instructions
(we need the AVX512DQ instructions). 

## Code sample

```C
#include "simdpcg32.h"

// create a new key
avx512_pcg32_random_t key;
uint64_t state = 0x4d595df4d0f33173;
uint64_t inc = 0x14057b7ef767814f;
avx512_pcg32_initialize(&key, state, inc);

// generate 32 random bytes, do this as many times as you want
__m256i randomstuff =  avx512_pcg32_random_r(&key);
```

## Sample result

```
Generating 50000 32-bit random numbers
Time reported in number of cycles per array element.
We store values to an array of size = 195 kB.

We just generate the random numbers:
populateRandom_pcg32(prec, size):  4.25 cycles per operation
populateRandom_avx512_pcg32(prec, size):  2.37 cycles per operation
populateRandom_avx512_pcg32_two(prec, size):  1.21 cycles per operation
populateRandom_avx512_pcg32_four(prec, size):  1.14 cycles per operation
```

## Usage 

```bash
$ make
$ ./fillarray
```


