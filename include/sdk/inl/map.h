#ifdef SDK_MAP_H

#define MIN_HASH_SIZE 512ULL // table size when first created
#define OCCUPANCY_PCT 0.5 // large PCT means smaller and slower

void __map_rehash(tilemap_t* this, u64 bucket_length) {
  map_entry** old_it = this->__buckets;
  this->__buckets = memory_alloc0(bucket_length);
  if (this->__length) {
    while (true) {
      map_entry* node = *old_it;
      while (node) {
        map_entry** it = this->__buckets;
        it += node->__hash % bucket_length;
        while (*it) it = &(*it)->__next;
        *it = node;
        --this->__length;
        node = node->__next;
      }
      if (!this->__length)
        break;
      ++old_it;
    }
  }
  memory_free(old_it);
  this->__buckets_length = bucket_length;
}

void _map_constructor(tilemap_t* this, u32 stride) {
  this->__buckets = memory_alloc0(sizeof(this->__buckets) * MIN_HASH_SIZE);
  this->__buckets_length = MIN_HASH_SIZE;
  this->__stride = stride;
  this->__length = 0;
}
void map_deconstructor(tilemap_t* this) {
  map_entry** it = this->__buckets;
  while (true) {
    map_entry* node = *it;
    while (node) {
      map_entry* node_next = node->__next;
      memory_free(node->__value);
      memory_free(node);
      --this->__length;
      node = node_next;
    }
    if (!this->__length)
      break;
    ++it;
  }
  memory_free(this->__buckets);
}

void* map_get(const tilemap_t* this, const u64 hash) {
  map_entry* node = *(this->__buckets + hash % this->__buckets_length);
  while (node) {
    if (hash == node->__hash) {
      return node->__value;
    }
    node = node->__next;
  }
  return 0;
}
void map_set(tilemap_t* this, const u64 hash, const void* value) {
  map_entry** it = this->__buckets;
  it += hash % this->__buckets_length;
  while (true) {
    if (!*it) {
      // new entry
      *it = memory_alloc(sizeof(map_entry));
      (*it)->__hash = hash;
      (*it)->__next = 0;
      (*it)->__value = memory_alloc(this->__stride);
      memory_copy((*it)->__value, value, this->__stride);
      ++this->__length;
      break;
    } else if ((*it)->__hash == hash) {
      // overwrite entry
      memory_copy((*it)->__value, value, this->__stride);
      break;
    }
    it = &(*it)->__next;
  };
  if (this->__length >= this->__buckets_length * OCCUPANCY_PCT) {
    u64 new_length = math_next2pow(this->__length / OCCUPANCY_PCT);
    __map_rehash(this, new_length);
  }
}
bool map_delete(tilemap_t* this, const u64 hash) {
  if (!this->__length)
    return false;
  map_entry** it = this->__buckets;
  it += hash % this->__buckets_length;
  while (*it) {
    if ((*it)->__hash == hash) {
      map_entry* node_next = (*it)->__next;
      memory_free((*it)->__value);
      memory_free((*it));
      *it = node_next;
      --this->__length;
      if (
        this->__length < this->__buckets_length * OCCUPANCY_PCT
        && (this->__buckets_length >> 1) >= MIN_HASH_SIZE
        ) {
        u64 new_length = math_next2pow(this->__length / OCCUPANCY_PCT);
        __map_rehash(this, new_length);
      }
      return true;
    }
    it = &(*it)->__next;
  }
  // not found
  return false;
}

#undef MIN_HASH_SIZE
#undef OCCUPANCY_PCT

#endif