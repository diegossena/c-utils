#pragma once

#include "../game.h"
#include "../entities/entity.h"

typedef struct moveto_t {
  event_listener_t onupdate;
  event_listener_t ondestroy;
  fn_any_t destroy_callback;
  fn_any_t update_callback;
  game_t* game;
  vector2d_t* position;
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
  f32 timer;
  // optional
  fn_any_t onupdate;
  fn_any_t ondestroy;
} moveto_props_t;

void routine_moveto_destroy(moveto_t* this) {
  emitter_off(&this->onupdate);
  emitter_off(&this->ondestroy);
  memory_free(this);
}
void routine_moveto_update(moveto_t* this) {
  this->timer += this->game->elapsed_time;
  f32 tick = this->game->elapsed_time / this->duration;
  this->position->x += tick * this->distance.x;
  this->position->y += tick * this->distance.y;
  if (this->timer >= this->duration) {
    if (this->destroy_callback) {
      this->destroy_callback();
    }
    routine_moveto_destroy(this);
  }
}

void routine_moveto(moveto_props_t props) {
  moveto_t* this = memory_alloc(sizeof(moveto_t));
  this->game = props.game;
  this->position = props.position;
  this->distance.x = props.target.x - props.position->x;
  this->distance.y = props.target.y - props.position->y;
  this->duration = math_max(props.duration, .001f);
  this->timer = 0.f;
  this->destroy_callback;
  this->update_callback;
  // register
  this->onupdate = (event_listener_t) {
    .callback = (listener_t)routine_moveto_update,
    .context = this,
  };
  queue_unshift(&props.game->onupdate, &this->onupdate.queue);
  this->ondestroy = (event_listener_t) {
    .callback = (listener_t)routine_moveto_destroy,
    .context = this,
  };
  queue_unshift(&props.game->ondestroy, &this->ondestroy.queue);
}