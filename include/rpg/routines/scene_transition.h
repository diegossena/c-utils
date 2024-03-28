#pragma once

#include <sdk/events.h>

static bool __scene_in_transition = false;

typedef struct scene_transition_t {
  window_t* window;
  // callbacks
  callback_t scene_destroy;
  callback_t scene_load;
  // private
  f32 timer, opacity;
  bool loading;
  gfx_rect_t background;
  // listeners
  event_listener_t __onupdate;
  event_listener_t __ondraw;
  event_listener_t __ondestroy;
} scene_transition_t;

typedef struct scene_transition_props_t {
  window_t* window;
  callback_t scene_destroy;
  callback_t scene_load;
} scene_transition_props_t;

void __scene_transition_destroy(scene_transition_t* this) {
  emitter_off(&this->__onupdate);
  emitter_off(&this->__ondraw);
  emitter_off(&this->__ondestroy);
  __scene_in_transition = false;
  memory_free(this);
}
void __scene_transition_update(scene_transition_t* this) {
  static const f32 duration = .85f;
  window_t* window = this->window;
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
      emitter_forward(&this->__ondraw);
      this->timer = 0.f;
      this->loading = false;
    }
  } else {
    this->opacity = 1.f - progress;
    if (this->timer >= duration) {
      return __scene_transition_destroy(this);
    }
  }
  window_render_request(window);
}
void __scene_transition_draw(scene_transition_t* this) {
  window_t* window = this->window;
  gfx_color_t gfx_color;
  gfx_color_new(&gfx_color, window, (color_t) { 0, 0, 0, this->opacity });
  this->background.color = &gfx_color;
  gfx_rect_draw(&this->background);
  gfx_color_free(&gfx_color);
}

void scene_transition(scene_transition_props_t props) {
  window_t* window = props.window;
  if (__scene_in_transition)
    return;
  __scene_in_transition = true;
  scene_transition_t* this = memory_alloc(sizeof(scene_transition_t));
  // copy
  this->window = window;
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
  this->__onupdate.callback = (listener_t)__scene_transition_update;
  this->__onupdate.context = this;
  emitter_on(&window->onupdate, &this->__onupdate);
  this->__ondestroy.callback = (listener_t)__scene_transition_destroy;
  this->__ondestroy.context = this;
  emitter_on(&window->onclose, &this->__ondestroy);
  this->__ondraw.callback = (listener_t)__scene_transition_draw;
  this->__ondraw.context = this;
  emitter_on(&window->ondraw, &this->__ondraw);
}