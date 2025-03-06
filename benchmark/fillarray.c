#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pcg32.h"
#include "simdpcg32.h"

static uint32_t counter;
void populateRandom_pcg32(uint32_t *answer, uint32_t size) {
  pcg32_random_t key = {
      .state = 324,
      .inc = 15}; // I am a crazy man using bleeding-edge C99 in 2018
  for (uint32_t i = 0; i < size; i++) {
    answer[i] = pcg32_random_r(&key);
  }
  counter += answer[size - 1];
}
#ifdef AVX2PCG
void populateRandom_avx2_pcg32(uint32_t *answer, uint32_t size) {
  uint32_t i = 0;
  avx2_pcg32_random_t key = {
      .state = {_mm256_set1_epi64x(1111), _mm256_set1_epi64x(1111)},
      .inc = {_mm256_set_epi64x(15, 13, 11, 9), _mm256_set_epi64x(7, 5, 3, 1)},
      .pcg32_mult_l =
          _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) & 0xffffffff),
      .pcg32_mult_h = _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) >> 32)};
  if (size >= 8) {
    for (; i < size - 8; i += 8) {
      __m256i r = avx2_pcg32_random_r(&key);
      _mm256_storeu_si256((__m256i *)(answer + i), r);
    }
  }
  if (i < size) {
    __m256i r = avx2_pcg32_random_r(&key);
    uint32_t buffer[8];
    _mm256_storeu_si256((__m256i *)buffer, r);
    memcpy(answer + i, buffer, sizeof(uint32_t) * (size - i));
  }
  counter += answer[size - 1];
}

void populateRandom_avx2_pcg32_two(uint32_t *answer, uint32_t size) {
  uint32_t i = 0;
  avx2_pcg32_random_t key1 = {
      .state = {_mm256_set1_epi64x(1111), _mm256_set1_epi64x(1111)},
      .inc = {_mm256_set_epi64x(15, 13, 11, 9), _mm256_set_epi64x(7, 5, 3, 1)},
      .pcg32_mult_l =
          _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) & 0xffffffff),
      .pcg32_mult_h = _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) >> 32)};
  avx2_pcg32_random_t key2 = {
      .state = {_mm256_set1_epi64x(2222), _mm256_set1_epi64x(1111)},
      .inc = {_mm256_set_epi64x(15, 13, 11, 9), _mm256_set_epi64x(7, 5, 3, 1)},
      .pcg32_mult_l =
          _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) & 0xffffffff),
      .pcg32_mult_h = _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) >> 32)};
  if (size >= 16) {
    for (; i < size - 16; i += 16) {
      __m256i r1 = avx2_pcg32_random_r(&key1);
      __m256i r2 = avx2_pcg32_random_r(&key2);
      _mm256_storeu_si256((__m256i *)(answer + i), r1);
      _mm256_storeu_si256((__m256i *)(answer + i + 8), r2);
    }
  }
  if (size - i >= 8) {
    __m256i r = avx2_pcg32_random_r(&key1);
    _mm256_storeu_si256((__m256i *)(answer + i), r);
    i += 8;
  }
  if (i < size) {
    __m256i r = avx2_pcg32_random_r(&key1);
    uint32_t buffer[8];
    _mm256_storeu_si256((__m256i *)buffer, r);
    memcpy(answer + i, buffer, sizeof(uint32_t) * (size - i));
  }
  counter += answer[size - 1];
}

