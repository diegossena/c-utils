#pragma once

#include <sdk/window/gfx/style.h>
#include <sdk/window/gfx/rect.h>
#include <sdk/window/gfx/text.h>
#include <sdk/window/gfx/image.h>

#include "../game.h"
#include "../routines/showdialog.h"

typedef struct local_map_t {
  game_t* game;
  // events
  event_listener_t ondraw;
  event_listener_t onkeydown;
  event_listener_t destroy;
  // assets
  image_src_t terrain_atlas;
  gfx_style_t title_style, play_style;
  // elements
  gfx_image_t background;
  gfx_text_t title, press_space, to_play;
  showdialog_t hp_display;
} local_map_t;

void localmap_load(game_t*);
void localmap_destroy(local_map_t*);

void localmap_ondraw(local_map_t* this) {
  gfx_bitmap_draw(&this->background);
  gfx_text_draw(&this->title);
  gfx_text_draw(&this->press_space);
  gfx_text_draw(&this->to_play);
  showdialog_draw(&this->hp_display);
}
void localmap_onkeydown(local_map_t* this) {

}
void localmap_destroy(local_map_t* this) {
  emitter_off(&this->ondraw);
  emitter_off(&this->onkeydown);
  emitter_off(&this->destroy);
  showdialog_free(&this->hp_display);
  gfx_style_free(&this->title_style);
  gfx_style_free(&this->play_style);
  gfx_bitmap_free(&this->terrain_atlas);
  memory_free(this);
}
void localmap_load(game_t* game) {
  local_map_t* this = memory_alloc(sizeof(local_map_t));
  window_t* window = game->window;
  this->game = game;
  // register
  this->ondraw = (event_listener_t) {
    .callback = (listener_t)localmap_ondraw,
    .context = this
  };
  emitter_on(&window->ondraw, &this->ondraw);
  this->destroy = (event_listener_t) {
    .callback = (listener_t)localmap_destroy,
    .context = this
  };
  emitter_on(&window->onclose, &this->destroy);
  this->onkeydown = (event_listener_t) {
    .callback = (listener_t)localmap_onkeydown,
    .context = this
  };
  emitter_on(&window->onkeydown, &this->onkeydown);
  // assets

}