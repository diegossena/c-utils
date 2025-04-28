#include <sdk/string.h>

// vsnprintf
#include <stdio.h>

export u64 string_length(const char* this) {
  const char* ptr = this;
  while (*ptr != '\0') ++ptr;
  return ptr - this;
}
export u64 string_nlength(const char* this, u64 count) {
  const char* ptr = this;
  while (count-- && *ptr != '\0') ++ptr;
  return ptr - this;
}
export bool string_equal(const char* this, const char* cstr) {
  while (*this != '\0') {
    if (*this++ != *cstr++)
      return false;
  }
  return true;
}
export bool string_startswith(const char* this, const char* neddle) {
  while (*neddle != '\0') {
    if (*this++ != *neddle++)
      return false;
  }
  return true;
}
export i32 string_format_va(char* this, const char* format, va_list args) {
  return sprintf(this, format, args);
}
export i32 string_format(char* stream, const char* format, ...) {
  va_list argv;
  va_start(argv, format);
  i32 length = string_format_va(stream, format, argv);
  va_end(argv);
  return length;
}
export i32 string_nformat_va(char* this, u64 size, const char* format, va_list args) {
  return vsnprintf(this, size, format, args);
}
export i32 string_nformat(char* target, u64 size, const char* format, ...) {
  va_list argv;
  va_start(argv, format);
  i32 length = string_nformat_va(target, size, format, argv);
  va_end(argv);
  return length;
}
export i64 string_i64(const char* this) {
  if (*this == '\0')
    return 0;
  bool neg = false;
  if (*this == '-') {
    neg = true;
    ++this;
  }
  i64 number = string_u64(this);
  return neg ? -number : number;
}
export u64 string_skip_u64(const char** this) {
  i64 number = 0;
  while (IS_DIGIT(**this))
    number = number * 10 + *((*this)++) - '0';
  return number;
}
export u64 string_u64(const char* this) {
  return string_skip_u64(&this);
}
export char* string_replace(const char* this, char* target, const char* pattern, const char* replacement) {
  u64 pattern_pass = string_length(pattern);

  char copy[TEXT_SIZE + 1] = { '\0' };

  const char* replacement_p;
  char
    * target_p = target,
    * copy_p = copy;

  while (*this)
    *copy_p++ = *this++;
  copy_p = copy;
  while (*copy_p) {
    if (string_equal(pattern, copy_p)) {
      replacement_p = replacement;
      while (*replacement_p != '\0')
        *target_p++ = *replacement_p++;
      copy_p += pattern_pass;
    } else {
      *target_p++ = *copy_p++;
    }
  }
  return target;
}
export char* url_encode(const char* this, char* target) {
  char copy[URL_SIZE + 1] = {};
  char
    * copy_p = copy,
    * target_p = target;
  while (*this)
    *copy_p++ = *this++;
  copy_p = copy;
  u8 ch;
  while (true) {
    ch = *copy_p;
    if (ch == '\0')
      break;
    ++copy_p;
    if (IS_ALPHA(ch) || IS_URL_PUNCT(ch)) {
      *target_p++ = ch;
    } else {
      const char hex [] = "0123456789ABCDEF";
      *target_p++ = '%';
      *target_p++ = hex[ch >> 4];
      *target_p++ = hex[ch & 0xf];
    }
  }
  return target;
}

export u64 wstring_length(const wchar_t* cstr) {
  const wchar_t* begin = cstr;
  while (*cstr != L'\0') ++cstr;
  return cstr - begin;
}