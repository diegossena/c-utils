#pragma once

#include <sdk/window/gfx/style.h>
#include <sdk/window/gfx/rect.h>
#include <sdk/window/gfx/text.h>
#include <sdk/window/gfx/bitmap.h>

#include "../game.h"
#include "../routines/scene_transition.h"
#include "../routines/showdialog.h"

typedef struct title_screen_t {
  game_t* game;
  // events
  event_listener_t ondraw;
  event_listener_t onkeydown;
  event_listener_t destroy;
  // assets
  bitmap_t terrain_atlas;
  gfx_style_t title_style, play_style;
  // elements
  gfx_bitmap_t background;
  gfx_text_t title, press_space, to_play;
  showdialog_t hp_display;
} title_screen_t;

void titlescreen_load(game_t*);
void titlescreen_destroy(title_screen_t*);

void titlescreen_ondraw(title_screen_t* this) {
  game_t* game = this->game;
  window_t* window = game->window;
  gfx_bitmap_draw(&this->background);
  gfx_text_draw(&this->title);
  gfx_text_draw(&this->press_space);
  gfx_text_draw(&this->to_play);
  showdialog_draw(&this->hp_display);
}
void titlescreen_onkeydown(title_screen_t* this) {
  if (keyboard_pressed(KEY_SPACE)) {
    scene_transition((scene_transition_props_t) {
      .game = this->game,
        .scene_destroy = { (listener_t)titlescreen_destroy, this },
        .scene_load = { (listener_t)titlescreen_load, this->game },
    });
  }
}
void titlescreen_destroy(title_screen_t* this) {
  emitter_off(&this->ondraw);
  emitter_off(&this->onkeydown);
  emitter_off(&this->destroy);
  showdialog_free(&this->hp_display);
  gfx_style_free(&this->title_style);
  gfx_style_free(&this->play_style);
  gfx_bitmap_free(&this->terrain_atlas);
  memory_free(this);
}
void titlescreen_load(game_t* game) {
  title_screen_t* this = memory_alloc(sizeof(title_screen_t));
  window_t* window = game->window;
  this->game = game;
  // register
  this->ondraw = (event_listener_t) {
    .callback = (listener_t)titlescreen_ondraw,
    .context = this
  };
  emitter_on(&window->ondraw, &this->ondraw);
  this->destroy = (event_listener_t) {
    .callback = (listener_t)titlescreen_destroy,
    .context = this
  };
  emitter_on(&window->onclose, &this->destroy);
  this->onkeydown = (event_listener_t) {
    .callback = (listener_t)titlescreen_onkeydown,
    .context = this
  };
  emitter_on(&window->onkeydown, &this->onkeydown);
  // assets
  gfx_bitmap_new(&this->terrain_atlas, L"./assets/sprites/terrain_atlas.png", window);
  gfx_style_new(&this->title_style, (gfx_style_props_t) {
    .window = window,
      .family = game->font_zelda_family,
      .size = 32.f,
      .style = FONT_STYLE_NORMAL,
      .weight = FONT_WEIGHT_BOLD
  });
  gfx_style_new(&this->play_style, (gfx_style_props_t) {
    .window = window,
      .family = game->font_megaman_family,
      .size = 26.f,
      .style = FONT_STYLE_NORMAL,
      .weight = FONT_WEIGHT_BOLD
  });
  // elements
  this->title = (gfx_text_t) {
    .window = window,
    gfx_text_cwstr(L"DreamShifters"),
    .rect = { 250.f, 10.f },
    .color = &this->game->white,
    .style = &this->title_style
  };
  gfx_text_auto_size(&this->title);
  this->press_space = (gfx_text_t) {
    .window = window,
    gfx_text_cwstr(L"Press Space"),
    .rect = { 500.f, 500.f },
    .style = &this->play_style,
    .color = &this->game->white,
  };
  gfx_text_auto_size(&this->press_space);
  this->to_play = (gfx_text_t) {
    .window = window,
    gfx_text_cwstr(L"to start"),
    .rect = { 545.f, 530.f },
    .style = &this->play_style,
    .color = &this->game->white,
  };
  gfx_text_auto_size(&this->to_play);
  this->background = (gfx_bitmap_t) {
    .window = window,
    .rect = { 0, 0, .width = window->width, .height = window->height },
    .position = { 512, 832 },
    .size = { 95, 95 },
    .image = &this->terrain_atlas
  };
  rect_update_size(&this->background.rect);
  showdialog_new(&this->hp_display, game);
  wstring_append_cwstr(&this->hp_display.text, L"HP: 9/10");
  this->hp_display.position.x = 10.f;
  this->hp_display.position.y = 10.f;
  showdialog_update(&this->hp_display);
}