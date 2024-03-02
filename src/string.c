#include "sdk/string.h"

#include "internal/memory.h"

#include <stdarg.h>
#include <stdio.h> // vsnprintf

class string {
  char* data;
  u64 length;
} string;

string* string_new(string* this) {
  return memory_alloc0(sizeof(string));
}
void string_free(string* this) {
  memory_free(this->data);
}

const char* string_cstr(const string* this) {
  return this->data;
}
u64 string_length(const string* this) {
  return this->length;
}
bool string_equal_cstr(const string* this, const char* cstr) {
  return cstr_equal(this->data, cstr);
}
bool string_equal_string(const string* this, const string* other) {
  if (this->length != other->length)
    return false;
  return cstr_equal(this->data, other->data);
}
i16 string_compare(const string* this, const string* right) {
  return cstr_compare(this->data, right->data);
}
void string_append_str(string* this, const char* cstr, u64 length) {
  if (!cstr || !*cstr)
    return;
  u64 total_length = this->length + length;
  this->data = memory_realloc(this->data, total_length + 1);

  char* ch = this->data + this->length;
  do { *ch++ = *cstr++; } while (*cstr);
  *ch = '\0';
  this->length = total_length;
}
void string_append_string(string* this, string* src) {
  string_append_str(this, src->data, src->length);
}
void string_append_char(string* this, const char ch) {
  if (!ch)
    return;
  this->data = memory_realloc(this->data, this->length + 2);
  this->data[this->length] = ch;
  ++this->length;
  this->data[this->length] = '\0';
}

i16 cstr_compare(const char* s1, const char* s2) {
  while (*s1 && *s1 == *s2) {
    ++s1;
    ++s2;
  }
  return (*s1 - *s2);
}
bool cstr_equal(const char* s1, const char* s2) {
  while (*s1) {
    if (*s1 != *s2)
      return false;
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
    return length;
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