#ifndef SDK_BUFFER_H
#define SDK_BUFFER_H

#include <sdk/types.h>

#define BUFFER_SIZE 65536

typedef struct buffer_t {
  const u8* data;
  u64 length;
} buffer_t;

extern u8 buffer_shared[BUFFER_SIZE];

#endif