void populateRandom_avx2_pcg32_four(uint32_t *answer, uint32_t size) {
  uint32_t i = 0;
  avx2_pcg32_random_t key1 = {
      .state = {_mm256_set1_epi64x(1111), _mm256_set1_epi64x(1111)},
      .inc = {_mm256_set_epi64x(15, 13, 11, 9), _mm256_set_epi64x(7, 5, 3, 1)},
      .pcg32_mult_l =
          _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) & 0xffffffff),
      .pcg32_mult_h = _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) >> 32)};
  avx2_pcg32_random_t key2 = {
      .state = {_mm256_set1_epi64x(2222), _mm256_set1_epi64x(1111)},
      .inc = {_mm256_set_epi64x(15, 13, 11, 9), _mm256_set_epi64x(7, 5, 3, 1)},
      .pcg32_mult_l =
          _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) & 0xffffffff),
      .pcg32_mult_h = _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) >> 32)};
  avx2_pcg32_random_t key3 = {
      .state = {_mm256_set1_epi64x(3333), _mm256_set1_epi64x(1111)},
      .inc = {_mm256_set_epi64x(15, 13, 11, 9), _mm256_set_epi64x(7, 5, 3, 1)},
      .pcg32_mult_l =
          _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) & 0xffffffff),
      .pcg32_mult_h = _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) >> 32)};
  avx2_pcg32_random_t key4 = {
      .state = {_mm256_set1_epi64x(4444), _mm256_set1_epi64x(1111)},
      .inc = {_mm256_set_epi64x(15, 13, 11, 9), _mm256_set_epi64x(7, 5, 3, 1)},
      .pcg32_mult_l =
          _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) & 0xffffffff),
      .pcg32_mult_h = _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) >> 32)};
  if (size >= 32) {
    for (; i < size - 32; i += 32) {
      __m256i r1 = avx2_pcg32_random_r(&key1);
      __m256i r2 = avx2_pcg32_random_r(&key2);
      __m256i r3 = avx2_pcg32_random_r(&key3);
      __m256i r4 = avx2_pcg32_random_r(&key4);
      _mm256_storeu_si256((__m256i *)(answer + i), r1);
      _mm256_storeu_si256((__m256i *)(answer + i + 8), r2);
      _mm256_storeu_si256((__m256i *)(answer + i + 16), r3);
      _mm256_storeu_si256((__m256i *)(answer + i + 24), r4);
    }
  }
  if (size >= 16) {
    for (; i < size - 16; i += 16) {
      __m256i r1 = avx2_pcg32_random_r(&key1);
      __m256i r2 = avx2_pcg32_random_r(&key2);
      _mm256_storeu_si256((__m256i *)(answer + i), r1);
      _mm256_storeu_si256((__m256i *)(answer + i + 8), r2);
    }
  }
  if (size - i >= 8) {
    __m256i r = avx2_pcg32_random_r(&key1);
    _mm256_storeu_si256((__m256i *)(answer + i), r);
    i += 8;
  }
  if (i < size) {
    __m256i r = avx2_pcg32_random_r(&key1);
    uint32_t buffer[8];
    _mm256_storeu_si256((__m256i *)buffer, r);
    memcpy(answer + i, buffer, sizeof(uint32_t) * (size - i));
  }
  counter += answer[size - 1];
}

void populateRandom_avx256_pcg32(uint32_t *answer, uint32_t size) {
  uint32_t i = 0;
  avx256_pcg32_random_t key = {
      .state = _mm256_set1_epi64x(1111),
      .inc = _mm256_set_epi64x(15, 13, 11, 9),
      .pcg32_mult_l =
          _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) & 0xffffffff),
      .pcg32_mult_h = _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) >> 32)};
  if (size >= 4) {
    for (; i < size - 4; i += 4) {
      __m128i r = avx256_pcg32_random_r(&key);
      _mm_storeu_si128((__m128i *)(answer + i), r);
    }
  }
  if (i < size) {
    __m128i r = avx256_pcg32_random_r(&key);
    uint32_t buffer[4];
    _mm_storeu_si128((__m128i *)buffer, r);
    memcpy(answer + i, buffer, sizeof(uint32_t) * (size - i));
  }
  counter += answer[size - 1];
}

