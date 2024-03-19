#pragma once

#include <sdk/types.h>
#include "../items/item.h"
#include "./scene.h"

typedef struct inventory_t {
  scene_t scene;
  queue_t list;
  game_t* state;
  u32 selected_x, selected_y;
} inventory_t;

void inventory_constructor(inventory_t* this) {
  queue_head(&this->list);
}

void inventory_render(game_t* this) {

}