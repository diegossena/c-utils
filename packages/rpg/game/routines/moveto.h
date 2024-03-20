#pragma once

#include "../game.h"
#include "../entities/entity.h"

typedef struct moveto_t {
  update_listener_t onupdate;
  event_listener_t ondestroy;
  // props
  game_t* game;
  vector2d_t* position;
  vector2d_t distance;
  f32 duration;
  f32 timer;
} moveto_t;


void routine_moveto_destroy(moveto_t* this) {
  queue_remove(&this->onupdate.queue);
  emitter_off(&this->ondestroy);
  memory_free(this);
}
void routine_moveto_update(moveto_t* this, f32 elapsed_time) {
  this->timer += elapsed_time;
  f32 tick = elapsed_time / this->duration;
  this->position->x += tick * this->distance.x;
  this->position->y += tick * this->distance.y;
  if (this->timer >= this->duration) {
    routine_moveto_destroy(this);
  }
}

void routine_moveto(game_t* game, vector2d_t* position, vector2d_t target, f32 duration) {
  moveto_t* this = memory_alloc(sizeof(moveto_t));
  this->game = game;
  this->position = position;
  this->distance.x = target.x - position->x;
  this->distance.y = target.y - position->y;
  this->duration = math_max(duration, .001f);
  this->timer = 0.f;
  // register
  this->onupdate = (update_listener_t) {
    .callback = (onupdate_callback_t)routine_moveto_update,
    .context = this,
  };
  queue_unshift(&game->onupdate, &this->onupdate.queue);
  this->ondestroy = (event_listener_t) {
    .callback = (listener_t)routine_moveto_destroy,
    .context = this,
  };
  queue_unshift(&game->ondestroy, &this->ondestroy.queue);
}