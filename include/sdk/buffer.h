#ifndef BUFFER_H
#define BUFFER_H

#include <sdk/types.h>
#include <sdk/memory.h>

typedef struct buffer_t {
  u64 length, size;
  // ...data
} buffer_t;

void* buffer_new(u64 size);
void buffer_free(void* this);
void* buffer_resize(void* this, u64 size);
void* buffer_resize0(void* this, u64 size);
buffer_t* buffer_header(void* this);

#endif