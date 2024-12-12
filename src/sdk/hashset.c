#include <sdk/hashset.h>

#define HASHSET_INITIAL_SIZE 512
#define HASHSET_DATA(this) ((u64*)this + sizeof(hashset_t))

export i32 array_u64_search(u64* array, u64 length, u64 value) {
  u64 low = 0;
  u64 high = length - 1;
  while (low <= high) {
    u64 mid = low + (high - low) / 2;
    // Check if x is present at mid
    if (array[mid] == value) {
      return mid;
    }
    if (array[mid] < value) {
      // If x greater, ignore left half
      low = mid + 1;
    } else {
      // If x is smaller, ignore right half
      high = mid - 1;
    }
  }
  // If we reach here, then element was not present
  return -1;
}

export hashset_t* hashset_new() {
  hashset_t* this = memory_alloc(sizeof(hashset_t) + HASHSET_INITIAL_SIZE * sizeof(u64));
  if (this) {
    this->length = 0;
    this->size = HASHSET_INITIAL_SIZE;
  }
  return this;
}
export void hashset_free(hashset_t* this) {
  memory_free(this);
}
export bool hashset_contains(const hashset_t* this, u64 hash) {
  return array_u64_search((void*)this + sizeof(hashset_t), this->length, hash) != -1;
}
export bool hashset_add(hashset_t** this_p, u64 hash) {
  hashset_t* this = *this_p;
  u64* data = HASHSET_DATA(this);
  u64 length = this->length;
  u64 size = this->size;
  u64 i = 0;
  if (length >= size) {
    size <<= 1;
    hashset_t* block = memory_realloc(this, sizeof(hashset_t) + size * sizeof(u64));
    if (block == null)
      return false;
    this = *this_p = block;
    this->size = size;
    data = HASHSET_DATA(this);
  }
  while (i < length) {
    if (data[i] == hash) {
      return true;
    } else if (data[i] > hash) {
      memory_copy(data + i + 1, data + i, (length - i) * sizeof(u64));
      break;
    }
    ++i;
  }
  ++length;
  data[i] = hash;
  this->length = length;
  return true;
}
export bool hashset_remove(hashset_t* this, const u64 hash) {
  u64* data = (void*)this + sizeof(hashset_t);
  for (u64 i = 0; i < this->length; i++) {
    if (data[i] == hash) {

      return true;
    }
  }
  return false;
}

#undef HASHSET_INITIAL_SIZE
#undef HASHSET_DATA