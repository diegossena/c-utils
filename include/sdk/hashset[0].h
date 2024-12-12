#ifndef HASHSET_H
#define HASHSET_H

#include <sdk/types.h>
#include <sdk/math.h>
#include <sdk/memory.h>

#define __HASHSET_BUCKETS_SIZE 512

#define hashset_set_cstr(this, key, value) { \
  const char entry_key[] = key; \
  typeof(value) entry_value = value; \
  hashset_set(this, math_hash_jenkins(entry_key, sizeof(entry_key) - 1), &entry_value); \
}
#define hashset_get_cstrp(this, type, key) \
  (type*)hashset_get(this, math_hash_jenkins(key, sizeof(key) - 1))
#define hashset_get_cstr(this, type, key) \
  (*hashset_get_cstrp(this, type, key))
#define hashset_delete_cstr(this, key) \
  hashset_delete(this, math_hash_jenkins(key, sizeof(key) - 1))

typedef struct hashmap_entry_t hashmap_entry_t;
typedef struct hashmap_entry_t {
  u64 __hash;
  hashmap_entry_t* __next;
} hashmap_entry_t;

typedef struct hashset_t {
  const u64 length;
  hashmap_entry_t* __buckets[__HASHSET_BUCKETS_SIZE];
} hashset_t;

export void hashset_constructor(hashset_t* this);
export void hashset_deconstructor(hashset_t* this);
export bool hashset_contains(const hashset_t* this, const u64 hash);
export void hashset_add(hashset_t* this, const u64 hash);
export bool hashset_remove(hashset_t* this, const u64 hash);

#endif