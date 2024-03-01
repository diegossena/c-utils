#include "sdk/string.h"

#include "base/string.h"
#include "base/memory.h"

#include <stdarg.h>
#include <stdio.h> // TODO: remove this line

void string_new(string* this) {
  this->data = 0;
  this->length = 0;
}
void string_free(string* this) {
  memory_free(this->data);
}

i16 string_compare(const char* s1, const char* s2) {
  while (*s1 && *s1 == *s2) {
    ++s1;
    ++s2;
  }
  return (*s1 - *s2);
}
bool string_equal(const char* s1, const char* s2) {
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
i32 string_format_v(char* stream, const char* format, void* args) {
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
i32 string_format(char* stream, const char* format, ...) {
  __builtin_va_list args;
  va_start(args, format);
  i32 length = string_format_v(stream, format, args);
  va_end(args);
  return length;
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
void string_append_char(string* this, const char ch) {
  if (!ch)
    return;
  this->data = memory_realloc(this->data, this->length + 2);
  this->data[this->length] = ch;
  ++this->length;
  this->data[this->length] = '\0';
}