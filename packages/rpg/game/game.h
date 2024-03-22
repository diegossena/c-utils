#pragma once

#include <sdk/window.h>
#include <sdk/keyboard.h>
#include <sdk/memory.h>
#include <sdk/time.h>
#include <sdk/queue.h>
#include <sdk/events.h>

typedef struct game_t {
  window_t* window;
  bool __in_transition;
  // timer
  f64 __last_update;
  f32 elapsed_time;
  // listeners
  queue_t onupdate;
  queue_t onmousemove;
  queue_t onmousedown;
  queue_t onmouseup;
  queue_t onkeydown;
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
  queue_head(&this->onkeydown);
  queue_head(&this->ondestroy);
  this->window = window;
  this->__last_update = time_absolute();
  gfx_font_load(
    window, L".\\assets\\fonts\\zelda-font.ttf"
  );
  gfx_font_load(
    window, L".\\assets\\fonts\\megaman_2.ttf"
  );
  return this;
}
void game_oncreate(game_t* this) {
  titlescreen_load(this);
}
void game_onupdate(game_t* this) {
  f64 now = time_absolute();
  this->elapsed_time = now - this->__last_update;
  this->__last_update = now;
  queue_foreach(event_listener_t, this->onupdate, it, it->queue.next) {
    it->callback(it->context);
  }
}
void game_onmousemove(game_t* this) {
  queue_foreach(event_listener_t, this->onmousemove, it, it->queue.next) {
    it->callback(it->context);
  }
}
void game_onmouseup(game_t* this) {
  queue_foreach(event_listener_t, this->onmouseup, it, it->queue.next) {
    it->callback(it->context);
  }
}
void game_onmousedown(game_t* this) {
  queue_foreach(event_listener_t, this->onmousedown, it, it->queue.next) {
    it->callback(it->context);
  }
}
void game_onkeydown(game_t* this) {
  queue_foreach(event_listener_t, this->onkeydown, it, it->queue.next) {
    it->callback(it->context);
  }
}
void game_free(game_t* this) {
  emitter_emit(&this->ondestroy);
  memory_free(this);
}