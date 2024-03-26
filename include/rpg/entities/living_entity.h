#pragma once

#include <sdk/types.h>

typedef struct living_entity_t {
  f32 speed;

  f32 health;
      // Max health of this entity
  f32 max_health;
  // Amount this entity can heal per tick.
  f32 healing_per_tick;
} living_entity_t;