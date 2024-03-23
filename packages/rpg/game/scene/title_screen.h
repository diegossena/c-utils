#pragma once

#include <sdk/window/gfx/rect.h>
#include <sdk/window/gfx/text.h>

#include "../game.h"
#include "../routines/scene_transition.h"
#include "../routines/showdialog.h"
#include "./local_map.h"

typedef struct title_screen_t {
  game_t* game;
  // events
  event_listener_t ondraw;
  event_listener_t onkeydown;
  event_listener_t destroy;
  // assets
  gfx_style_t title_style, play_style;
  // elements
  gfx_image_t background;
  gfx_text_t title, press_space, to_play;
} title_screen_t;

void titlescreen_load(game_t*);
void titlescreen_destroy(title_screen_t*);

void titlescreen_ondraw(title_screen_t* this) {
  gfx_image_draw(&this->background);
  gfx_text_draw(&this->title);
  gfx_text_draw(&this->press_space);
  gfx_text_draw(&this->to_play);
}
void titlescreen_onkeydown(title_screen_t* this) {
  if (keyboard_pressed(KEY_SPACE)) {
    scene_transition((scene_transition_props_t) {
      .game = this->game,
        .scene_destroy = { (listener_t)titlescreen_destroy, this },
        .scene_load = { (listener_t)localmap_load, this->game },
    });
  }
}
void titlescreen_destroy(title_screen_t* this) {
  emitter_off(&this->ondraw);
  emitter_off(&this->onkeydown);
  emitter_off(&this->destroy);
  gfx_style_free(&this->title_style);
  gfx_style_free(&this->play_style);
  memory_free(this);
}
void titlescreen_load(game_t* game) {
  // init
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

  // title
  this->title = (gfx_text_t) {
    .window = window,
    gfx_text_cwstr(L"DreamShifters"),
    .rect = { 250.f, 10.f },
    .color = &this->game->white,
    .style = &this->title_style
  };
  gfx_text_adjust(&this->title);
  // press_space
  this->press_space = (gfx_text_t) {
    .window = window,
    gfx_text_cwstr(L"Press Space"),
    .rect = { 500.f, 500.f },
    .style = &this->play_style,
    .color = &this->game->white,
  };
  gfx_text_adjust(&this->press_space);
  // to_play
  this->to_play = (gfx_text_t) {
    .window = window,
    gfx_text_cwstr(L"to start"),
    .rect = { 545.f, 530.f },
    .style = &this->play_style,
    .color = &this->game->white,
  };
  gfx_text_adjust(&this->to_play);
  // background
  this->background = (gfx_image_t) {
    .window = window,
    .rect = { 0, 0, .width = window->width, .height = window->height },
    .position = { 512, 832 },
    .size = { 95, 95 },
    .src = &game->terrain_atlas
  };
  rect_update_size(&this->background.rect);
}