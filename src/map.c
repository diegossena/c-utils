#include "sdk/map.h"

#include "base/memory.h"

#include <stdio.h> // TODO: remove this line

#define MIN_HASH_SIZE 512ULL // table size when first created
#define OCCUPANCY_PCT 0.5 // large PCT means smaller and slower

interface map_entry map_entry;

interface map_entry {
  u64 hash;
  void* value;
  map_entry* next;
} map_entry;

void __rehash(map* this, u64 bucket_length) {
  map_entry** buckets = memory_alloc0(bucket_length);
  map_entry** old_it = this->buckets;
  if (this->length) {
    while (true) {
      map_entry* node = *old_it;
      while (node) {
        map_entry** it = buckets + node->hash % bucket_length;
        while (*it) it = &(*it)->next;
        *it = node;
        --this->length;
        node = node->next;
      }
      if (!this->length)
        break;
      ++old_it;
    }
  }
  memory_free(this->buckets);
  this->buckets = buckets;
  this->buckets_length = bucket_length;
}

void map_new_stride(map* this, u64 stride) {
  this->length = 0;
  this->stride = stride;
  this->buckets = memory_alloc0(sizeof(map_entry**) * MIN_HASH_SIZE);
  this->buckets_length = MIN_HASH_SIZE;
}
void map_free(map* this) {
  map_entry** it = this->buckets;
  while (true) {
    map_entry* node = *it;
    while (node) {
      map_entry* node_next = node->next;
      memory_free(node->value);
      memory_free(node);
      --this->length;
      node = node_next;
    }
    if (!this->length)
      break;
    ++it;
  }
  memory_free(this->buckets);
}

void* map_get(const map* this, const u64 hash) {
  map_entry* node = *((map_entry**)this->buckets + hash % this->buckets_length);
  while (node) {
    if (hash == node->hash) {
      return node->value;
    }
    node = node->next;
  }
  return 0;
}
void _map_set(map* this, const u64 hash, const void* value) {
  map_entry** it = this->buckets;
  it += hash % this->buckets_length;
  while (true) {
    if (!*it) {
      // new entry
      *it = memory_alloc(sizeof(map_entry));
      (*it)->hash = hash;
      (*it)->value = memory_alloc(this->stride);
      memory_copy((*it)->value, value, this->stride);
      (*it)->next = 0;
      ++this->length;
      break;
    } else if ((*it)->hash == hash) {
      // overwrite entry
      memory_copy((*it)->value, value, this->stride);
      break;
    }
    it = &(*it)->next;
  };
  if (this->length >= this->buckets_length * OCCUPANCY_PCT) {
    u64 new_length = math_next2pow(this->length / OCCUPANCY_PCT);
    __rehash(this, new_length);
  }
}
bool map_delete(map* this, u64 hash) {
  if (!this->length)
    return false;
  map_entry** it = this->buckets;
  while (*it) {
    if ((*it)->hash == hash) {
      map_entry* node_next = (*it)->next;
      memory_free((*it)->value);
      memory_free((*it));
      *it = node_next;
      --this->length;
      if (
        this->length < this->buckets_length * OCCUPANCY_PCT
        && (this->buckets_length >> 1) >= MIN_HASH_SIZE
        ) {
        u64 new_length = math_next2pow(this->length / OCCUPANCY_PCT);
        __rehash(this, new_length);
      }
      return true;
    }
    it = &(*it)->next;
  }
  // not found
  return false;
}