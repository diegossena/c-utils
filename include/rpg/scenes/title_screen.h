#pragma once

#include <sdk/window/gfx/rect.h>
#include <sdk/window/gfx/text.h>

#include <rpg/game.h>
#include <rpg/routines/scene_transition.h>
#include <rpg/components/showdialog.h>
#include <rpg/scenes/local_map.h>

typedef struct title_screen_t {
  game_t* game;
  // events
  event_listener_t ondraw;
  event_listener_t onkeydown;
  event_listener_t destroy;
  // assets
  gfx_text_style_t title_style, play_style;
  // elements
  gfx_image_t background;
  gfx_text_t title, press_space, to_play;
} title_screen_t;

void titlescreen_destroy(title_screen_t* this) {
  emitter_off(&this->ondraw);
  emitter_off(&this->onkeydown);
  emitter_off(&this->destroy);
  gfx_text_free(&this->title);
  gfx_text_free(&this->press_space);
  gfx_text_free(&this->to_play);
  gfx_text_style_free(&this->title_style);
  gfx_text_style_free(&this->play_style);
  memory_free(this);
}

void titlescreen_ondraw(title_screen_t* this) {
  gfx_image_draw(&this->background);
  gfx_text_draw(&this->title);
  gfx_text_draw(&this->press_space);
  gfx_text_draw(&this->to_play);
}
void titlescreen_onkeydown(title_screen_t* this) {
  window_t* window = this->game->window;
  if (window->has_focus) {
    if (keyboard_pressed(KEY_SPACE)) {
      scene_transition((scene_transition_props_t) {
        .game = this->game,
          .scene_destroy = { (listener_t)titlescreen_destroy, this },
          .scene_load = { (listener_t)scene_localmap_load, this->game },
      });
    }
  }
}
void scene_titlescreen_load(game_t* game) {
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
  gfx_text_style_new(&this->title_style, (text_style_props_t) {
    .window = window,
      .family = game->font_zelda_family,
      .size = 32.f,
      .style = FONT_STYLE_NORMAL,
      .weight = FONT_WEIGHT_BOLD
  });
  gfx_text_style_new(&this->play_style, (text_style_props_t) {
    .window = window,
      .family = game->font_megaman_family,
      .size = 26.f,
      .style = FONT_STYLE_NORMAL,
      .weight = FONT_WEIGHT_BOLD
  });
  // background
  this->background = (gfx_image_t) {
    .window = window,
    .src_position = { 609, 929 },
    .src_width = 61,
    .src_height = 57,
    .src = &game->terrain_atlas
  };
  rect_set_width(&this->background.rect, window->width);
  rect_set_height(&this->background.rect, window->height);
  // title
  this->title = (gfx_text_t) {
    .window = window,
    .rect = { 250.f, 10.f },
    .color = &this->game->white,
    .style = &this->title_style
  };
  gfx_text_new(&this->title);
  wstring_append_cwstr(&this->title.text, L"DreamShifters");
  gfx_text_update(&this->title);
  // press_space
  this->press_space = (gfx_text_t) {
    .window = window,
    .rect = { 500.f, 500.f },
    .style = &this->play_style,
    .color = &this->game->white,
  };
  gfx_text_new(&this->press_space);
  wstring_append_cwstr(&this->press_space.text, L"Press Space");
  gfx_text_update(&this->press_space);
  // to_play
  this->to_play = (gfx_text_t) {
    .window = window,
    .rect = { 545.f, 530.f },
    .style = &this->play_style,
    .color = &this->game->white,
  };
  gfx_text_new(&this->to_play);
  wstring_append_cwstr(&this->to_play.text, L"to start");
  gfx_text_update(&this->to_play);
}