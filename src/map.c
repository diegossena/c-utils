#include "sdk/map.h"

#include "internal/memory.h"

#include <stdio.h> // TODO: remove this line

#define MIN_HASH_SIZE 512ULL // table size when first created
#define OCCUPANCY_PCT 0.5 // large PCT means smaller and slower

class map {
  u64 length;
  u64 stride;
  void* buckets;
  u64 buckets_length;
} map;
interface map_entry map_entry;
interface map_entry {
  u64 hash;
  void* value;
  map_entry* next;
} map_entry;

void __rehash(map* this, u64 bucket_length) {
  map_entry** old_it = this->buckets;
  this->buckets = memory_alloc0(bucket_length);
  if (this->length) {
    while (true) {
      map_entry* node = *old_it;
      while (node) {
        map_entry** it = this->buckets;
        it += node->hash % bucket_length;
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
  memory_free(old_it);
  this->buckets_length = bucket_length;
}

map* _map_new(u64 stride) {
  map* this = memory_alloc(sizeof(map));
  this->buckets = memory_alloc0(sizeof(map_entry**) * MIN_HASH_SIZE);
  this->buckets_length = MIN_HASH_SIZE;
  this->length = 0;
  this->stride = stride;
  return this;
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
  memory_free(this);
}

void* map_get(const map* this, const u64 hash) {
  printf("map_get\n");
  map_entry* node = *((map_entry**)this->buckets + hash % this->buckets_length);
  while (node) {
    if (hash == node->hash) {
      return node->value;
    }
    node = node->next;
  }
  printf("!map_get\n");
  return 0;
}
void map_set(map* this, const u64 hash, const void* value) {
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
bool map_delete(map* this, const u64 hash) {
  if (!this->length)
    return false;
  map_entry** it = this->buckets;
  it += hash % this->buckets_length;
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