#include <sdk/hashset.h>

#define HASHSET_MIN_SIZE 512ULL // table size when first created
#define HASHSET_OCCUPANCY_PCT 0.5 // large PCT means smaller and slower

SDK_EXPORT void __hashset_rehash(hashset_t* this, u64 bucket_length) {
  hashmap_entry_t** old_it = this->__buckets;
  hashmap_entry_t** new_bucket = memory_alloc0(sizeof(this->__buckets) * bucket_length);
  do {
    if (*old_it) {
      hashmap_entry_t** new_node = new_bucket + (*old_it)->__hash % bucket_length;
      *new_node = *old_it;
    }
    ++old_it;
  } while (--this->__buckets_length);
  memory_free(this->__buckets);
  this->__buckets = new_bucket;
  this->__buckets_length = bucket_length;
}
SDK_EXPORT void hashset_constructor(hashset_t* this) {
  this->__buckets = memory_alloc0(sizeof(this->__buckets) * HASHSET_MIN_SIZE);
  this->__buckets_length = HASHSET_MIN_SIZE;
  *(u64*)&this->length = 0;
}
SDK_EXPORT void hashset_deconstructor(hashset_t* this) {
  hashmap_entry_t** it = this->__buckets;
  while (true) {
    hashmap_entry_t* node = *it;
    while (node) {
      hashmap_entry_t* node_next = node->__next;
      memory_free(node);
      --*(u64*)&this->length;
      node = node_next;
    }
    if (!this->length)
      break;
    ++it;
  }
  memory_free(this->__buckets);
}
SDK_EXPORT bool hashset_contains(const hashset_t* this, const u64 hash) {
  hashmap_entry_t* node = *(this->__buckets + hash % this->__buckets_length);
  while (node) {
    if (hash == node->__hash) {
      return true;
    }
    node = node->__next;
  }
  return 0;
}
SDK_EXPORT void hashset_add(hashset_t* this, const u64 hash) {
  hashmap_entry_t** it = this->__buckets + hash % this->__buckets_length;
  while (true) {
    if (!*it) {
      // new entry
      *it = memory_alloc(sizeof(hashmap_entry_t));
      (*it)->__hash = hash;
      (*it)->__next = 0;
      ++*(u64*)&this->length;
      break;
    } else if ((*it)->__hash == hash) {
      // already exists
      break;
    }
    it = &(*it)->__next;
  }
  if (this->length >= this->__buckets_length * HASHSET_OCCUPANCY_PCT) {
    u64 new_length = math_next2pow(this->length / HASHSET_OCCUPANCY_PCT);
    __hashset_rehash(this, new_length);
  }
}
SDK_EXPORT bool hashset_remove(hashset_t* this, const u64 hash) {
  if (!this->length)
    return false;
  hashmap_entry_t** it = this->__buckets;
  it += hash % this->__buckets_length;
  while (*it) {
    if ((*it)->__hash == hash) {
      hashmap_entry_t* node_next = (*it)->__next;
      memory_free((*it));
      *it = node_next;
      --*(u64*)&this->length;
      if (
        this->length < this->__buckets_length * HASHSET_OCCUPANCY_PCT
        && (this->__buckets_length >> 1) >= HASHSET_MIN_SIZE
        ) {
        u64 new_length = math_next2pow(this->length / HASHSET_OCCUPANCY_PCT);
        __hashset_rehash(this, new_length);
      }
      return true;
    }
    it = &(*it)->__next;
  }
  // not found
  return false;
}

#undef HASHSET_MIN_SIZE
#undef HASHSET_OCCUPANCY_PCT