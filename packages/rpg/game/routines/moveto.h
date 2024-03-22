#pragma once

#include "../game.h"
#include "../entities/entity.h"
#include <sdk/events.h>

typedef struct moveto_t {
  event_listener_t onupdate;
  event_listener_t ondestroy;
  callback_t destroy_listener;
  callback_t update_listener;
  game_t* game;
  vector2d_t* position;
  vector2d_t start;
  vector2d_t target;
  vector2d_t distance;
  f32 duration;
  f32 timer;
} moveto_t;

typedef struct moveto_props_t {
  // required
  game_t* game;
  vector2d_t* position;
  vector2d_t target;
  f32 duration;
  // optional
  callback_t onupdate;
  callback_t ondestroy;
} moveto_props_t;

void routine_moveto_destroy(moveto_t* this) {
  emitter_off(&this->onupdate);
  emitter_off(&this->ondestroy);
  memory_free(this);
}
void routine_moveto_update(moveto_t* this) {
  this->timer += this->game->elapsed_time;
  f32 tick = this->timer / this->duration;
  if (tick > 1.f) {
    tick = 1.f;
  }
  this->position->x = tick * this->distance.x + this->start.x;
  this->position->y = tick * this->distance.y + this->start.y;

  console_log("this->position->y=%f", this->position->y);
  if (this->timer >= this->duration) {
    if (this->destroy_listener.callback) {
      this->destroy_listener.callback(this->destroy_listener.context);
    }
    routine_moveto_destroy(this);
  }
}

void routine_moveto(moveto_props_t props) {
  moveto_t* this = memory_alloc(sizeof(moveto_t));
  this->game = props.game;
  this->position = props.position;
  this->start = *props.position;
  this->target = props.target;
  this->distance.x = props.target.x - props.position->x;
  this->distance.y = props.target.y - props.position->y;
  this->duration = math_max(props.duration, .001f);
  this->timer = 0.f;
  this->destroy_listener = props.ondestroy;
  this->update_listener = props.onupdate;
  // register
  this->onupdate.callback = (listener_t)routine_moveto_update;
  this->onupdate.context = this;
  emitter_prepend(&props.game->onupdate, &this->onupdate);
  this->ondestroy.callback = (listener_t)routine_moveto_destroy;
  this->ondestroy.context = this;
  emitter_prepend(&props.game->ondestroy, &this->ondestroy);
}