#include <sdk/string.h>
#include <sdk/memory.h>
#include <sdk/math.h>
#include <stdio.h>
#include <stdarg.h>

void __string_mutate(string_t* this, u64 size) {
  if (this->__data == this->__small_string) {
    if (size > SMALL_STRING_SIZE) {
      u64 capacity = math_next2pow(size);
      char* buffer = (char*)memory_alloc(size + 1);
      memory_copy(buffer, this->__data, this->__length);
      this->__data = buffer;
      this->__capacity = capacity;
    }
  } else if (size > this->__capacity) {
    u64 capacity = math_next2pow(size);
    this->__data = (char*)memory_realloc(this->__data, capacity);
    this->__capacity = capacity;
  }
}

void string_constructor(string_t* this) {
  this->__data = 0;
  this->__length = 0;
  *this->__small_string = '\0';
  this->__data = this->__small_string;
}
void string_deconstructor(string_t* this) {
  memory_free(this->__data);
}

const char* string_cstr(const string_t* this) {
  return this->__data;
}
u64 string_length(const string_t* this) {
  return this->__length;
}
bool string_equal_cstr(const string_t* this, const char* cstr) {
  return cstr_equal(this->__data, cstr);
}
bool string_equal_string(const string_t* this, const string_t* other) {
  if (this->__length != other->__length)
    return false;
  return cstr_equal(this->__data, other->__data);
}
i16 string_compare(const string_t* this, const string_t* right) {
  return cstr_compare(this->__data, right->__data);
}
void string_clear(string_t* this) {
  this->__length = 0;
  *this->__data = '\0';
}
void string_append_str(string_t* this, const char* cstr, u64 length) {
  if (!length)
    return;
  u64 total_length = this->__length + length;
  __string_mutate(this, total_length + 1);
  memory_copy(this->__data + this->__length, cstr, length);
  this->__length = total_length;
  this->__data[this->__length] = '\0';
}
void string_append_string(string_t* this, string_t* src) {
  string_append_str(this, src->__data, src->__length);
}
void string_append_char(string_t* this, const char ch) {
  if (!ch)
    return;
  string_append_str(this, &ch, 1);
}

i16 cstr_compare(const char* s1, const char* s2) {
  while (*s1 && *s1 == *s2) { ++s1; ++s2; }
  return (*s1 - *s2);
}
bool cstr_equal(const char* s1, const char* s2) {
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
i32 cstr_format_v(char* stream, const char* format, void* args) {
  i32 length = vsnprintf(stream, MAX_BUFSIZ, format, args);
  stream[length] = '\0';
  if (length < 0) {
    error("vsnprintf", length);
  }
  return length;
}
/*
@returns i32 - length
*/
i32 cstr_format(char* stream, const char* format, ...) {
  __builtin_va_list args;
  va_start(args, format);
  i32 length = cstr_format_v(stream, format, args);
  va_end(args);
  return length;
}
u64 cstr_length(const char* cstr) {
  const char* begin = cstr;
  while (*cstr) ++cstr;
  return cstr - begin;
}