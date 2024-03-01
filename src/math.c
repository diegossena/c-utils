#include "sdk/math.h"

const u8 u64_bits_length = sizeof(u64) * 8; // 64

u64 math_next2pow(u64 number) {
  if (!number)
    return 1;
  --number;
  for (u64 i = 1; i < u64_bits_length; i *= 2)
    number |= number >> i;
  return ++number;
}

u64 math_jenkins_hash(const byte* data, u64 size) {
  u64 hash = 0;
  const byte* end = data + size;
  while (data != end) {
    hash += *data++;
    hash += hash << 10;
    hash ^= hash >> 6;
  }
  hash += hash << 3;
  hash ^= hash >> 11;
  hash += hash << 15;
  return hash;
}