#pragma once

typedef struct game_context_t game_context_t;

#include <sdk/types.h>
#include <sdk/window.h>
#include <sdk/keyboard.h>
#include <sdk/memory.h>
#include <sdk/time.h>
#include "entities/entity.h"

typedef struct game_context_t {
  window_t* window;
  queue_t entities;
  f64 last_update;
  f32 elapsed_time;
} game_context_t;

game_context_t* game_new(window_t* window) {
  game_context_t* this = memory_alloc(sizeof(game_context_t));
  this->entities.next = &this->entities;
  this->entities.prev = &this->entities;
  this->window = window;
  this->last_update = time_absolute();
  return this;
}
void game_free(game_context_t* this) {
  memory_free(this);
}

void game_update(game_context_t* this) {
  // timer
  f64 now = time_absolute();
  this->elapsed_time = now - this->last_update;
  this->last_update = now;
  // handle inputs
  if (keyboard_pressed(KEY_UP)) {

  } else if (keyboard_pressed(KEY_DOWN)) {

  }
  if (keyboard_pressed(KEY_LEFT)) {

  } else if (keyboard_pressed(KEY_RIGHT)) {

  }
  // entities
  for (entity_t* it = (entity_t*)this->entities.next; it != (entity_t*)&this->entities; it = (entity_t*)it->queue.next) {
    it->update(it);
  }
  console_log("update");
}