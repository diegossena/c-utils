#include <sdk/hashset.h>

#define HASHSET_INITIAL_SIZE 2LLU

export u64* hashset_entries(const hashset_t* this) {
  return (void*)this + sizeof(hashset_t);
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
  u64* data = hashset_entries(this);
  u64 low = 0;
  u64 mid = 0;
  u64 high = this->length - 1;
  while (low <= high) {
    mid = low + (high - low) / 2;
    if (data[mid] == hash) {
      return true;
    } else if (data[mid] < hash) {
      low = mid + 1;
    } else {
      high = mid - 1;
    }
  }
  // If we reach here, then element was not present
  return false;
}
export bool hashset_add(hashset_t** this_p, u64 hash) {
  hashset_t* this = *this_p;
  u64* data = hashset_entries(this);
  u64 length = this->length;
  u64 size = this->size;
  if (length >= size) {
    size <<= 1;
    this = memory_realloc(this, sizeof(hashset_t) + size * sizeof(u64));
    if (this == null)
      return false;
    *this_p = this;
    this->size = size;
    data = hashset_entries(this);
  }
  u64 index = 0;
  if (length) {
    u64 low = 0;
    u64 high = length - 1;
    while (low <= high) {
      index = low + (high - low) / 2;
      if (data[index] == hash) {
        return true;
      } else if (data[index] > hash) {
        if (index == 0)
          break;
        high = index - 1;
      } else {
        low = index + 1;
      }
    }
    index = low;
  }
  for (u64 i = length; i > index; i--) {
    data[i] = data[i - 1];
  }
  data[index] = hash;
  this->length = length + 1;
  return true;
}
export bool hashset_remove(hashset_t* this, const u64 hash) {
  u64* data = hashset_entries(this);
  u64 low = 0;
  u64 i = 0;
  u64 high = this->length - 1;
  while (low <= high) {
    i = low + (high - low) / 2;
    if (data[i] == hash) {
      --this->length;
      while (i < this->length) {
        data[i] = data[i + 1];
        ++i;
      }
      return true;
    } else if (data[i] < hash) {
      low = i + 1;
    } else {
      high = i - 1;
    }
  }
  return false;
}

#undef HASHSET_INITIAL_SIZE