#include <sdk/string.h>

#define __STRING_FORMAT__PUTC(c) \
  do { \
    if (pos < size) \
      target[pos] = (c); \
    ++pos; \
  } while (0);
enum __string_format_flag_t {
  __STRING_FORMAT__ZEROPAD = 1, // pad with zero
  __STRING_FORMAT__PLUS = 4, // unsigned/signed long
  __STRING_FORMAT__SIGN = 2, // unsigned/signed long
  __STRING_FORMAT__SPACE = 8, // space if plus
  __STRING_FORMAT__LEFT = 16, // left justified
  __STRING_FORMAT__SMALL = 32, // Must be 32 == 0x20
  __STRING_FORMAT__SPECIAL = 64, // 0x
  __STRING_FORMAT__WIDE = 128, // UTF-16 string
};
static i32 __string_format_va__get_flags(const char** format) {
  int flags = 0;
  do {
    switch (**format) {
      case '-':
        flags |= __STRING_FORMAT__LEFT;
        break;
      case '+':
        flags |= __STRING_FORMAT__PLUS;
        break;
      case ' ':
        flags |= __STRING_FORMAT__SPACE;
        break;
      case '#':
        flags |= __STRING_FORMAT__SPECIAL;
        break;
      case '0':
        flags |= __STRING_FORMAT__ZEROPAD;
        break;
      default:
        return flags;
    }
    ++(*format);
  } while (1);
}
static i32 __string_format_va__get_int(const char** format, va_list* args) {
  if (IS_DIGIT(**format))
    return string_skip_u64(format);
  if (**format == '*') {
    ++(*format);
    /* it's the next argument */
    return va_arg(*args, i32);
  }
  return 0;
}
static u64 __string_format_va__utf16s_utf8nlen(const u16* s16, u64 maxlen) {
  u64 len, clen;

  for (len = 0; len < maxlen && *s16; len += clen) {
    u16 c0 = *s16++;

    /* First, get the length for a BMP character */
    clen = 1 + (c0 >= 0x80) + (c0 >= 0x800);
    if (len + clen > maxlen)
      break;
    /*
     * If this is a high surrogate, and we're already at maxlen, we
     * can't include the character if it's a valid surrogate pair.
     * Avoid accessing one extra word just to check if it's valid
     * or not.
     */
    if ((c0 & 0xfc00) == 0xd800) {
      if (len + clen == maxlen)
        break;
      if ((*s16 & 0xfc00) == 0xdc00) {
        ++s16;
        ++clen;
      }
    }
  }

  return len;
}
/*
 * put_dec_full4 handles numbers in the range 0 <= r < 10000.
 * The multiplier 0xccd is round(2^15/10), and the approximation
 * r/10 == (r * 0xccd) >> 15 is exact for all r < 16389.
 */
static void __string_format_va__put_dec_full4(char* end, i32 r) {
  int i;
  for (i = 0; i < 3; i++) {
    i32 q = (r * 0xccd) >> 15;
    *--end = '0' + (r - q * 10);
    r = q;
  }
  *--end = '0' + r;
}
/*
 * Call put_dec_full4 on x % 10000, return x / 10000.
 * The approximation x/10000 == (x * 0x346DC5D7) >> 43
 * holds for all x < 1,128,869,999.  The largest value this
 * helper will ever be asked to convert is 1,125,520,955.
 * (second call in the put_dec code, assuming n is all-ones).
 */
static i32 __string_format_va__put_dec_helper4(char* end, i32 x) {
  i32 q = (x * 0x346DC5D7ULL) >> 43;
  __string_format_va__put_dec_full4(end, x - q * 10000);
  return q;
}
/* Based on code by Douglas W. Jones found at
 * <http://www.cs.uiowa.edu/~jones/bcd/decimal.html#sixtyfour>
 * Performs no 64-bit division and hence should be fast on 32-bit machines.
 */
