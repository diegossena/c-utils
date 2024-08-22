#ifndef SDK_ARRAY
#define SDK_ARRAY

#include <sdk/types.h>
#include <sdk/memory.h>

typedef struct array_t {
  u64 length;
  // u64 stride;
  // ...data
} array_t;

void* array_new(u64 length);
void* array_free(void* this);
void* array_resize(void* this, u64 size);

array_t* array_header(void* this);

#endif