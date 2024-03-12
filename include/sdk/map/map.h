#pragma once

#include <sdk/types.h>
#include <sdk/math/math.h>

#define map_hash(data) math_jenkins_hash(data, sizeof(data))

#define map_new(type) _map_new(sizeof(type))
#define map_set_cstr(this, key, value) { \
  typeof(value) tmp = value;      \
  map_set(this, math_hash_jenkins(key, sizeof(key)), &tmp);         \
}
#define map_get_cstr(this, type, key) \
  (*map_get_cstr_p(this, type, key))
#define map_get_cstr_p(this, type, key) \
  (type*)map_get(this, math_hash_jenkins(key, sizeof(key)))
#define map_delete_cstr(this, key) \
  map_delete(this, math_hash_jenkins(key, sizeof(key)))

typedef struct map_entry map_entry;
typedef struct map_entry {
  u64 __hash;
  void* __value;
  map_entry* __next;
} map_entry;
typedef struct map_t {
  u64 __length;
  u64 __stride;
  map_entry** __buckets;
  u64 __buckets_length;
} map_t;

map_t* _map_new(u64 stride);
void map_free(map_t*);

void* map_get(const map_t*, const u64 hash);
void map_set(map_t*, const u64 hash, const void* value);
bool map_delete(map_t*, const u64 hash);

#include <sdk/map/map.inl.h>