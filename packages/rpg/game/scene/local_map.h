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

} local_map_t;

void localmap_load(game_t*);
void localmap_destroy(local_map_t*);

void localmap_ondraw(local_map_t* this) {

}
void localmap_onkeydown(local_map_t* this) {

}
void localmap_destroy(local_map_t* this) {
  emitter_off(&this->ondraw);
  emitter_off(&this->onkeydown);
  emitter_off(&this->destroy);
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
  // assets

}