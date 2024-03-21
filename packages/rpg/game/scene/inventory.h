#pragma once

#include <sdk/types.h>
#include "../items/item.h"

typedef struct inventory_t {
  game_t* game;
  queue_t items; // item_t
  u32 selected_x, selected_y;
} inventory_t;