static char* __string_format_va__put_dec(char* end, u64 n) {
  i32 d3, d2, d1, q, h;
  char* p = end;

  d1 = ((i32)n >> 16); /* implicit "& 0xffff" */
  h = (n >> 32);
  d2 = (h) & 0xffff;
  d3 = (h >> 16); /* implicit "& 0xffff" */

  /* n = 2^48 d3 + 2^32 d2 + 2^16 d1 + d0
       = 281_4749_7671_0656 d3 + 42_9496_7296 d2 + 6_5536 d1 + d0 */
  q = 656 * d3 + 7296 * d2 + 5536 * d1 + ((i32)n & 0xffff);
  q = __string_format_va__put_dec_helper4(p, q);
  p -= 4;

  q += 7671 * d3 + 9496 * d2 + 6 * d1;
  q = __string_format_va__put_dec_helper4(p, q);
  p -= 4;

  q += 4749 * d3 + 42 * d2;
  q = __string_format_va__put_dec_helper4(p, q);
  p -= 4;

  q += 281 * d3;
  q = __string_format_va__put_dec_helper4(p, q);
  p -= 4;

  __string_format_va__put_dec_full4(p, q);
  p -= 4;

  /* strip off the extra 0's we printed */
  while (p < end && *p == '0')
    ++p;

  return p;
}
static char* __string_format_va__number(char* end, u64 num, int base, char locase) {
  /*
   * locase = 0 or 0x20. ORing digits or letters with 'locase'
   * produces same digits or (maybe lowercased) letters
   */

  /* we are called with base 8, 10 or 16, only, thus don't need "G..."  */
  static const char digits[16] = "0123456789ABCDEF"; /* "GHIJKLMNOPQRSTUVWXYZ"; */

  switch (base) {
    case 10:
      if (num != 0)
        end = __string_format_va__put_dec(end, num);
      break;
    case 8:
      for (; num != 0; num >>= 3)
        *--end = '0' + (num & 07);
      break;
    case 16:
      for (; num != 0; num >>= 4)
        *--end = digits[num & 0xf] | locase;
      break;
    default:
      // unreachable
  }

  return end;
}
static u32 __string_format_va__utf16_to_utf32(const u16** s16) {
  u16 c0, c1;

  c0 = *(*s16)++;
  /* not a surrogate */
  if ((c0 & 0xf800) != 0xd800)
    return c0;
  /* invalid: low surrogate instead of high */
  if (c0 & 0x0400)
    return 0xfffd;
  c1 = **s16;
  /* invalid: missing low surrogate */
  if ((c1 & 0xfc00) != 0xdc00)
    return 0xfffd;
  /* valid surrogate pair */
  ++(*s16);
  return (0x10000 - (0xd800 << 10) - 0xdc00) + (c0 << 10) + c1;
}
static char __string_format_va__get_sign(long long* num, int flags) {
  if (!(flags & __STRING_FORMAT__SIGN))
    return 0;
  if (*num < 0) {
    *num = -(*num);
    return '-';
  }
  if (flags & __STRING_FORMAT__PLUS)
    return '+';
  if (flags & __STRING_FORMAT__SPACE)
    return ' ';
  return 0;
}
static u64 __string_format_va__get_number(int sign, int qualifier, va_list* ap) {
  if (sign) {
    switch (qualifier) {
      case 'L':
        return va_arg(*ap, long long);
      case 'l':
        return va_arg(*ap, long);
      case 'h':
        return (short)va_arg(*ap, int);
      case 'H':
        return (signed char)va_arg(*ap, int);
      default:
        return va_arg(*ap, int);
    };
  } else {
    switch (qualifier) {
      case 'L':
        return va_arg(*ap, u64);
      case 'l':
        return va_arg(*ap, unsigned long);
      case 'h':
        return (unsigned short)va_arg(*ap, int);
      case 'H':
        return (unsigned char)va_arg(*ap, int);
      default:
        return va_arg(*ap, i32);
    }
  }
}
export i32 string_format_va(char* target, u64 size, const char* format, va_list ap) {
  /* The maximum space required is to print a 64-bit number in octal */
  char tmp[(sizeof(u64) * 8 + 2) / 3];
  char* tmp_end = &tmp[sizeof(tmp)];
  long long num;
  int base;
  const char* s;
  u64 len, pos;
  char sign;

  int flags;		/* flags to number() */

  int field_width;	/* width of output field */
  int precision;		/* min. # of digits for integers; max
           number of chars for from string */
  int qualifier;		/* 'h', 'hh', 'l' or 'll' for integer fields */

  va_list args;

  /*
   * We want to pass our input va_list to helper functions by reference,
   * but there's an annoying edge case. If va_list was originally passed
   * to us by value, we could just pass &ap down to the helpers. This is
   * the case on, for example, X86_32.
   * However, on X86_64 (and possibly others), va_list is actually a
   * size-1 array containing a structure. Our function parameter ap has
   * decayed from T[1] to T*, and &ap has type T** rather than T(*)[1],
   * which is what will be expected by a function taking a va_list *
   * parameter.
   * One standard way to solve this mess is by creating a copy in a local
   * variable of type va_list and then passing a pointer to that local
   * copy instead, which is what we do here.
   */
  va_copy(args, ap);

  for (pos = 0; *format; ++format) {
    if (*format != '%' || *++format == '%') {
      __STRING_FORMAT__PUTC(*format);
      continue;
    }

    /* process flags */
    flags = __string_format_va__get_flags(&format);

    /* get field width */
    field_width = __string_format_va__get_int(&format, &args);
    if (field_width < 0) {
      field_width = -field_width;
      flags |= __STRING_FORMAT__LEFT;
    }

    if (flags & __STRING_FORMAT__LEFT)
      flags &= ~__STRING_FORMAT__ZEROPAD;

    /* get the precision */
    precision = -1;
    if (*format == '.') {
      ++format;
      precision = __string_format_va__get_int(&format, &args);
      if (precision >= 0)
        flags &= ~__STRING_FORMAT__ZEROPAD;
    }

    /* get the conversion qualifier */
    qualifier = -1;
    if (*format == 'h' || *format == 'l') {
      qualifier = *format;
      ++format;
      if (qualifier == *format) {
        qualifier -= 'a' - 'A';
        ++format;
      }
    }

    sign = 0;

    switch (*format) {
      case 'c':
        flags &= __STRING_FORMAT__LEFT;
        s = tmp;
        if (qualifier == 'l') {
          ((u16*)tmp)[0] = (u16)va_arg(args, i32);
          ((u16*)tmp)[1] = L'\0';
          precision = MAX_I32;
          goto wstring;
        } else {
          tmp[0] = (unsigned char)va_arg(args, int);
          precision = len = 1;
        }
        goto output;

      case 's':
        flags &= __STRING_FORMAT__LEFT;
        if (precision < 0)
          precision = MAX_I32;
        s = va_arg(args, void*);
        if (!s)
          s = precision < 6 ? "" : "(null)";
        else if (qualifier == 'l') {
        wstring:
          flags |= __STRING_FORMAT__WIDE;
          precision = len = __string_format_va__utf16s_utf8nlen((const u16*)s, precision);
          goto output;
        }
        precision = len = strnlen(s, precision);
        goto output;

        /* integer number formats - set up the flags and "break" */
      case 'o':
        base = 8;
        break;

      case 'p':
        if (precision < 0)
          precision = 2 * sizeof(void*);
      case 'x':
        flags |= __STRING_FORMAT__SMALL;
      case 'X':
        base = 16;
        break;

      case 'd':
      case 'i':
        flags |= __STRING_FORMAT__SIGN;
      case 'u':
        flags &= ~__STRING_FORMAT__SPECIAL;
        base = 10;
        break;

      default:
        /*
         * Bail out if the conversion specifier is invalid.
         * There's probably a typo in the format string and the
         * remaining specifiers are unlikely to match up with
         * the arguments.
         */
        goto fail;
    }
    if (*format == 'p') {
      num = (i64)va_arg(args, void*);
    } else {
      num = __string_format_va__get_number(flags & __STRING_FORMAT__SIGN, qualifier, &args);
    }

    sign = __string_format_va__get_sign(&num, flags);
    if (sign)
      --field_width;

    s = __string_format_va__number(tmp_end, num, base, flags & __STRING_FORMAT__SMALL);
    len = tmp_end - s;
    /* default precision is 1 */
    if (precision < 0)
      precision = 1;
    /* precision is minimum number of digits to print */
    if (precision < len)
      precision = len;
    if (flags & __STRING_FORMAT__SPECIAL) {
      /*
       * For octal, a leading 0 is printed only if necessary,
       * i.e. if it's not already there because of the
       * precision.
       */
      if (base == 8 && precision == len)
        ++precision;
      /*
       * For hexadecimal, the leading 0x is skipped if the
       * output is empty, i.e. both the number and the
       * precision are 0.
       */
      if (base == 16 && precision > 0)
        field_width -= 2;
      else
        flags &= ~__STRING_FORMAT__SPECIAL;
    }
    /*
     * For zero padding, increase the precision to fill the field
     * width.
     */
    if ((flags & __STRING_FORMAT__ZEROPAD) && field_width > precision)
      precision = field_width;

  output:
      /* Calculate the padding necessary */
    field_width -= precision;
    /* Leading padding with ' ' */
    if (!(flags & __STRING_FORMAT__LEFT))
      while (field_width-- > 0)
        __STRING_FORMAT__PUTC(' ');
    /* sign */
    if (sign)
      __STRING_FORMAT__PUTC(sign);
    /* 0x/0X for hexadecimal */
    if (flags & __STRING_FORMAT__SPECIAL) {
      __STRING_FORMAT__PUTC('0');
      __STRING_FORMAT__PUTC('X' | (flags & __STRING_FORMAT__SMALL));
    }
    /* Zero padding and excess precision */
    while (precision-- > len)
      __STRING_FORMAT__PUTC('0');
    /* Actual output */
    if (flags & __STRING_FORMAT__WIDE) {
      const u16* ws = (const u16*)s;

      while (len-- > 0) {
        u32 c32 = __string_format_va__utf16_to_utf32(&ws);
        u8* s8;
        u64 clen;

        if (c32 < 0x80) {
          __STRING_FORMAT__PUTC(c32);
          continue;
        }

        /* Number of trailing octets */
        clen = 1 + (c32 >= 0x800) + (c32 >= 0x10000);

        len -= clen;
        s8 = (u8*)&target[pos];

        /* Avoid writing partial character */
        __STRING_FORMAT__PUTC('\0');
        pos += clen;
        if (pos >= size)
          continue;

        /* Set high bits of leading octet */
        *s8 = (0xf00 >> 1) >> clen;
        /* Write trailing octets in reverse order */
        for (s8 += clen; clen; --clen, c32 >>= 6)
          *s8-- = 0x80 | (c32 & 0x3f);
        /* Set low bits of leading octet */
        *s8 |= c32;
      }
    } else {
      while (len-- > 0)
        __STRING_FORMAT__PUTC(*s++);
    }
    /* Trailing padding with ' ' */
    while (field_width-- > 0)
      __STRING_FORMAT__PUTC(' ');
  }
fail:
  va_end(args);

  if (size)
    target[min(pos, size - 1)] = '\0';

  return pos;
}
#undef __STRING_FORMAT__PUTC