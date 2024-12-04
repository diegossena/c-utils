#ifndef BUFFER_H
#define BUFFER_H

#include <sdk/types.h>
#include <sdk/memory.h>

#define BUFFER_DEFAULT_SIZE 65535

typedef struct buffer_t {
  u64 length;
  // ...data
} buffer_t;

void* buffer_new(u64 size);
void buffer_free(void* this);
void* buffer_resize(void* this, u64 size);
void* buffer_resize0(void* this, u64 size);
buffer_t* buffer_header(void* this);
u64 buffer_length(void* this);

#endif