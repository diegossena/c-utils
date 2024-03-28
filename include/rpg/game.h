#pragma once

typedef struct game_t {
  window_t* window;
  event_listener_t ondestroy;
  // assets
  const wchar_t* font_zelda_family;
  const wchar_t* font_megaman_family;
} game_t;

#include <rpg/entities/entity.h>
#include <rpg/scenes/title_screen.h>

void game_destroy(game_t* this) {
  emitter_off(&this->ondestroy);
  memory_free(this);
}
void game_onpreload(window_t* window) {
  game_t* this = memory_alloc(sizeof(game_t));
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
  scene_titlescreen_load(this);
}