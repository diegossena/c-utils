#pragma once

#include <sdk/types.h>
#include <sdk/math.h>

#define map_hash(data) math_jenkins_hash(data, sizeof(data))
#define map_new(type) _map_new(sizeof(type))
#define map_set_cstr(this, key, value) { \
  typeof(value) tmp = value;      \
  map_set(this, math_jenkins_hash(key, sizeof(key)), &tmp);         \
}
#define map_get_cstr(this, type, key) \
  (*map_get_cstr_p(this, type, key))
#define map_get_cstr_p(this, type, key) \
  (type*)map_get(this, math_jenkins_hash(key, sizeof(key)))
#define map_delete_cstr(this, key) \
  map_delete(this, math_jenkins_hash(key, sizeof(key)))

class map map;

map* _map_new(u64 stride);
void map_free(map*);

void* map_get(const map*, const u64 hash);
void map_set(map*, const u64 hash, const void* value);
bool map_delete(map*, const u64 hash);