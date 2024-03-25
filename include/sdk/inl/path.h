#ifdef SDK_PATH_H

#include <sdk/path.h>
#include <sdk/string.h>

void path_join(string_t* this, const char* path, u64 length) {
  char* ptr = this->__data + this->__length - 1;
  if (*ptr != '\\' && *ptr != '/') {
    *++ptr = path_sep;
    ++this->__length;
  }
  __string_mutate(this, MAX_PATH);
  ptr = this->__data + this->__length;
  memory_copy(ptr, path, length);
  ptr += length;
  *ptr = '\0';
  this->__length += length;
}
void path_dirname(string_t* this) {
  char* ptr = this->__data + this->__length - 1;
  while (--this->__length && *ptr != '\\' && *ptr != '/') {
    --ptr;
  }
  *ptr = '\0';
  __string_mutate(this, MAX_PATH);
}

#endif