void populateRandom_avx256_pcg32_two(uint32_t *answer, uint32_t size) {
  uint32_t i = 0;
  avx256_pcg32_random_t key1 = {
      .state = _mm256_set1_epi64x(1111),
      .inc = _mm256_set_epi64x(15, 13, 11, 9),
      .pcg32_mult_l =
          _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) & 0xffffffff),
      .pcg32_mult_h = _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) >> 32)};
  avx256_pcg32_random_t key2 = {
      .state = _mm256_set1_epi64x(2222),
      .inc = _mm256_set_epi64x(7, 5, 3, 1),
      .pcg32_mult_l =
          _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) & 0xffffffff),
      .pcg32_mult_h = _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) >> 32)};
  if (size >= 8) {
    for (; i < size - 8; i += 8) {
      __m128i r1 = avx256_pcg32_random_r(&key1);
      __m128i r2 = avx256_pcg32_random_r(&key2);
      _mm_storeu_si128((__m128i *)(answer + i), r1);
      _mm_storeu_si128((__m128i *)(answer + i + 4), r2);
    }
  }
  if (size - i >= 4) {
    __m128i r = avx256_pcg32_random_r(&key1);
    _mm_storeu_si128((__m128i *)(answer + i), r);
    i += 4;
  }
  if (i < size) {
    __m128i r = avx256_pcg32_random_r(&key1);
    uint32_t buffer[8];
    _mm_storeu_si128((__m128i *)buffer, r);
    memcpy(answer + i, buffer, sizeof(uint32_t) * (size - i));
  }
  counter += answer[size - 1];
}

void populateRandom_avx256_pcg32_four(uint32_t *answer, uint32_t size) {
  uint32_t i = 0;
  avx256_pcg32_random_t key1 = {
      .state = _mm256_set1_epi64x(1111),
      .inc = _mm256_set_epi64x(15, 13, 11, 9),
      .pcg32_mult_l =
          _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) & 0xffffffff),
      .pcg32_mult_h = _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) >> 32)};
  avx256_pcg32_random_t key2 = {
      .state = _mm256_set1_epi64x(2222),
      .inc = _mm256_set_epi64x(15, 13, 11, 9),
      .pcg32_mult_l =
          _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) & 0xffffffff),
      .pcg32_mult_h = _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) >> 32)};
  avx256_pcg32_random_t key3 = {
      .state = _mm256_set1_epi64x(3333),
      .inc = _mm256_set_epi64x(15, 13, 11, 9),
      .pcg32_mult_l =
          _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) & 0xffffffff),
      .pcg32_mult_h = _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) >> 32)};
  avx256_pcg32_random_t key4 = {
      .state = _mm256_set1_epi64x(4444),
      .inc = _mm256_set_epi64x(15, 13, 11, 9),
      .pcg32_mult_l =
          _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) & 0xffffffff),
      .pcg32_mult_h = _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) >> 32)};
  if (size >= 16) {
    for (; i < size - 16; i += 16) {
      __m128i r1 = avx256_pcg32_random_r(&key1);
      __m128i r2 = avx256_pcg32_random_r(&key2);
      __m128i r3 = avx256_pcg32_random_r(&key3);
      __m128i r4 = avx256_pcg32_random_r(&key4);
      _mm_storeu_si128((__m128i *)(answer + i), r1);
      _mm_storeu_si128((__m128i *)(answer + i + 4), r2);
      _mm_storeu_si128((__m128i *)(answer + i + 8), r3);
      _mm_storeu_si128((__m128i *)(answer + i + 12), r4);
    }
  }
  if (size >= 8) {
    for (; i < size - 8; i += 8) {
      __m128i r1 = avx256_pcg32_random_r(&key1);
      __m128i r2 = avx256_pcg32_random_r(&key2);
      _mm_storeu_si128((__m128i *)(answer + i), r1);
      _mm_storeu_si128((__m128i *)(answer + i + 4), r2);
    }
  }
  if (size - i >= 4) {
    __m128i r = avx256_pcg32_random_r(&key1);
    _mm_storeu_si128((__m128i *)(answer + i), r);
    i += 8;
  }
  if (i < size) {
    __m128i r = avx256_pcg32_random_r(&key1);
    uint32_t buffer[4];
    _mm_storeu_si128((__m128i *)buffer, r);
    memcpy(answer + i, buffer, sizeof(uint32_t) * (size - i));
  }
  counter += answer[size - 1];
}
#endif

#ifdef AVX512PCG

