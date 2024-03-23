#pragma once

#include <sdk/types.h>

typedef struct tilemap_t {
  const char* layers;
  u64 width, height;
} tilemap_t;

char tilemap_tile_get(tilemap_t* this, i32 layer_index, i32 x, i32 y) {
  if (x >= 0 && x < this->width && y >= 0 && y < this->height) {
    this->layers[layer_index * this->width * this->height + y * this->width + x];
  }
  return '\0';
}
void tilemap_draw(tilemap_t* this, u64 layer_index);