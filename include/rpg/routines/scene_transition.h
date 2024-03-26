#pragma once

#include "../game.h"
#include "../entities/entity.h"
#include <sdk/events.h>

typedef struct scene_transition_t {
  game_t* game;
  event_listener_t onupdate;
  event_listener_t ondraw;
  event_listener_t ondestroy;
  callback_t scene_destroy;
  callback_t scene_load;
  // private
  f32 timer, opacity;
  bool loading;
  gfx_rect_t background;
} scene_transition_t;

typedef struct scene_transition_props_t {
  game_t* game;
  callback_t scene_destroy;
  callback_t scene_load;
} scene_transition_props_t;

void scene_transition_destroy(scene_transition_t* this) {
  emitter_off(&this->onupdate);
  emitter_off(&this->ondraw);
  emitter_off(&this->ondestroy);
  this->game->__in_transition = false;
  memory_free(this);
}
void scene_transition_update(scene_transition_t* this) {
  static const f32 duration = .85f;
  game_t* game = this->game;
  window_t* window = game->window;
  this->timer += window->elapsed_time;
  f32 progress = this->timer / duration;
  if (progress > 1.f) {
    progress = 1.f;
  }
  if (this->loading) {
    this->opacity = progress;
    if (this->timer >= duration) {
      callback_emit(&this->scene_destroy);
      callback_emit(&this->scene_load);
      emitter_forward(&this->ondraw);
      this->timer = 0.f;
      this->loading = false;
    }
  } else {
    this->opacity = 1.f - progress;
    if (this->timer >= duration) {
      return scene_transition_destroy(this);
    }
  }
  window_render_request(window);
}
void scene_transition_draw(scene_transition_t* this) {
  window_t* window = this->game->window;
  gfx_color_t gfx_color;
  gfx_color_new(&gfx_color, window, (color_t) { 1.f, 1.f, 1.f, this->opacity });
  this->background.color = &gfx_color;
  gfx_rect_draw(&this->background);
  gfx_color_free(&gfx_color);
}

void scene_transition(scene_transition_props_t props) {
  game_t* game = props.game;
  if (game->__in_transition)
    return;
  game->__in_transition = true;
  scene_transition_t* this = memory_alloc(sizeof(scene_transition_t));
  window_t* window = game->window;
  // copy
  this->game = game;
  this->timer = 0.f;
  this->scene_destroy = props.scene_destroy;
  this->scene_load = props.scene_load;
  // init
  this->loading = true;
  this->background = (gfx_rect_t) {
    .window = window,
    .rect = { 0, 0, window->width, window->height }
  };
  // register
  this->onupdate.callback = (listener_t)scene_transition_update;
  this->onupdate.context = this;
  emitter_on(&window->onupdate, &this->onupdate);
  this->ondestroy.callback = (listener_t)scene_transition_destroy;
  this->ondestroy.context = this;
  emitter_on(&window->onclose, &this->ondestroy);
  this->ondraw.callback = (listener_t)scene_transition_draw;
  this->ondraw.context = this;
  emitter_on(&window->ondraw, &this->ondraw);
}