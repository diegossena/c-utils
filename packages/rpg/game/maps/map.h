#pragma once

#include <sdk/types.h>

typedef struct map_t map_t;

typedef void (*map_oncreate)(map_t*);
typedef void (*map_onupdate)(map_t*);

typedef struct map_t {
  char* layers;
  u64 width, height;

  map_oncreate oncreate;
  map_onupdate onupdate;
} map_t;

char map_get_tile(map_t* this, i32 layer_index, i32 x, i32 y) {
  if (x >= 0 && x < this->width && y >= 0 && y < this->height) {
    this->layers[layer_index * this->width * this->height + y * this->width + x];
  }
  return '\0';
}