#include "sdk/path.h"
#include "internal/memory.h"
#include "internal/string.h"
#include "sdk/console.h" // TODO: remove this line

void path_join(string_t* this, const char* path, u64 length) {
  char* ptr = this->data + this->length - 1;
  if (*ptr != '\\' && *ptr != '/') {
    *++ptr = path_sep;
    ++this->length;
  }
  this->data = memory_realloc(this->data, this->length + length + 1);
  ptr = this->data + this->length;
  memory_copy(ptr, path, length);
  ptr += length;
  *ptr = '\0';
  this->length += length;
}
void path_dirname(string_t* this) {
  char* ptr = this->data + this->length - 1;
  while (--this->length && *ptr != '\\' && *ptr != '/') {
    --ptr;
  }
  *ptr = '\0';
  this->data = memory_realloc(this->data, this->length + 1);
}