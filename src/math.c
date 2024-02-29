#include "sdk/math.h"

const u8 u64_bits_length = sizeof(u64) * 8; // 64

u64 math_closest_2pow(u64 number) {
  if (!number)
    return 1;
  --number;
  for (u64 i = 1; i < u64_bits_length; i *= 2)
    number |= number >> i;
  return ++number;
}