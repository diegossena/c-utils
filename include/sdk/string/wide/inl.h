#include <sdk/string/wide.h>
#include <sdk/memory.h>
#include <sdk/math.h>
#include <stdio.h>
#include <stdarg.h>

void __wstring_mutate(wstring_t* this, u64 size) {
  if (this->__data == this->__small_string) {
    if (size > SMALL_STRING_SIZE) {
      u64 capacity = math_next2pow(size);
      wchar_t* buffer = (wchar_t*)memory_alloc(sizeof(wchar_t) * size + 1);
      memory_copy(buffer, this->__data, this->__length * sizeof(wchar_t));
      this->__data = buffer;
      this->__capacity = capacity;
    }
  } else if (size > this->__capacity) {
    u64 capacity = math_next2pow(size);
    this->__data = (wchar_t*)memory_realloc(this->__data, capacity * sizeof(wchar_t));
    this->__capacity = capacity;
  }
}
/*
@returns i32 - length
*/
i32 __cwstr_format_va(wchar_t* stream, u64 count, const wchar_t* format, void* args) {
  i32 length = vswprintf(stream, count, format, args);
  // stream[length] = L'\0';
  if (length < 0) {
    error("vswprintf", length);
  }
  return length;
}

void wstring_new(wstring_t* this) {
  this->__data = 0;
  this->__length = 0;
  *this->__small_string = L'\0';
  this->__data = this->__small_string;
}
void wstring_free(wstring_t* this) {
  if (this->__data != this->__small_string) {
    memory_free(this->__data);
  }
}
const wchar_t* wstring_cwstr(const wstring_t* this) {
  return this->__data;
}
u64 wstring_length(const wstring_t* this) {
  return this->__length;
}
bool wstring_equal_cwstr(const wstring_t* this, const wchar_t* cstr) {
  return cwstr_equal(this->__data, cstr);
}
bool wstring_equal_wstring(const wstring_t* this, const wstring_t* other) {
  if (this->__length != other->__length)
    return false;
  return cwstr_equal(this->__data, other->__data);
}
i16 wstring_compare(const wstring_t* this, const wstring_t* right) {
  return cwstr_compare(this->__data, right->__data);
}
void wstring_clear(wstring_t* this) {
  this->__length = 0;
  *this->__data = L'\0';
}
void wstring_append_wstr(wstring_t* this, const wchar_t* cstr, u64 length) {
  if (!length)
    return;
  u64 total_length = this->__length + length;
  __wstring_mutate(this, total_length + 1);
  memory_copy(this->__data + this->__length, cstr, sizeof(wchar_t) * length);
  this->__length = total_length;
  this->__data[this->__length] = L'\0';
}
void wstring_append_wstring(wstring_t* this, wstring_t* src) {
  wstring_append_wstr(this, src->__data, src->__length);
}
void wstring_append_wchar(wstring_t* this, const wchar_t ch) {
  if (!ch)
    return;
  wstring_append_wstr(this, &ch, 1);
}
void string_format(wstring_t* this, const wchar_t* format, ...) {
  u64 capacity = this->__data == this->__small_string
    ? SMALL_STRING_SIZE
    : this->__capacity;
  __builtin_va_list args;
  va_start(args, format);
  this->__length = __cwstr_format_va(this->__data, capacity, format, args);
  va_end(args);
}

i16 cwstr_compare(const wchar_t* s1, const wchar_t* s2) {
  while (*s1 && *s1 == *s2) { ++s1; ++s2; }
  return (*s1 - *s2);
}
bool cwstr_equal(const wchar_t* s1, const wchar_t* s2) {
  while (true) {
    if (*s1 != *s2)
      return false;
    if (!*s1)
      break;
    ++s1; ++s2;
  }
  return true;
}
/*
@returns i32 - length
*/
i32 cwstr_format(wchar_t* stream, const wchar_t* format, ...) {
  __builtin_va_list args;
  va_start(args, format);
  i32 length = __cwstr_format_va(stream, MAX_BUFSIZ, format, args);
  va_end(args);
  return length;
}
u64 cwstr_length(const wchar_t* cstr) {
  const wchar_t* begin = cstr;
  while (*cstr != L'\0') ++cstr;
  return cstr - begin;
}