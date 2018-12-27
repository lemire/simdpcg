#include "simdpcg32.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
void printafew() {
  // create a new key
  avx512_pcg32_random_t key = {
      .state = _mm512_set_epi64(0xb5f380a45f908741, 0x88b545898d45385d,
                                0xd81c7fe764f8966c, 0x44a9a3b6b119e7bc,
                                0x3cb6e04dc22f629, 0x727947debc931183,
                                0xfbfa8fdcff91891f, 0xb9384fd8f34c0f49),
      .inc = _mm512_set_epi64(0xbf2de0670ac3d03e, 0x98c40c0dc94e71e,
                              0xf3565f35a8c61d00, 0xd3c83e29b30df640,
                              0x14b7f6e4c89630fa, 0x37cc7b0347694551,
                              0x4a052322d95d485b, 0x10f3ade77a26e15e),
      .multiplier = _mm512_set_epi64(0x4fdf9bc35712f7b3, 0xd398452268e95263,
                                     0x11921fe50350bc27, 0xa86b2993096c0c47,
                                     0xb79a40ffa4a1c1e7, 0x1facade8ed066211,
                                     0x4507894c307ff2e5, 0x71c9b5465a547075)};
  __m256i randomstuff = avx512_pcg32_random_r(&key);
  uint32_t buffer[8];
  _mm256_storeu_si256((__m256i *)(buffer), randomstuff);
  printf("printafew:  %x %x %x %x %x %x %x %x\n", buffer[0], buffer[1],
         buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7]);
}
// should check that it is a Binomial distribution
void check_uniform() {
  // create a new key
  avx512_pcg32_random_t key = {
      .state = _mm512_set_epi64(0xb5f380a45f908741, 0x88b545898d45385d,
                                0xd81c7fe764f8966c, 0x44a9a3b6b119e7bc,
                                0x3cb6e04dc22f629, 0x727947debc931183,
                                0xfbfa8fdcff91891f, 0xb9384fd8f34c0f49),
      .inc = _mm512_set_epi64(0xbf2de0670ac3d03e, 0x98c40c0dc94e71e,
                              0xf3565f35a8c61d00, 0xd3c83e29b30df640,
                              0x14b7f6e4c89630fa, 0x37cc7b0347694551,
                              0x4a052322d95d485b, 0x10f3ade77a26e15e),
      .multiplier = _mm512_set_epi64(0x4fdf9bc35712f7b3, 0xd398452268e95263,
                                     0x11921fe50350bc27, 0xa86b2993096c0c47,
                                     0xb79a40ffa4a1c1e7, 0x1facade8ed066211,
                                     0x4507894c307ff2e5, 0x71c9b5465a547075)};
  size_t *bitset1 = (uint64_t *)malloc((1 << 16) * sizeof(size_t));
  size_t *bitset2 = (uint64_t *)malloc((1 << 16) * sizeof(size_t));
  memset(bitset1, 0, (1 << 16) * sizeof(size_t));
  memset(bitset2, 0, (1 << 16) * sizeof(size_t));
  size_t N = 10000000;
  for (size_t t = 0; t < N; t++) {
    __m256i randomstuff = avx512_pcg32_random_r(&key);
    uint32_t buffer[8];
    _mm256_storeu_si256((__m256i *)(buffer), randomstuff);
    for (size_t j = 0; j < 8; j++) {
      uint16_t low = buffer[j] & 0xFFFF;
      uint16_t high = (buffer[j] >> 16) & 0xFFFF;
      bitset1[low]++;
      bitset2[high]++;
    }
  }
  float mean = N * 8 * 1.0 / (1 << 16);
  float mystderr = sqrt(mean); // approximate
  // we count...
  printf("with high probability, all counts should be between %f and %f \n",
         mean - 5 * mystderr, mean + 5 * mystderr);
  for (size_t c = 0; c < (1 << 16); c++) {
    size_t empty1 = 0;
    size_t empty2 = 0;
    for (size_t j = 0; j < (1 << 16); j++) {
      if (bitset1[j] == c)
        empty1++;
      if (bitset2[j] == c)
        empty2++;
    }
    if ((empty1 != 0) && (empty2 != 0)) {
      if ((c < mean - 5 * mystderr) || (c > mean + 5 * mystderr)) {
        printf("%zu : %zu %zu \n", c, empty1, empty2);
        abort();
      }
    }
  }
  printf("looks ok\n");
  free(bitset1);
  free(bitset2);
}

int main() {
  printafew();
  check_uniform();
  return EXIT_SUCCESS;
}
