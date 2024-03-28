#pragma once

typedef struct game_t {
  window_t* window;
  event_listener_t ondestroy;
  // assets
  gfx_image_src_t pallet_town_interiors;
  gfx_image_src_t character_png;
  gfx_text_style_t dialog_style;
  gfx_stroke_t stroke_solid;
  gfx_color_t white;
  gfx_color_t green;
  gfx_color_t darkblue;
  const wchar_t* font_zelda_family;
  const wchar_t* font_megaman_family;
} game_t;

#include <rpg/entities/entity.h>
#include <rpg/scenes/title_screen.h>

void game_destroy(game_t* this) {
  emitter_off(&this->ondestroy);
  gfx_text_style_free((gfx_text_style_t*)&this->dialog_style);
  gfx_stroke_free((gfx_stroke_t*)&this->stroke_solid);
  gfx_color_free((gfx_color_t*)&this->white);
  gfx_color_free((gfx_color_t*)&this->green);
  gfx_color_free((gfx_color_t*)&this->darkblue);
  gfx_image_src_free(&this->pallet_town_interiors);
  gfx_image_src_free(&this->character_png);
  memory_free(this);
}
void game_onpreload(window_t* window) {
  game_t* this = memory_alloc0(sizeof(game_t));
  // props
  this->window = window;
  window->context = this;
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
  game_t* this = window->context;
  // register
  this->ondestroy = (event_listener_t) {
    .callback = (listener_t)game_destroy,
    .context = this
  };
  emitter_on(&window->onclose, &this->ondestroy);
  // assets
  gfx_text_style_new((gfx_text_style_t*)&this->dialog_style, (text_style_props_t) {
    .window = window,
      .size = 28.f,
      .family = this->font_megaman_family,
      .weight = FONT_WEIGHT_NORMAL
  });
  gfx_stroke_new(&this->stroke_solid, (gfx_stroke_props_t) {
    window, STROKE_STYLE_SOLID
  });
  gfx_color_new(&this->white, window, COLOR_WHITE);
  gfx_color_new(&this->green, window, COLOR_GREEN);
  gfx_color_new(&this->darkblue, window, COLOR_DARKBLUE);
  scene_titlescreen_load(this);
}