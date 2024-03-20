#pragma once

#include <sdk/window.h>
#include <sdk/keyboard.h>
#include <sdk/memory.h>
#include <sdk/time.h>
#include <sdk/queue.h>
#include <sdk/events.h>

typedef void (*onupdate_callback_t)(void* context, f32 elapsed_time);
typedef void (*onmouse_callback_t)(void* context, vector2d_t cursor);

typedef struct update_listener_t {
  queue_t queue;
  void* context;
  onupdate_callback_t callback;
} update_listener_t;
typedef struct mouse_listener_t {
  queue_t queue;
  void* context;
  onmouse_callback_t callback;
} mouse_listener_t;

typedef struct game_t {
  window_t* window;
  // timer
  f64 last_update;

  // events

  // update_listener_t
  queue_t onupdate;
  // mouse_listener_t
  queue_t onmousemove;
  // mouse_listener_t
  queue_t onmousedown;
  // mouse_listener_t
  queue_t onmouseup;
  // listener_t
  queue_t ondestroy;
} game_t;

#include "./entities/entity.h"
#include "./tilemaps/tilemap.h"
#include "./scene/title_screen.h"

game_t* game_new(window_t* window) {
  game_t* this = memory_alloc(sizeof(game_t));
  queue_head(&this->onupdate);
  queue_head(&this->onmousemove);
  queue_head(&this->onmousedown);
  queue_head(&this->onmouseup);
  queue_head(&this->ondestroy);
  this->window = window;
  this->last_update = time_absolute();
  titlescreen_load(this);
  return this;
}
void game_onupdate(game_t* this) {
  f64 now = time_absolute();
  f32 elapsed_time = now - this->last_update;
  this->last_update = now;
  queue_foreach(update_listener_t, this->onupdate, it, it->queue.next) {
    it->callback(it->context, elapsed_time);
  }
}
void game_onmousemove(game_t* this, vector2d_t cursor) {
  queue_foreach(mouse_listener_t, this->onmousemove, it, it->queue.next) {
    it->callback(it->context, cursor);
  }
}
void game_onmouseup(game_t* this, vector2d_t cursor) {
  queue_foreach(mouse_listener_t, this->onmouseup, it, it->queue.next) {
    it->callback(it->context, cursor);
  }
}
void game_onmousedown(game_t* this, vector2d_t cursor) {
  queue_foreach(mouse_listener_t, this->onmousedown, it, it->queue.next) {
    it->callback(it->context, cursor);
  }
}
void game_free(game_t* this) {
  emitter_emit(&this->ondestroy);
  memory_free(this);
}