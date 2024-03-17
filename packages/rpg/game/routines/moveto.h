#pragma once

#include "./routines.h"
#include "../entities/entity.h"

typedef struct moveto_t {
  routine_t routine;
  entity_t* entity;
  f32 start_x;
  f32 start_y;
  f32 target_x;
  f32 target_y;
  f32 duration;
  f32 timer;
} moveto_t;