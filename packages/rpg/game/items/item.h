#pragma once

#include <sdk/string.h>
#include <sdk/window.h>
#include "../entities/entity.h"

typedef struct item_t {
  queue_t queue;
  string_t name;
  string_t description;
  bitmap_t* sprite;
  void (*onuse)(entity_t*);
} item_t;