#ifndef SIMDXORSHIFT128PLUS_H
#define SIMDXORSHIFT128PLUS_H

#include <stdint.h> // life is short, please use a C99-compliant compiler

#if defined(_MSC_VER)
/* Microsoft C/C++-compatible compiler */
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
/* GCC-compatible compiler, targeting x86/x86-64 */
#include <x86intrin.h>
#elif defined(__GNUC__) && defined(__ARM_NEON__)
/* GCC-compatible compiler, targeting ARM with NEON */
#include <arm_neon.h>
#elif defined(__GNUC__) && defined(__IWMMXT__)
/* GCC-compatible compiler, targeting ARM with WMMX */
#include <mmintrin.h>
#elif (defined(__GNUC__) || defined(__xlC__)) &&                               \
    (defined(__VEC__) || defined(__ALTIVEC__))
/* XLC or GCC-compatible compiler, targeting PowerPC with VMX/VSX */
#include <altivec.h>
#elif defined(__GNUC__) && defined(__SPE__)
/* GCC-compatible compiler, targeting PowerPC with SPE */
#include <spe.h>
#endif

#ifdef __AVX2__
#define AVX2PCG
typedef struct avx2_pcg_state_setseq_64 {
  __m256i state[2]; // RNG state.  All values are possible.
  __m256i inc[2];   // Controls which RNG sequence (stream) is selected. Must
                    // *always* be odd.
  __m256i
      pcg32_mult_l; // set to _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2dULL)
                    // & 0xffffffffu);
  __m256i
      pcg32_mult_h; // set to _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2dULL)
                    // >> 32);

} avx2_pcg32_random_t;

// credit Wenzel Jakob
// https://github.com/wjakob/pcg32/blob/master/pcg32_8.h
static inline __m256i avx2_pcg32_random_r(avx2_pcg32_random_t *rng) {
  const __m256i mask_l = _mm256_set1_epi64x(UINT64_C(0x00000000ffffffff));
  const __m256i shift0 = _mm256_set_epi32(7, 7, 7, 7, 6, 4, 2, 0);
  const __m256i shift1 = _mm256_set_epi32(6, 4, 2, 0, 7, 7, 7, 7);
  const __m256i const32 = _mm256_set1_epi32(32);

  __m256i s0 = rng->state[0], s1 = rng->state[1];

  /* Extract low and high words for partial products below */
  __m256i s0_l = _mm256_and_si256(s0, mask_l);
  __m256i s0_h = _mm256_srli_epi64(s0, 32);
  __m256i s1_l = _mm256_and_si256(s1, mask_l);
  __m256i s1_h = _mm256_srli_epi64(s1, 32);

  /* Improve high bits using xorshift step */
  __m256i s0s = _mm256_srli_epi64(s0, 18);
  __m256i s1s = _mm256_srli_epi64(s1, 18);

  __m256i s0x = _mm256_xor_si256(s0s, s0);
  __m256i s1x = _mm256_xor_si256(s1s, s1);

  __m256i s0xs = _mm256_srli_epi64(s0x, 27);
  __m256i s1xs = _mm256_srli_epi64(s1x, 27);

  __m256i xors0 = _mm256_and_si256(mask_l, s0xs);
  __m256i xors1 = _mm256_and_si256(mask_l, s1xs);

  /* Use high bits to choose a bit-level rotation */
  __m256i rot0 = _mm256_srli_epi64(s0, 59);
  __m256i rot1 = _mm256_srli_epi64(s1, 59);

  /* 64 bit multiplication using 32 bit partial products :( */
  __m256i m0_hl = _mm256_mul_epu32(s0_h, rng->pcg32_mult_l);
  __m256i m1_hl = _mm256_mul_epu32(s1_h, rng->pcg32_mult_l);
  __m256i m0_lh = _mm256_mul_epu32(s0_l, rng->pcg32_mult_h);
  __m256i m1_lh = _mm256_mul_epu32(s1_l, rng->pcg32_mult_h);

  /* Assemble lower 32 bits, will be merged into one 256 bit vector below */
  xors0 = _mm256_permutevar8x32_epi32(xors0, shift0);
  rot0 = _mm256_permutevar8x32_epi32(rot0, shift0);
  xors1 = _mm256_permutevar8x32_epi32(xors1, shift1);
  rot1 = _mm256_permutevar8x32_epi32(rot1, shift1);

  /* Continue with partial products */
  __m256i m0_ll = _mm256_mul_epu32(s0_l, rng->pcg32_mult_l);
  __m256i m1_ll = _mm256_mul_epu32(s1_l, rng->pcg32_mult_l);

  __m256i m0h = _mm256_add_epi64(m0_hl, m0_lh);
  __m256i m1h = _mm256_add_epi64(m1_hl, m1_lh);

  __m256i m0hs = _mm256_slli_epi64(m0h, 32);
  __m256i m1hs = _mm256_slli_epi64(m1h, 32);

  __m256i s0n = _mm256_add_epi64(m0hs, m0_ll);
  __m256i s1n = _mm256_add_epi64(m1hs, m1_ll);

  __m256i xors = _mm256_or_si256(xors0, xors1);
  __m256i rot = _mm256_or_si256(rot0, rot1);

  rng->state[0] = _mm256_add_epi64(s0n, rng->inc[0]);
  rng->state[1] = _mm256_add_epi64(s1n, rng->inc[1]);

  /* Finally, rotate and return the result */
  __m256i result =
      _mm256_or_si256(_mm256_srlv_epi32(xors, rot),
                      _mm256_sllv_epi32(xors, _mm256_sub_epi32(const32, rot)));

  return result;
}