void populateRandom_avx512_pcg32(uint32_t *answer, uint32_t size) {
  uint32_t i = 0;
  avx512_pcg32_random_t key = {
      .state = _mm512_set1_epi64(1111),
      .inc = _mm512_set_epi64(15, 13, 11, 9, 7, 5, 3, 1),
      .multiplier = _mm512_set1_epi64(0x5851f42d4c957f2d)};
  if (size >= 8) {
    for (; i < size - 8; i += 8) {
      __m256i r = avx512_pcg32_random_r(&key);
      _mm256_storeu_si256((__m256i *)(answer + i), r);
    }
  }
  if (i < size) {
    __m256i r = avx512_pcg32_random_r(&key);
    uint32_t buffer[8];
    _mm256_storeu_si256((__m256i *)buffer, r);
    memcpy(answer + i, buffer, sizeof(uint32_t) * (size - i));
  }
  counter += answer[size - 1];
}

void populateRandom_avx512bis_pcg32(uint32_t *answer, uint32_t size) {
  uint32_t i = 0;
  avx512bis_pcg32_random_t key = {
      .state = {_mm512_set1_epi64(1111), _mm512_set1_epi64(2222)},
      .inc = {_mm512_set_epi64(15, 13, 11, 9, 7, 5, 3, 1),
              _mm512_set_epi64(155, 135, 115, 95, 75, 55, 35, 15)},
      .multiplier = _mm512_set1_epi64(0x5851f42d4c957f2d)};
  if (size >= 16) {
    for (; i < size - 16; i += 16) {
      __m512i r = avx512bis_pcg32_random_r(&key);
      _mm512_storeu_si512((__m512i *)(answer + i), r);
    }
  }
  if (i < size) {
    __m512i r = avx512bis_pcg32_random_r(&key);
    uint32_t buffer[16];
    _mm512_storeu_si512((__m512i *)buffer, r);
    memcpy(answer + i, buffer, sizeof(uint32_t) * (size - i));
  }
  counter += answer[size - 1];
}

void populateRandom_avx512bis_pcg32_two(uint32_t *answer, uint32_t size) {
  uint32_t i = 0;
  avx512bis_pcg32_random_t key1 = {
      .state = {_mm512_set1_epi64(1111), _mm512_set1_epi64(2222)},
      .inc = {_mm512_set_epi64(15, 13, 11, 9, 7, 5, 3, 1),
              _mm512_set_epi64(155, 135, 115, 95, 75, 55, 35, 15)},
      .multiplier = _mm512_set1_epi64(0x5851f42d4c957f2d)};
  avx512bis_pcg32_random_t key2 = {
      .state = {_mm512_set1_epi64(3333), _mm512_set1_epi64(4444)},
      .inc = {_mm512_set_epi64(15, 13, 11, 9, 7, 5, 3, 1),
              _mm512_set_epi64(155, 135, 115, 95, 75, 55, 35, 15)},
      .multiplier = _mm512_set1_epi64(0x5851f42d4c957f2d)};
  if (size >= 32) {
    for (; i < size - 32; i += 32) {
      __m512i r1 = avx512bis_pcg32_random_r(&key1);
      __m512i r2 = avx512bis_pcg32_random_r(&key2);
      _mm512_storeu_si512((__m512i *)(answer + i), r1);
      _mm512_storeu_si512((__m512i *)(answer + i + 16), r2);
    }
  }
  if (size >= 16) {
    for (; i < size - 16; i += 16) {
      __m512i r = avx512bis_pcg32_random_r(&key1);
      _mm512_storeu_si512((__m512i *)(answer + i), r);
    }
  }
  if (i < size) {
    __m512i r = avx512bis_pcg32_random_r(&key1);
    uint32_t buffer[16];
    _mm512_storeu_si512((__m512i *)buffer, r);
    memcpy(answer + i, buffer, sizeof(uint32_t) * (size - i));
  }
  counter += answer[size - 1];
}

