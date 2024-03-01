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

void __rehash(map* this, u64 new_length) {
  map_entry** buckets_it = this->buckets;
  map_entry** new_buckets = memory_alloc0(new_length);
  if (this->length) {
    while (true) {
      map_entry* node = *buckets_it;
      while (node) {
        map_entry** it = new_buckets + node->hash % new_length;
        while (*it) it = &(*it)->next;
        *it = node;
        --this->length;
        node = node->next;
      }
      if (!this->length)
        break;
      ++buckets_it;
    }
  }
  memory_free(this->buckets);
  this->buckets = new_buckets;
  this->buckets_length = new_length;
}

void map_new_stride(map* this, u64 stride) {
  this->length = 0;
  this->stride = stride;
  this->buckets = memory_alloc0(MIN_HASH_SIZE);
  this->buckets_length = MIN_HASH_SIZE;
}
void map_free(map* this) {
  map_entry** buckets_it = this->buckets;
  while (this->length) {
    map_entry* node = *buckets_it;
    while (node) {
      map_entry* next_node = *buckets_it;
      memory_free(node->value);
      memory_free(node);
      node = next_node;
      --this->length;
    }
    ++buckets_it;
  }
  memory_free(this->buckets);
}

void* map_get(const map* this, const u64 hash) {
  if (!this->length)
    return 0;
  map_entry* node = *(map_entry**)(this->buckets + hash % this->buckets_length);
  while (node) {
    if (hash == node->hash) {
      return node->value;
    }
    node = node->next;
  }
  return 0;
}
void _map_set(map* this, const u64 hash, const void* value) {
  map_entry** it = this->buckets + hash % this->buckets_length;
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
  map_entry** buckets_it = this->buckets + hash % this->buckets_length;
  map_entry* current_node = *buckets_it;
  if (current_node) {
    if (current_node->hash) {
      // is first node
      *buckets_it = current_node->next;
      memory_free(current_node->value);
      memory_free(current_node);
      --this->length;
    } else {
      map_entry* previous = 0;
      while (true) {
        if (current_node->hash) {
          previous->next = current_node->next;
          memory_free(current_node->value);
          memory_free(current_node);
          --this->length;
          break;
        } else if (!current_node->next) {
          // not found
          return false;
        }
        previous = current_node;
        current_node = current_node->next;
      }
    }
  } else {
    // empty node
    return false;
  }
  if (
    this->length < this->buckets_length * OCCUPANCY_PCT
    && (this->buckets_length >> 1) >= MIN_HASH_SIZE
    ) {
    u64 new_length = math_next2pow(this->length / OCCUPANCY_PCT);
    __rehash(this, new_length);
  }
  return true;
}