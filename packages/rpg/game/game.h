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
  event_listener_t ondestroy;
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

void game_ondestroy(game_t* this) {
  gfx_style_free((gfx_style_t*)&this->dialog_style);
  gfx_stroke_free((gfx_stroke_t*)&this->stroke_solid);
  gfx_color_free((gfx_color_t*)&this->white);
  gfx_color_free((gfx_color_t*)&this->green);
  gfx_color_free((gfx_color_t*)&this->darkblue);
  memory_free(this);
}
void game_onpreload(window_t* window) {
  game_t* this = memory_alloc(sizeof(game_t));
  // props
  this->window = window;
  window->context = this;
  this->__in_transition = false;
  // register
  this->ondestroy = (event_listener_t) {
    .callback = (listener_t)game_ondestroy,
    .context = this
  };
  emitter_on(&window->onclose, &this->ondestroy);
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
}
void game_onload(window_t* window) {
  game_t* this = (game_t*)window->context;
  gfx_style_new((gfx_style_t*)&this->dialog_style, (gfx_style_props_t) {
    .window = window,
      .size = 28.f,
      .family = this->font_megaman_family,
      .weight = FONT_WEIGHT_NORMAL
  });
  titlescreen_load(this);
}