void populateRandom_avx512bis_pcg32_four(uint32_t *answer, uint32_t size) {
  uint32_t i = 0;
  avx512bis_pcg32_random_t key1 = {
      .state = {_mm512_set1_epi64(1111), _mm512_set1_epi64(2222)},
      .inc = {_mm512_set_epi64(15, 13, 11, 9, 7, 5, 3, 1),
              _mm512_set_epi64(155, 135, 115, 95, 75, 55, 35, 15)},
      .multiplier = _mm512_set1_epi64(0x5851f42d4c957f2d)};
  avx512bis_pcg32_random_t key2 = {
      .state = {_mm512_set1_epi64(3333), _mm512_set1_epi64(4444)},
      .inc = {_mm512_set_epi64(15, 13, 11, 9, 7, 5, 3, 1),
              _mm512_set_epi64(155, 135, 115, 95, 75, 55, 35, 15)},
      .multiplier = _mm512_set1_epi64(0x5851f42d4c957f2d)};
  avx512bis_pcg32_random_t key3 = {
      .state = {_mm512_set1_epi64(5555), _mm512_set1_epi64(6666)},
      .inc = {_mm512_set_epi64(15, 13, 11, 9, 7, 5, 3, 1),
              _mm512_set_epi64(155, 135, 115, 95, 75, 55, 35, 15)},
      .multiplier = _mm512_set1_epi64(0x5851f42d4c957f2d)};
  avx512bis_pcg32_random_t key4 = {
      .state = {_mm512_set1_epi64(7777), _mm512_set1_epi64(8888)},
      .inc = {_mm512_set_epi64(15, 13, 11, 9, 7, 5, 3, 1),
              _mm512_set_epi64(155, 135, 115, 95, 75, 55, 35, 15)},
      .multiplier = _mm512_set1_epi64(0x5851f42d4c957f2d)};
  if (size >= 64) {
    for (; i < size - 64; i += 64) {
      __m512i r1 = avx512bis_pcg32_random_r(&key1);
      __m512i r2 = avx512bis_pcg32_random_r(&key2);
      __m512i r3 = avx512bis_pcg32_random_r(&key3);
      __m512i r4 = avx512bis_pcg32_random_r(&key4);
      _mm512_storeu_si512((__m512i *)(answer + i), r1);
      _mm512_storeu_si512((__m512i *)(answer + i + 16), r2);
      _mm512_storeu_si512((__m512i *)(answer + i + 32), r3);
      _mm512_storeu_si512((__m512i *)(answer + i + 48), r4);
    }
  }
  if (size >= 32) {
    for (; i < size - 32; i += 32) {
      __m512i r1 = avx512bis_pcg32_random_r(&key1);
      __m512i r2 = avx512bis_pcg32_random_r(&key2);
      _mm512_storeu_si512((__m512i *)(answer + i), r1);
      _mm512_storeu_si512((__m512i *)(answer + i + 16), r2);
    }
  }
  if (size >= 16) {
    for (; i < size - 16; i += 16) {
      __m512i r = avx512bis_pcg32_random_r(&key1);
      _mm512_storeu_si512((__m512i *)(answer + i), r);
    }
  }
  if (i < size) {
    __m512i r = avx512bis_pcg32_random_r(&key1);
    uint32_t buffer[16];
    _mm512_storeu_si512((__m512i *)buffer, r);
    memcpy(answer + i, buffer, sizeof(uint32_t) * (size - i));
  }
  counter += answer[size - 1];
}
void populateRandom_avx512_pcg32_two(uint32_t *answer, uint32_t size) {
  uint32_t i = 0;
  avx512_pcg32_random_t key1 = {
      .state = _mm512_set1_epi64(1111),
      .inc = _mm512_set_epi64(15, 13, 11, 9, 7, 5, 3, 1),
      .multiplier = _mm512_set1_epi64(0x5851f42d4c957f2d)};
  avx512_pcg32_random_t key2 = {
      .state = _mm512_set1_epi64(1111222),
      .inc = _mm512_set_epi64(15, 13, 11, 9, 7, 5, 3, 1),
      .multiplier = _mm512_set1_epi64(0x5851f42d4c957f2d)};
  if (size >= 16) {
    for (; i < size - 16; i += 16) {
      __m256i r1 = avx512_pcg32_random_r(&key1);
      __m256i r2 = avx512_pcg32_random_r(&key2);
      _mm256_storeu_si256((__m256i *)(answer + i), r1);
      _mm256_storeu_si256((__m256i *)(answer + i + 8), r2);
    }
  }
  if (size - i >= 8) {
    __m256i r = avx512_pcg32_random_r(&key1);
    _mm256_storeu_si256((__m256i *)(answer + i), r);
    i += 8;
  }
  if (i < size) {
    __m256i r = avx512_pcg32_random_r(&key1);
    uint32_t buffer[8];
    _mm256_storeu_si256((__m256i *)buffer, r);
    memcpy(answer + i, buffer, sizeof(uint32_t) * (size - i));
  }
  counter += answer[size - 1];
}

