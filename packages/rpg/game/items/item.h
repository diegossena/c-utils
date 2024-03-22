#pragma once

#include <sdk/string.h>
#include <sdk/window.h>
#include <sdk/window/gfx/bitmap.h>
#include "../entities/entity.h"

typedef struct item_t {
  queue_t queue;
  string_t name;
  string_t description;
  image_t* sprite;
  void (*onuse)(entity_t*);
} item_t;