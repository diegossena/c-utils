#include "sdk/math.h"

const u8 u64_bits_length = sizeof(u64) * 8; // 64

f32 math_min_f32(f32 a, f32 b) { return math_min(a, b); }
f32 math_max_f32(f32 a, f32 b) { return math_max(a, b); }
f32 math_clamp_f32(f32 value, f32 min, f32 max) {
  f32 higher = math_max(value, min);
  return math_min(higher, max);
}

u64 math_next2pow(u64 number) {
  if (!number)
    return 1;
  --number;
  for (u64 i = 1; i < u64_bits_length; i *= 2)
    number |= number >> i;
  return ++number;
}

u64 math_hash_jenkins(const byte* data, u64 size) {
  u64 hash = 0;
  const byte* end = data + size;
  while (data != end) {
    hash += *data;
    hash += hash << 10;
    hash ^= hash >> 6;
    ++data;
  }
  hash += hash << 3;
  hash ^= hash >> 11;
  hash += hash << 15;
  return hash;
}