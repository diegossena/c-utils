#include <sdk/hashset.h>

export void hashset_constructor(hashset_t* this) {
  memory_fill(this->__buckets, 0, sizeof(this->__buckets));
  *(u64*)&this->length = 0;
}
export void hashset_deconstructor(hashset_t* this) {
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
}
export bool hashset_contains(const hashset_t* this, const u64 hash) {
  hashmap_entry_t* node = *(this->__buckets + hash % __HASHSET_BUCKETS_SIZE);
  while (node) {
    if (hash == node->__hash) {
      return true;
    }
    node = node->__next;
  }
  return 0;
}
export void hashset_add(hashset_t* this, const u64 hash) {
  hashmap_entry_t** it = this->__buckets + hash % __HASHSET_BUCKETS_SIZE;
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
}
export bool hashset_remove(hashset_t* this, const u64 hash) {
  if (!this->length)
    return false;
  hashmap_entry_t** it = this->__buckets;
  it += hash % __HASHSET_BUCKETS_SIZE;
  while (*it) {
    if ((*it)->__hash == hash) {
      hashmap_entry_t* node_next = (*it)->__next;
      memory_free((*it));
      *it = node_next;
      --*(u64*)&this->length;
      return true;
    }
    it = &(*it)->__next;
  }
  // not found
  return false;
}

#undef BUCKETS_SIZE