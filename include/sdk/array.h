#ifndef SDK_ARRAY
#define SDK_ARRAY

#include <sdk/types.h>

#define array_push(this, value) { \
  typeof(value) tmp = value; \
  _array_push(this, &tmp); \
}
#define array_constructor(this, type) \
  _array_constructor(this, sizeof(type))

#define array_at(this, type, index) \
  (*(type*)_array_at(this, index))

typedef struct array_t {
  u64 __capacity;
  u64 __length;
  u64 __stride;
  void* __data;
} array_t;

#endif