void populateRandom_avx512_pcg32_four(uint32_t *answer, uint32_t size) {
  uint32_t i = 0;
  avx512_pcg32_random_t key1 = {
      .state = _mm512_set1_epi64(1111),
      .inc = _mm512_set_epi64(15, 13, 11, 9, 7, 5, 3, 1),
      .multiplier = _mm512_set1_epi64(0x5851f42d4c957f2d)};
  avx512_pcg32_random_t key2 = {
      .state = _mm512_set1_epi64(1111222),
      .inc = _mm512_set_epi64(15, 13, 11, 9, 7, 5, 3, 1),
      .multiplier = _mm512_set1_epi64(0x5851f42d4c957f2d)};
  avx512_pcg32_random_t key3 = {
      .state = _mm512_set1_epi64(111133333),
      .inc = _mm512_set_epi64(15, 13, 11, 9, 7, 5, 3, 1),
      .multiplier = _mm512_set1_epi64(0x5851f42d4c957f2d)};
  avx512_pcg32_random_t key4 = {
      .state = _mm512_set1_epi64(1111444444),
      .inc = _mm512_set_epi64(15, 13, 11, 9, 7, 5, 3, 1),
      .multiplier = _mm512_set1_epi64(0x5851f42d4c957f2d)};
  if (size >= 32) {
    for (; i < size - 32; i += 32) {
      __m256i r1 = avx512_pcg32_random_r(&key1);
      __m256i r2 = avx512_pcg32_random_r(&key2);
      __m256i r3 = avx512_pcg32_random_r(&key3);
      __m256i r4 = avx512_pcg32_random_r(&key4);
      _mm256_storeu_si256((__m256i *)(answer + i), r1);
      _mm256_storeu_si256((__m256i *)(answer + i + 8), r2);
      _mm256_storeu_si256((__m256i *)(answer + i + 16), r3);
      _mm256_storeu_si256((__m256i *)(answer + i + 24), r4);
    }
  }
  if (size >= 16) {
    for (; i < size - 16; i += 16) {
      __m256i r1 = avx512_pcg32_random_r(&key1);
      __m256i r2 = avx512_pcg32_random_r(&key2);
      _mm256_storeu_si256((__m256i *)(answer + i), r1);
      _mm256_storeu_si256((__m256i *)(answer + i + 8), r2);
    }
  }
  if (size - i >= 8) {
    __m256i r = avx512_pcg32_random_r(&key1);
    _mm256_storeu_si256((__m256i *)(answer + i), r);
    i += 8;
  }
  if (i < size) {
    __m256i r = avx512_pcg32_random_r(&key1);
    uint32_t buffer[8];
    _mm256_storeu_si256((__m256i *)buffer, r);
    memcpy(answer + i, buffer, sizeof(uint32_t) * (size - i));
  }
  counter += answer[size - 1];
}

#endif

#define RDTSC_START(cycles)                                                    \
  do {                                                                         \
    register unsigned cyc_high, cyc_low;                                       \
    __asm volatile("cpuid\n\t"                                                 \
                   "rdtsc\n\t"                                                 \
                   "mov %%edx, %0\n\t"                                         \
                   "mov %%eax, %1\n\t"                                         \
                   : "=r"(cyc_high), "=r"(cyc_low)::"%rax", "%rbx", "%rcx",    \
                     "%rdx");                                                  \
    (cycles) = ((uint64_t)cyc_high << 32) | cyc_low;                           \
  } while (0)

