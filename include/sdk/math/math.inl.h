#include <sdk/math.h>

/** @brief An approximate representation of PI. */
#define K_PI 3.14159265358979323846f

u64 math_next2pow(u64 number) {
  if (!number)
    return 1;
  --number;
  static const u8 u64_bits_length = sizeof(u64) * 8; // 64
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
i64 math_ceil(f64 number) {
  i64 integer = (i64)number;
  if (number > 0 && number != integer) {
    ++integer;
  }
  return integer;
}
i64 math_round(f64 number) {
  i64 integer = number;
  f64 decimals = number - integer;
  if (decimals < .5) {
    return integer;
  }
  return integer + 1;
}