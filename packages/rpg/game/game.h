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
#include <sdk/window/gfx/image.h>

typedef struct game_t {
  window_t* window;
  bool __in_transition;
  event_listener_t ondestroy;
  // assets
  image_src_t terrain_atlas;
  gfx_style_t dialog_style;
  gfx_stroke_t stroke_solid;
  gfx_color_t white;
  gfx_color_t green;
  gfx_color_t darkblue;
  const wchar_t* font_zelda_family;
  const wchar_t* font_megaman_family;
} game_t;

#include "./entities/entity.h"
#include "./scene/title_screen.h"

void game_destroy(game_t* this) {
  emitter_off(&this->ondestroy);
  gfx_style_free((gfx_style_t*)&this->dialog_style);
  gfx_stroke_free((gfx_stroke_t*)&this->stroke_solid);
  gfx_color_free((gfx_color_t*)&this->white);
  gfx_color_free((gfx_color_t*)&this->green);
  gfx_color_free((gfx_color_t*)&this->darkblue);
  gfx_image_src_free(&this->terrain_atlas);
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
    .callback = (listener_t)game_destroy,
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
}
void game_onload(window_t* window) {
  game_t* this = (game_t*)window->context;
  gfx_style_new((gfx_style_t*)&this->dialog_style, (gfx_style_props_t) {
    .window = window,
      .size = 28.f,
      .family = this->font_megaman_family,
      .weight = FONT_WEIGHT_NORMAL
  });
  gfx_image_src_new(&this->terrain_atlas, L"./assets/sprites/terrain_atlas.png", window);
  gfx_stroke_new(&this->stroke_solid, window, (gfx_stroke_props_t) { STROKE_STYLE_SOLID });
  gfx_color_new(&this->white, window, COLOR_WHITE);
  gfx_color_new(&this->green, window, COLOR_GREEN);
  gfx_color_new(&this->darkblue, window, COLOR_DARKBLUE);
  // titlescreen_load(this);
  localmap_load(this);
}