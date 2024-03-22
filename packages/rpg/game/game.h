#pragma once

#include <sdk/window.h>
#include <sdk/keyboard.h>
#include <sdk/memory.h>
#include <sdk/time.h>
#include <sdk/queue.h>
#include <sdk/events.h>
#include <sdk/window/gfx/color.h>
#include <sdk/window/gfx/stroke.h>
#include <sdk/window/gfx/style.h>

typedef struct game_t {
  window_t* window;
  bool __in_transition;
  // timer
  f64 __last_update;
  f32 elapsed_time;
  // listeners
  queue_t onupdate;
  queue_t ondraw;
  queue_t onmousemove;
  queue_t onmousedown;
  queue_t onmouseup;
  queue_t onkeydown;
  queue_t ondestroy;
  // assets
  const wchar_t* font_zelda_family;
  const wchar_t* font_megaman_family;
  const gfx_style_t dialog_style;
  const gfx_stroke_t stroke_solid;
  const gfx_color_t white;
  const gfx_color_t green;
  const gfx_color_t darkblue;
} game_t;

#include "./entities/entity.h"
#include "./tilemaps/tilemap.h"
#include "./scene/title_screen.h"

game_t* game_new(window_t* window) {
  game_t* this = memory_alloc(sizeof(game_t));
  this->__in_transition = false;
  // event_listeners
  queue_head(&this->onupdate);
  queue_head(&this->ondraw);
  queue_head(&this->onmousemove);
  queue_head(&this->onmousedown);
  queue_head(&this->onmouseup);
  queue_head(&this->onkeydown);
  queue_head(&this->ondestroy);
  // timer
  this->window = window;
  this->__last_update = time_absolute();
  // fonts
  gfx_font_load(
    window, L".\\assets\\fonts\\zelda-font.ttf"
  );
  this->font_zelda_family = L"TLOZ Minish Cap/A Link to the Past/Four Sword";
  gfx_font_load(
    window, L".\\assets\\fonts\\megaman_2.ttf"
  );
  this->font_megaman_family = L"MegaMan 2";
  // assets
  gfx_stroke_new((gfx_stroke_t*)&this->stroke_solid, window, (gfx_stroke_props_t) { STROKE_STYLE_SOLID });
  gfx_color_new((gfx_color_t*)&this->white, window, COLOR_WHITE);
  gfx_color_new((gfx_color_t*)&this->green, window, COLOR_GREEN);
  gfx_color_new((gfx_color_t*)&this->darkblue, window, COLOR_DARKBLUE);
  return this;
}
void game_onload(game_t* this) {
  window_t* window = this->window;
  gfx_style_new((gfx_style_t*)&this->dialog_style, (gfx_style_props_t) {
    .window = window,
      .size = 28.f,
      .family = this->font_megaman_family,
      .weight = FONT_WEIGHT_NORMAL
  });
  titlescreen_load(this);
}
void game_onupdate(game_t* this) {
  f64 now = time_absolute();
  this->elapsed_time = now - this->__last_update;
  this->__last_update = now;
  queue_foreach(event_listener_t, this->onupdate) {
    it->callback(it->context);
  }
}
void game_ondraw(game_t* this) {
  queue_foreach(event_listener_t, this->ondraw) {
    it->callback(it->context);
  }
}
void game_onmousemove(game_t* this) {
  queue_foreach(event_listener_t, this->onmousemove) {
    it->callback(it->context);
  }
}
void game_onmouseup(game_t* this) {
  queue_foreach(event_listener_t, this->onmouseup) {
    it->callback(it->context);
  }
}
void game_onmousedown(game_t* this) {
  queue_foreach(event_listener_t, this->onmousedown) {
    it->callback(it->context);
  }
}
void game_onkeydown(game_t* this) {
  queue_foreach(event_listener_t, this->onkeydown) {
    it->callback(it->context);
  }
}
void game_free(game_t* this) {
  emitter_emit(&this->ondestroy);
  gfx_style_free((gfx_style_t*)&this->dialog_style);
  gfx_stroke_free((gfx_stroke_t*)&this->stroke_solid);
  gfx_color_free((gfx_color_t*)&this->white);
  gfx_color_free((gfx_color_t*)&this->green);
  gfx_color_free((gfx_color_t*)&this->darkblue);
  memory_free(this);
}