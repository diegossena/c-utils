#pragma once

#include <sdk/types.h>
#include <sdk/window.h>
#include <sdk/keyboard.h>
#include <sdk/memory.h>
#include <sdk/time.h>

typedef void (*game_onupdate_cb)(void* context, f32 elapsed_time);
typedef void (*game_onmouse_cb)(void* context, vector2d_t cursor);
typedef void (*game_onevent_cb)(void* context);

typedef struct game_update_ev_t {
  queue_t queue;
  void* context;
  game_onupdate_cb callback;
} game_update_ev_t;
typedef struct game_mouse_ev_t {
  queue_t queue;
  void* context;
  game_onmouse_cb callback;
} game_mouse_ev_t;
typedef struct game_event_t {
  queue_t queue;
  void* context;
  game_onevent_cb callback;
} game_event_t;

typedef struct game_t {
  window_t* window;
  // timer
  f64 last_update;

  // events
  // game_update_event_t
  queue_t onupdate;
  // game_mouse_event_t
  queue_t onmousemove;
  // game_mouse_event_t
  queue_t onmousedown;
  // game_mouse_event_t
  queue_t onmouseup;
  // game_mouse_event_t
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
  queue_foreach(game_update_ev_t, this->onupdate, it, it->queue.next) {
    it->callback(it->context, elapsed_time);
  }
}
void game_onmousemove(game_t* this, vector2d_t cursor) {
  queue_foreach(game_mouse_ev_t, this->onmousemove, it, it->queue.next) {
    it->callback(it->context, cursor);
  }
}
void game_onmouseup(game_t* this, vector2d_t cursor) {
  queue_foreach(game_mouse_ev_t, this->onmouseup, it, it->queue.next) {
    it->callback(it->context, cursor);
  }
}
void game_onmousedown(game_t* this, vector2d_t cursor) {
  queue_foreach(game_mouse_ev_t, this->onmousedown, it, it->queue.next) {
    it->callback(it->context, cursor);
  }
}
void game_free(game_t* this) {
  queue_foreach(game_event_t, this->ondestroy, it, it->queue.next) {
    it->callback(it->context);
  }
  memory_free(this);
}