typedef struct avx256_pcg_state_setseq_64 { // Internals are *Private*.
  __m256i state; // (8x64bits) RNG state.  All values are possible.
  __m256i inc;   // (8x64bits)Controls which RNG sequences (stream) is
                 // selected. Must *always* be odd. You probably want
                 // distinct sequences
  __m256i
      pcg32_mult_l; // set to _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) &
                    // 0xffffffff)
  __m256i
      pcg32_mult_h; // set to _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) >>
                    // 32)

} avx256_pcg32_random_t;

// untested
static inline __m256i hacked_mm256_rorv_epi32(__m256i x, __m256i r) {
  return _mm256_or_si256(
      _mm256_sllv_epi32(x, _mm256_sub_epi32(_mm256_set1_epi32(32), r)),
      _mm256_srlv_epi32(x, r));
}

// untested
static inline __m256i hacked_mm256_mullo_epi64(__m256i x, __m256i ml,
                                               __m256i mh) {
  __m256i xl =
      _mm256_and_si256(x, _mm256_set1_epi64x(UINT64_C(0x00000000ffffffff)));
  __m256i xh = _mm256_srli_epi64(x, 32);
  __m256i hl = _mm256_slli_epi64(_mm256_mul_epu32(xh, ml), 32);
  __m256i lh = _mm256_slli_epi64(_mm256_mul_epu32(xl, mh), 32);
  __m256i ll = _mm256_mul_epu32(xl, ml);
  return _mm256_add_epi64(ll, _mm256_add_epi64(hl, lh));
}

static inline __m128i avx256_pcg32_random_r(avx256_pcg32_random_t *rng) {
  __m256i oldstate = rng->state;
  rng->state =
      _mm256_add_epi64(hacked_mm256_mullo_epi64(rng->state, rng->pcg32_mult_l,
                                                rng->pcg32_mult_h),
                       rng->inc);
  __m256i xorshifted = _mm256_srli_epi64(
      _mm256_xor_si256(_mm256_srli_epi64(oldstate, 18), oldstate), 27);
  __m256i rot = _mm256_srli_epi64(oldstate, 59);
  return _mm256_castsi256_si128(
      _mm256_permutevar8x32_epi32(hacked_mm256_rorv_epi32(xorshifted, rot),
                                  _mm256_set_epi32(7, 7, 7, 7, 6, 4, 2, 0)));
}
#endif

#if defined(__AVX512F__) && defined(__AVX512DQ__)
#define AVX512PCG
typedef struct avx512_pcg_state_setseq_64 { // Internals are *Private*.
  __m512i state;      // (8x64bits) RNG state.  All values are possible.
  __m512i inc;        // (8x64bits)Controls which RNG sequences (stream) is
                      // selected. Must *always* be odd. You probably want
                      // distinct sequences
  __m512i multiplier; // set to _mm512_set1_epi64(0x5851f42d4c957f2d);
} avx512_pcg32_random_t;

static inline __m256i avx512_pcg32_random_r(avx512_pcg32_random_t *rng) {
  __m512i oldstate = rng->state;
  rng->state = _mm512_add_epi64(_mm512_mullo_epi64(rng->multiplier, rng->state),
                                rng->inc);
  __m512i xorshifted = _mm512_srli_epi64(
      _mm512_xor_epi64(_mm512_srli_epi64(oldstate, 18), oldstate), 27);
  __m512i rot = _mm512_srli_epi64(oldstate, 59);
  return _mm512_cvtepi64_epi32(_mm512_rorv_epi32(xorshifted, rot));
}

typedef struct avx512bis_pcg_state_setseq_64 { // Internals are *Private*.
  __m512i state[2];   // (8x64bits) RNG state.  All values are possible.
  __m512i inc[2];     // (8x64bits)Controls which RNG sequences (stream) is
                      // selected. Must *always* be odd. You probably want
                      // distinct sequences
  __m512i multiplier; // set to _mm512_set1_epi64(0x5851f42d4c957f2d);
} avx512bis_pcg32_random_t;

static inline __m512i
avx512bis_pcg32_random_r(avx512bis_pcg32_random_t *rng) {
  __m512i oldstate0 = rng->state[0];
  __m512i oldstate1 = rng->state[1];

  rng->state[0] = _mm512_add_epi64(
      _mm512_mullo_epi64(rng->multiplier, rng->state[0]), rng->inc[0]);
  rng->state[1] = _mm512_add_epi64(
      _mm512_mullo_epi64(rng->multiplier, rng->state[1]), rng->inc[1]);

  __m512i xorshifted0 = _mm512_srli_epi64(
      _mm512_xor_epi64(_mm512_srli_epi64(oldstate0, 18), oldstate0), 27);
  __m512i rot0 = _mm512_srli_epi64(oldstate0, 59);
  __m512i xorshifted1 = _mm512_srli_epi64(
      _mm512_xor_epi64(_mm512_srli_epi64(oldstate1, 18), oldstate1), 27);
  __m512i rot1 = _mm512_srli_epi64(oldstate1, 59);
  return _mm512_inserti32x8(
      _mm512_castsi256_si512(
          _mm512_cvtepi64_epi32(_mm512_rorv_epi32(xorshifted0, rot0))),
      _mm512_cvtepi64_epi32(_mm512_rorv_epi32(xorshifted1, rot1)), 1);
}
#endif

#endif
