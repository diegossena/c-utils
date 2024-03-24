#pragma once

#include <sdk/string.h>
#include <sdk/window.h>
#include <sdk/window/gfx/image.h>
#include "../entities/entity.h"

typedef struct item_t {
  queue_t queue;
  string_t name;
  string_t description;
  gfx_image_src_t* sprite;
  void (*onuse)(entity_t*);
} item_t;