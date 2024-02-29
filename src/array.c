#include "sdk/array.h"
#include "sdk/math.h"
#include "base/memory.h"

#include <stdio.h> // TODO: remove this line

bool __resize(array* arr, u64 capacity) {
  arr->data = memory_realloc(arr->data, capacity * arr->stride);
  arr->capacity = capacity;
}

void _array_new(array* arr, u64 stride) {
  arr->capacity = 0;
  arr->length = 0;
  arr->stride = stride;
  arr->data = 0;
}
void array_free(array* arr) {
  memory_free(arr->data);
}

void _array_push(array* arr, const void* value) {
  if (arr->length == arr->capacity) {
    u64 capacity = math_closest_2pow(arr->capacity + 1);
    __resize(arr, capacity);
  }
  void* addr = arr->data + arr->length * arr->stride;
  memory_copy(addr, value, arr->stride);
  ++arr->length;
}