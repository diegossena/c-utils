#ifndef SDK_BUFFER_H
#define SDK_BUFFER_H

#include <sdk/types.h>

#define BUFFER_SIZE 65535

typedef u8 buffer_default_t[BUFFER_SIZE];

typedef struct buffer_t {
  const u8* data;
  u64 length;
} buffer_t;

buffer_t buffer_new(u64 size);
void buffer_free(buffer_t* this);

#endif