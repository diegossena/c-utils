#ifndef SDK_ARRAY
#define SDK_ARRAY

#include <sdk/types.h>
#include <sdk/memory.h>

typedef struct array_t {
  u64 capacity;
  u64 length;
  u64 stride;
  // ...data
} array_t;

void* array_new(u64 length, u64 stride);
void* array_free(void* this);

void* array_resize(void* this);
void* array_push(void* this, const void* value_ptr);

array_t* array_header(void* this);

#endif