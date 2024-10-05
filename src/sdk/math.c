#include <sdk/math.h>

#include <math.h>

SDK_EXPORT f64 math_log10(f64 x) {
  static const f64
    ivln10hi = 4.34294481878168880939e-01, /* 0x3fdbcb7b, 0x15200000 */
    ivln10lo = 2.50829467116452752298e-11, /* 0x3dbb9438, 0xca9aadd5 */
    log10_2hi = 3.01029995663611771306e-01, /* 0x3FD34413, 0x509F6000 */
    log10_2lo = 3.69423907715893078616e-13, /* 0x3D59FEF3, 0x11F12B36 */
    Lg1 = 6.666666666666735130e-01,  /* 3FE55555 55555593 */
    Lg2 = 3.999999999940941908e-01,  /* 3FD99999 9997FA04 */
    Lg3 = 2.857142874366239149e-01,  /* 3FD24924 94229359 */
    Lg4 = 2.222219843214978396e-01,  /* 3FCC71C5 1D8E78AF */
    Lg5 = 1.818357216161805012e-01,  /* 3FC74664 96CB03DE */
    Lg6 = 1.531383769920937332e-01,  /* 3FC39A09 D078C69F */
    Lg7 = 1.479819860511658591e-01;  /* 3FC2F112 DF3E5244 */
  union { f64 f; u64 i; } u = { x };
  f64 hfsq, f, s, z, R, w, t1, t2, dk, y, hi, lo, val_hi, val_lo;
  u32 hx;
  int k;
  hx = u.i >> 32;
  k = 0;
  if (hx < 0x00100000 || hx >> 31) {
    if (u.i << 1 == 0)
      return -1 / (x * x);  /* log(+-0)=-inf */
    if (hx >> 31)
      return (x - x) / 0.0; /* log(-#) = NaN */
    /* subnormal number, scale x up */
    k -= 54;
    x *= 0x1p54;
    u.f = x;
    hx = u.i >> 32;
  } else if (hx >= 0x7ff00000) {
    return x;
  } else if (hx == 0x3ff00000 && u.i << 32 == 0)
    return 0;
  /* reduce x into [sqrt(2)/2, sqrt(2)] */
  hx += 0x3ff00000 - 0x3fe6a09e;
  k += (int)(hx >> 20) - 0x3ff;
  hx = (hx & 0x000fffff) + 0x3fe6a09e;
  u.i = (u64)hx << 32 | (u.i & 0xffffffff);
  x = u.f;
  f = x - 1.0;
  hfsq = 0.5 * f * f;
  s = f / (2.0 + f);
  z = s * s;
  w = z * z;
  t1 = w * (Lg2 + w * (Lg4 + w * Lg6));
  t2 = z * (Lg1 + w * (Lg3 + w * (Lg5 + w * Lg7)));
  R = t2 + t1;
  /* hi+lo = f - hfsq + s*(hfsq+R) ~ log(1+f) */
  hi = f - hfsq;
  u.f = hi;
  u.i &= (u64)-1 << 32;
  hi = u.f;
  lo = f - hi - hfsq + s * (hfsq + R);
  /* val_hi+val_lo ~ log10(1+f) + k*log10(2) */
  val_hi = hi * ivln10hi;
  dk = k;
  y = dk * log10_2hi;
  val_lo = dk * log10_2lo + (lo + hi) * ivln10lo + lo * ivln10hi;
  /*
   * Extra precision in for adding y is not strictly needed
   * since there is no very large cancellation near x = sqrt(2) or
   * x = 1/sqrt(2), but we do it anyway since it costs little on CPUs
   * with some parallelism and it reduces the error for many args.
   */
  w = y + val_hi;
  val_lo += (y - w) + val_hi;
  val_hi = w;
  return val_lo + val_hi;
}

SDK_EXPORT u64 math_next2pow(u64 number) {
  if (!number)
    return 1;
  --number;
  static const u8 u64_bits_length = sizeof(u64) * 8; // 64
  for (u64 i = 1; i < u64_bits_length; i *= 2)
    number |= number >> i;
  return ++number;
}

SDK_EXPORT u64 math_hash_jenkins(const byte_t* data, u64 size) {
  u64 hash = 0;
  const byte_t* end = data + size;
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
SDK_EXPORT i64 math_ceil(f64 number) {
  i64 integer = (i64)number;
  if (number > 0 && number != integer) {
    ++integer;
  }
  return integer;
}
SDK_EXPORT f64 math_pow(f64 base, f64 exponent) {
  return pow(base, exponent);
}
SDK_EXPORT f64 math_floor(f64 number) {
  return floor(number);
}
SDK_EXPORT f64 math_floor_f64(f64 number, u8 places) {
  if (places > 0) {
    number *= math_pow(10, places);
  }
  number = math_floor(number);
  if (places > 0) {
    number *= math_pow(10, -places);
  }
  return number;
}
SDK_EXPORT i64 math_round(f64 number) {
  i64 integer = number;
  f64 decimals = number - integer;
  if (decimals < .5) {
    return integer;
  }
  return integer + 1;
}
SDK_EXPORT f64 math_round_f64(f64 number, u8 places) {
  if (places > 0) {
    number *= math_pow(10, places);
  }
  number = math_round(number);
  if (places > 0) {
    number *= math_pow(10, -places);
  }
  return number;
}
SDK_EXPORT f64 math_ceil_f64(f64 number, u8 places) {
  if (places > 0) {
    number *= math_pow(10, places);
  }
  number = math_ceil(number);
  if (places > 0) {
    number *= math_pow(10, -places);
  }
  return number;
}
SDK_EXPORT i32 math_random() {
  return rand();
}
SDK_EXPORT i32 math_random_in_range(i32 min, i32 max) {
  return (rand() % (max - min + 1)) + min;
}