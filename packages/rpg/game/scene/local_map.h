#pragma once

#include <sdk/window/gfx/style.h>
#include <sdk/window/gfx/rect.h>
#include <sdk/window/gfx/text.h>
#include <sdk/window/gfx/image.h>

#include "../game.h"
#include "../tilemaps/tilemap.h"

typedef struct local_map_t {
  game_t* game;
  // event_listener_t
  event_listener_t ondraw;
  event_listener_t onkeydown;
  event_listener_t destroy;
  // elements
  showdialog_t hp_display;
} local_map_t;

// void localmap_load(game_t*);
// void localmap_destroy(local_map_t*);

void localmap_ondraw(local_map_t* this) {
  showdialog_draw(&this->hp_display);
}
void localmap_onkeydown(local_map_t* this) {

}
void localmap_destroy(local_map_t* this) {
  emitter_off(&this->ondraw);
  emitter_off(&this->onkeydown);
  emitter_off(&this->destroy);
  showdialog_free(&this->hp_display);
  memory_free(this);
}
void localmap_load(game_t* game) {
  // init
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

  // elements

  // hp_display
  this->hp_display.game = game;
  showdialog_new(&this->hp_display);
  wstring_append_cwstr(&this->hp_display.text, L"HP: 10/10");
  this->hp_display.position.x = 10.f;
  this->hp_display.position.y = 10.f;
  showdialog_update(&this->hp_display);
}