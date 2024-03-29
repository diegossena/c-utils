#pragma once

#include <sdk/window/gfx/rect.h>
#include <sdk/window/gfx/text.h>
#include <sdk/window/gfx/image.h>
#include <sdk/math.h>

#include <rpg/game.h>
#include <rpg/components/player.h>
#include <rpg/components/tilemap.h>

typedef struct local_map_t {
  game_t* game;
  // event_listener_t
  event_listener_t ondraw;
  event_listener_t ondestroy;
  listener_t unload;
  // components
  tilemap_t tilemap;
  player_t player;
} local_map_t;

#include <rpg/maps/hero_home_2f.h>

void localmap_draw(local_map_t* this) {
  tilemap_t* tilemap = &this->tilemap;
  player_t* player = &this->player;
  tilemap_draw(tilemap, tilemap->bg0);
  tilemap_draw(tilemap, tilemap->bg1);
  player_draw(player);
  tilemap_draw(tilemap, tilemap->bg2);
}
void localmap_destroy(local_map_t* this) {
  this->unload(this);
  emitter_off(&this->ondraw);
  emitter_off(&this->ondestroy);
  memory_free(this);
}
void scene_localmap_load(game_t* game) {
  // init
  local_map_t* this = memory_alloc0(sizeof(local_map_t));
  window_t* window = game->window;
  this->game = game;
  // register
  this->ondraw = (event_listener_t) {
    .callback = (listener_t)localmap_draw,
    .context = this
  };
  emitter_on(&window->ondraw, &this->ondraw);
  this->ondestroy = (event_listener_t) {
    .callback = (listener_t)localmap_destroy,
    .context = this
  };
  emitter_on(&window->onclose, &this->ondestroy);
  // components
  tilemap_new(&this->tilemap, this->game);
  player_new(&this->player, &this->tilemap);
  // load
  hero_home_2f_load(this);
}