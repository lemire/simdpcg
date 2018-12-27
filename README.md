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
avx512_pcg32_random_t key = {
    .state = _mm512_set_epi64(0xb5f380a45f908741, 0x88b545898d45385d, 0xd81c7fe764f8966c, 0x44a9a3b6b119e7bc, 0x3cb6e04dc22f629, 0x727947debc931183, 0xfbfa8fdcff91891f, 0xb9384fd8f34c0f49),
    .inc = _mm512_set_epi64(0xbf2de0670ac3d03e, 0x98c40c0dc94e71e, 0xf3565f35a8c61d00, 0xd3c83e29b30df640, 0x14b7f6e4c89630fa, 0x37cc7b0347694551, 0x4a052322d95d485b, 0x10f3ade77a26e15e),
      .multiplier =  _mm512_set1_epi64(0x5851f42d4c957f2d)};

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


