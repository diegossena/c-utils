#pragma once

#include "../game.h"
#include "../entities/entity.h"

typedef struct moveto_t {
  game_update_ev_t onupdate;
  event_listener_t ondestroy;
  // props
  game_t* game;
  vector2d_t* position;
  vector2d_t start;
  vector2d_t target;
  f32 duration;
  f32 timer;
} moveto_t;


void routine_moveto_destroy(moveto_t* this) {
  queue_remove(&this->onupdate.queue);
  queue_remove(&this->ondestroy.queue);
  memory_free(this);
}
void routine_moveto_update(moveto_t* this, f32 elapsed_time) {
  this->timer += elapsed_time;
  f32 tick = this->timer / this->duration;
  if (tick > 1.f) tick = 1.f;

  this->position->x = (this->target.x - this->start.x) * tick + this->start.x;
  this->position->y = (this->target.y - this->start.y) * tick + this->start.y;

  if (this->timer >= this->duration) {
    this->position->x = this->target.x;
    this->position->y = this->target.y;
    routine_moveto_destroy(this);
  }
}

void routine_moveto(game_t* game, vector2d_t* position, vector2d_t target, f32 duration) {
  moveto_t* this = memory_alloc(sizeof(moveto_t));
  this->game = game;
  this->position = position;
  this->start = *position;
  this->target = target;
  this->duration = duration;
  // register
  this->onupdate = (game_update_ev_t) {
    .callback = routine_moveto_update,
    .context = this,
  };
  queue_push(&game->onupdate, &this->onupdate.queue);
  this->ondestroy = (event_listener_t) {
    .callback = routine_moveto_destroy,
    .context = this,
  };
  queue_push(&game->ondestroy, &this->ondestroy.queue);
}