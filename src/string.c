#include "sdk/string.h"

#include "base/string.h"
#include "base/memory.h"

#include <stdio.h> // TODO: remove this line

void string_constructor(string* str) {
  str->data = 0;
  str->length = 0;
}
void string_free(string* str) {
  memory_free(str->data);
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

void string_append_cstr(string* str, const char* cstr) {
  if (!*cstr)
    return;
  u64 length = str->length;

  str->length += cstr_length(cstr);
  str->data = memory_realloc(str->data, str->length + 1);

  char* ch = str->data + length;
  do { *ch++ = *cstr++; } while (*cstr);
  *ch = '\0';
}
void string_append_char(string* str, const char ch) {
  if (!ch)
    return;
  str->data = memory_realloc(str->data, str->length + 2);
  str->data[str->length++] = ch;
  str->data[str->length] = '\0';
}