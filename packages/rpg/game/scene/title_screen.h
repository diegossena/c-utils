#pragma once

#include <sdk/window/gfx/color.h>
#include <sdk/window/gfx/stroke.h>
#include <sdk/window/gfx/style.h>
#include <sdk/window/gfx/rect.h>
#include <sdk/window/gfx/text.h>
#include <sdk/window/gfx/bitmap.h>

#include "../game.h"
#include "../routines/moveto.h"

typedef struct title_screen_t {
  game_t* game;
  // events
  event_listener_t onupdate;
  event_listener_t onkeydown;
  event_listener_t ondestroy;
  // assets
  bitmap_t terrain_atlas;
  gfx_color_t color_white;
  gfx_color_t color_green;
  gfx_stroke_t stroke_solid;
  gfx_style_t title_style, play_style;
 // elements
  bool container_clicking;
  gfx_bitmap_t background;
  gfx_text_t title, press_space, to_play;
} title_screen_t;

void titlescreen_onupdate(title_screen_t* this) {
  gfx_bitmap_draw(&this->background);
  gfx_text_draw(&this->title);
  gfx_text_draw(&this->press_space);
  gfx_text_draw(&this->to_play);
}
void titlescreen_background_onmove(title_screen_t* this) {
  console_log("titlescreen_background_onmove");
  rect_update_size(&this->background.rect);
  console_log(
    "%f %f %f %f",
    this->background.rect.left_top.x, this->background.rect.left_top.y,
    this->background.rect.right_bottom.x, this->background.rect.right_bottom.y
  );
}
void titlescreen_background_ondestroy(title_screen_t* this) {
  this->background.rect.left_top.y = 0;
  rect_update_size(&this->background.rect);
}
void titlescreen_onkeydown(title_screen_t* this) {
  if (keyboard_pressed(KEY_SPACE)) {
    vector2d_t target = {
      this->background.rect.left_top.x,
      this->background.rect.left_top.y - this->game->window->height
    };
    routine_moveto((moveto_props_t) {
      .game = this->game,
        .duration = 2.f,
        .position = &this->background.rect.left_top,
        .target = target,
        .onupdate = { (listener_t)titlescreen_background_onmove, this },
        .ondestroy = { (listener_t)titlescreen_background_ondestroy, this }
    });
  }
}
void titlescreen_ondestroy(title_screen_t* this) {
  emitter_off(&this->ondestroy);
  emitter_off(&this->onupdate);
  gfx_stroke_free(&this->stroke_solid);
  gfx_color_free(&this->color_white);
  gfx_color_free(&this->color_green);
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
  gfx_stroke_new(&this->stroke_solid, window, (gfx_stroke_props_t) { STROKE_STYLE_SOLID });
  gfx_color_new(&this->color_white, window, COLOR_WHITE);
  gfx_color_new(&this->color_green, window, COLOR_GREEN);
  gfx_style_new(&this->title_style, (gfx_style_props_t) {
    .window = window,
      .family = L"TLOZ Minish Cap/A Link to the Past/Four Sword",
      .size = 32.f,
      .style = FONT_STYLE_NORMAL,
      .weight = FONT_WEIGHT_BOLD
  });
  gfx_style_new(&this->play_style, (gfx_style_props_t) {
    .window = window,
      .family = L"MegaMan 2",
      .size = 26.f,
      .style = FONT_STYLE_NORMAL,
      .weight = FONT_WEIGHT_BOLD
  });

  // elements

  this->title = (gfx_text_t) {
    .window = window,
    gfx_text_cwstr(L"DreamShifters"),
    .position = { 250.f, 10.f },
    .color = &this->color_white,
    .format = &this->title_style
  };
  this->press_space = (gfx_text_t) {
    .window = window,
    gfx_text_cwstr(L"Press Space"),
    .position = { 500.f, 500.f },
    .format = &this->play_style,
    .color = &this->color_white,
  };
  this->to_play = (gfx_text_t) {
    .window = window,
    gfx_text_cwstr(L"to start"),
    .position = { 545.f, 530.f },
    .format = &this->play_style,
    .color = &this->color_white,
  };
  this->background = (gfx_bitmap_t) {
    .window = window,
    .rect = { 0, 0, .width = window->width, .height = window->height },
    .position = { 512, 832 },
    .size = { 95, 95 },
    .image = &this->terrain_atlas
  };
  rect_update_size(&this->background.rect);
}