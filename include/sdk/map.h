#pragma once

#include <sdk/types.h>
#include <sdk/math.h>

#define map_hash(data) math_jenkins_hash(data, sizeof(data))
#define map_new(this, type) map_new_stride(this, sizeof(type))
#define map_set(this, hash, value) { \
  typeof(value) tmp = value;      \
  _map_set(this, hash, &tmp);         \
}

class map {
  u64 length;
  u64 stride;
  void* buckets;
  u64 buckets_length;
} map;

void map_new_stride(map*, u64 stride);
void map_free(map*);

void* map_get(const map*, const u64 hash);
void _map_set(map*, const u64 hash, const void* value);
bool map_delete(map*, const u64 hash);