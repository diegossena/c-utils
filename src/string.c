#include "sdk/string.h"

#include "base/string.h"
#include "base/memory.h"

#include <stdio.h> // TODO: remove this line

void string_constructor(string* this) {
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
@returns i32 - written
*/
i32 string_format(char* stream, const char* format, void* args) {
  i32 written = vsnprintf(stream, MAX_BUFSIZ, format, args);
  stream[written] = '\0';
  if (written < 0) {
    error("vsnprintf", written);
    return written;
  }
  return written;
}

void string_append_cstr(string* this, const char* cstr) {
  if (!*cstr)
    return;
  u64 length = this->length;

  this->length += cstr_length(cstr);
  this->data = memory_realloc(this->data, this->length + 1);

  char* ch = this->data + length;
  do { *ch++ = *cstr++; } while (*cstr);
  *ch = '\0';
}
void string_append_char(string* this, const char ch) {
  if (!ch)
    return;
  this->data = memory_realloc(this->data, this->length + 2);
  this->data[this->length++] = ch;
  this->data[this->length] = '\0';
}