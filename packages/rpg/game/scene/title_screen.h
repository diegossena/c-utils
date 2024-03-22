#pragma once

#include <sdk/window/gfx/style.h>
#include <sdk/window/gfx/rect.h>
#include <sdk/window/gfx/text.h>
#include <sdk/window/gfx/bitmap.h>

#include "../game.h"
#include "../routines/scene_transition.h"

typedef struct title_screen_t {
  game_t* game;
  // events
  event_listener_t onupdate;
  event_listener_t onkeydown;
  event_listener_t ondestroy;
  // assets
  bitmap_t terrain_atlas;
  gfx_style_t title_style, play_style;
  // elements
  gfx_bitmap_t background;
  gfx_text_t title, press_space, to_play;
} title_screen_t;

void titlescreen_load(game_t*);
void titlescreen_ondestroy(title_screen_t*);

void titlescreen_onupdate(title_screen_t* this) {
  game_t* game = this->game;
  window_t* window = game->window;
  gfx_bitmap_draw(&this->background);
  gfx_text_draw(&this->title);
  gfx_text_draw(&this->press_space);
  gfx_text_draw(&this->to_play);

  const wchar_t display [] = L"HP: 9/10";
  u64 display_length = sizeof(display) / sizeof(wchar_t) - 1;
  u32 lines_count = 1;
  f32 font_size = 18.f;

  gfx_text_t text = {
    .window = window,
    .text = display,
    .length = display_length,
    .rect = { 10.f, 10.f },
    .format = &game->dialog_style,
    .color = &game->white
  };
  gfx_text_auto_size(&text);
  gfx_rect_t background = {
    .window = window,
    .rect = {
      text.rect.left_top.x - 1, text.rect.left_top.y - 1,
      text.rect.right_bottom.x + 1, text.rect.right_bottom.y + 1
    },
    .color = &game->darkblue
  };
  gfx_rect_t border = {
    .window = window,
    .rect = {
      text.rect.left_top.x - 2, text.rect.left_top.y - 2,
      text.rect.right_bottom.x + 2, text.rect.right_bottom.y + 2
    },
    .color = &game->white,
    .stroke = &game->stroke_solid,
    .border_width = 2.f
  };

  gfx_rect_draw(&background);
  gfx_rect_draw(&border);
  gfx_text_draw(&text);
}
void titlescreen_onkeydown(title_screen_t* this) {
  if (keyboard_pressed(KEY_SPACE)) {
    scene_transition((scene_transition_props_t) {
      .game = this->game,
        .scene_destroy = { (listener_t)titlescreen_ondestroy, this },
        .scene_load = { (listener_t)titlescreen_load, this->game },
    });
  }
}
void titlescreen_ondestroy(title_screen_t* this) {
  emitter_off(&this->ondestroy);
  emitter_off(&this->onupdate);
  emitter_off(&this->onkeydown);
  gfx_style_free(&this->title_style);
  gfx_style_free(&this->play_style);
  gfx_bitmap_free(&this->terrain_atlas);
  memory_free(this);
}
void titlescreen_load(game_t* game) {
  title_screen_t* this = memory_alloc(sizeof(title_screen_t));
  window_t* window = game->window;
  this->game = game;
  // events
  this->ondestroy = (event_listener_t) {
    .callback = (listener_t)titlescreen_ondestroy,
    .context = this
  };
  emitter_on(&game->ondestroy, &this->ondestroy);
  this->onupdate = (event_listener_t) {
    .callback = (listener_t)titlescreen_onupdate,
    .context = this
  };
  emitter_on(&game->onupdate, &this->onupdate);
  this->onkeydown = (event_listener_t) {
    .callback = (listener_t)titlescreen_onkeydown,
    .context = this
  };
  emitter_on(&game->onkeydown, &this->onkeydown);

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
    .format = &this->title_style
  };
  gfx_text_auto_size(&this->title);
  this->press_space = (gfx_text_t) {
    .window = window,
    gfx_text_cwstr(L"Press Space"),
    .rect = { 500.f, 500.f },
    .format = &this->play_style,
    .color = &this->game->white,
  };
  gfx_text_auto_size(&this->press_space);
  this->to_play = (gfx_text_t) {
    .window = window,
    gfx_text_cwstr(L"to start"),
    .rect = { 545.f, 530.f },
    .format = &this->play_style,
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
}