#define RDTSC_FINAL(cycles)                                                    \
  do {                                                                         \
    register unsigned cyc_high, cyc_low;                                       \
    __asm volatile("rdtscp\n\t"                                                \
                   "mov %%edx, %0\n\t"                                         \
                   "mov %%eax, %1\n\t"                                         \
                   "cpuid\n\t"                                                 \
                   : "=r"(cyc_high), "=r"(cyc_low)::"%rax", "%rbx", "%rcx",    \
                     "%rdx");                                                  \
    (cycles) = ((uint64_t)cyc_high << 32) | cyc_low;                           \
  } while (0)

/*
 * Prints the best number of operations per cycle where
 * test is the function call, answer is the expected answer generated by
 * test, repeat is the number of times we should repeat and size is the
 * number of operations represented by test.
 */
#define BEST_TIME(test, pre, repeat, size)                                     \
  do {                                                                         \
    printf("%s: ", #test);                                                     \
    fflush(NULL);                                                              \
    uint64_t cycles_start, cycles_final, cycles_diff;                          \
    uint64_t min_diff = (uint64_t)-1;                                          \
    for (int i = 0; i < repeat; i++) {                                         \
      pre;                                                                     \
      __asm volatile("" ::: /* pretend to clobber */ "memory");                \
      RDTSC_START(cycles_start);                                               \
      test;                                                                    \
      RDTSC_FINAL(cycles_final);                                               \
      cycles_diff = (cycles_final - cycles_start);                             \
      if (cycles_diff < min_diff)                                              \
        min_diff = cycles_diff;                                                \
    }                                                                          \
    uint64_t S = size;                                                         \
    float cycle_per_op = (min_diff) / (double)S;                               \
    printf(" %.2f cycles per operation", cycle_per_op);                        \
    printf("\n");                                                              \
    fflush(NULL);                                                              \
  } while (0)

void demo(int size) {
  counter = 0;
  printf("Generating %d 32-bit random numbers \n", size);
  printf("Time reported in number of cycles per array element.\n");
  printf("We store values to an array of size = %lu kB.\n",
         size * sizeof(uint32_t) / (1024));
  int repeat = 500;
  uint32_t *prec = malloc(size * sizeof(uint32_t));
  printf("\nWe just generate the random numbers: \n");
  BEST_TIME(populateRandom_pcg32(prec, size), , repeat, size);
#ifdef AVX2PCG
  BEST_TIME(populateRandom_avx2_pcg32(prec, size), , repeat, size);
  BEST_TIME(populateRandom_avx2_pcg32_two(prec, size), , repeat, size);
  BEST_TIME(populateRandom_avx2_pcg32_four(prec, size), , repeat, size);
  BEST_TIME(populateRandom_avx256_pcg32(prec, size), , repeat, size);
  BEST_TIME(populateRandom_avx256_pcg32_two(prec, size), , repeat, size);
  BEST_TIME(populateRandom_avx256_pcg32_four(prec, size), , repeat, size);
#else
  printf("\nAVX2 not enabled!\n");
#endif

#ifdef AVX512PCG
  BEST_TIME(populateRandom_avx512_pcg32(prec, size), , repeat, size);
  BEST_TIME(populateRandom_avx512_pcg32_two(prec, size), , repeat, size);
  BEST_TIME(populateRandom_avx512_pcg32_four(prec, size), , repeat, size);
  BEST_TIME(populateRandom_avx512bis_pcg32(prec, size), , repeat, size);
  BEST_TIME(populateRandom_avx512bis_pcg32_two(prec, size), , repeat, size);
  BEST_TIME(populateRandom_avx512bis_pcg32_four(prec, size), , repeat, size);

#else
  printf("\nAVX512 not enabled!\n");
#endif
  free(prec);
  printf(" %d \n", (int)counter);
}

int main() {
  demo(50000);
  return 0;
}
