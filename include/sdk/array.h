#ifndef ARRAY_H
#define ARRAY_H

#include <sdk/types.h>

#define array_t(type) \
  typedef struct array_ ## type ## { \
    type* buffer; \
    u64 length; \
    u64 capacity; \
  }

#define array_push(array, item) array.buffer[array.length++] = item

#endif