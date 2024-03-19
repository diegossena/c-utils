#pragma once

#include "./routines.h"
#include "../entities/entity.h"

typedef struct moveto_t {
  routine_t routine;
  rect_t* rect;
  f32 start_x;
  f32 start_y;
  f32 target_x;
  f32 target_y;
  f32 duration;
  f32 timer;
} moveto_t;

void routine_moveto_update(moveto_t* this) {

}

routine_t* routine_moveto(moveto_t props) {
  routine_t* this = malloc(sizeof(routine_t));
  this->update = routine_moveto_update;
}