#pragma once

#include <sdk/math/geometry_2d.h>
#include <sdk/queue.h>

typedef enum direction_t { SOUTH, WEST, NORTH, EAST } direction_t;

typedef struct entity_t entity_t;

typedef void (*entity_update)(entity_t*);
typedef void (*entity_draw)(entity_t*);

typedef struct entity_t {
  queue_t queue;
  game_context_t* context;
  vector2d_t position;
  direction_t direction;
  entity_update update;
  entity_draw draw;
} entity_t;