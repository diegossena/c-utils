#include <sdk/hashset.h>

export hashset_t* hashset_new() {
  hashset_t* this = memory_alloc0(sizeof(hashset_t));
  assert(this);
  return this;
}
export void hashset_free(hashset_t* this) {
  memory_free(this);
}
export bool hashset_contains(const hashset_t* this, u64 hash) {
  u64 length = this->length;
  const u64* it = (void*)this + sizeof(hashset_t);
  while (length--) {
    if (*it++ == hash)
      return true;
  }
  return false;
}
export bool hashset_add(hashset_t** this, u64 hash) {
  u64* data = (void*)*this + sizeof(hashset_t);
  u64 length = (*this)->length;
  u64 i = 0;
  while (i < length) {
    if (data[i] == hash) {
      return true;
    } else if (data[i] > hash) {
      break;
    }
    ++i;
  }
  ++length;
  hashset_t* block = memory_realloc(*this, sizeof(hashset_t) + length * sizeof(u64));
  if (block == null)
    return false;
  *this = block;
  (*this)->length = length;
  data = (void*)block + sizeof(hashset_t);
  memory_copy(data + i + 1, data + i, length - i);
  data[i] = hash;
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

#undef BUCKETS_SIZE