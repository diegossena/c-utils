#pragma once

#include <sdk/types.h>
#include <sdk/window.h>
#include <sdk/keyboard.h>
#include <sdk/memory.h>
#include <sdk/time.h>

typedef struct game_t game_t;
typedef struct scene_t scene_t;

typedef struct game_t {
  window_t* window;
  queue_t routines;
  // queue_t objects;
  scene_t* scene;
  // timer
  f64 last_update;
  f32 elapsed_time;
} game_t;

#include "./scenes/scene.h"
#include "./entities/entity.h"
#include "./routines/routines.h"
#include "./tilemaps/tilemap.h"
#include "./scenes/title_screen.h"

game_t* game_new(window_t* window) {
  game_t* this = memory_alloc(sizeof(game_t));
  // queue_head(&this->entities);
  this->window = window;
  this->last_update = time_absolute();
  queue_head(&this->routines);
  // queue_head(&this->objects);
  title_screen_load(this);
  return this;
}
void game_onupdate(game_t* this) {
  // timer
  f64 now = time_absolute();
  this->elapsed_time = now - this->last_update;
  this->last_update = now;
  // routines
  routine_t* routine_it = (routine_t*)this->routines.next;
  while (routine_it != (routine_t*)&this->routines) {
    routine_t* next = (routine_t*)routine_it->queue.next;
    routine_it->onupdate(routine_it);
    routine_it = next;
  }
  this->scene->onupdate(this);
}
void game_onmousemove(game_t* this, vector2d_t cursor) {
  this->scene->onmousemove(this, cursor);
}
void game_onmouseup(game_t* this, vector2d_t cursor) {
  this->scene->onmouseup(this, cursor);
}
void game_onmousedown(game_t* this, vector2d_t cursor) {
  this->scene->onmousedown(this, cursor);
}
void game_free(game_t* this) {
  memory_free(this->scene);
  memory_free(this);
}