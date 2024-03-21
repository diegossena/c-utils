#pragma once

#include <sdk/window/gfx/color.h>
#include <sdk/window/gfx/stroke.h>
#include <sdk/window/gfx/style.h>
#include <sdk/window/gfx/rect.h>
#include <sdk/window/gfx/text.h>

#include "../game.h"
#include "../routines/moveto.h"

typedef struct title_screen_t {
  game_t* game;
  // events
  update_listener_t onupdate;
  mouse_listener_t onmousemove;
  mouse_listener_t onmousedown;
  mouse_listener_t onmouseup;
  event_listener_t ondestroy;
  // styles
  gfx_color_t color_black;
  gfx_color_t color_green;
  gfx_stroke_t stroke_solid;
  gfx_style_t title_format, press_space_format;
 // elements
  gfx_rect_t container;
  bool container_clicking;
  gfx_text_t title, press_space;
} title_screen_t;

void titlescreen_onmousemove(title_screen_t* this, vector2d_t cursor) {
  window_t* window = this->game->window;
  if (rect_pointin(&this->container.rect, cursor)) {
    this->title.color = &this->color_green;
  } else {
    this->title.color = &this->color_black;
  }
}
void titlescreen_onmousedown(title_screen_t* this, vector2d_t cursor) {
  window_t* window = this->game->window;
  if (rect_pointin(&this->container.rect, cursor)) {
    this->container_clicking = true;
  }
}
void titlescreen_onmouseup(title_screen_t* this, vector2d_t cursor) {
  window_t* window = this->game->window;
  if (this->container_clicking
    && rect_pointin(&this->container.rect, cursor)) {
  }
  this->container_clicking = false;
}
void titlescreen_onupdate(title_screen_t* this) {
  gfx_rect_draw(&this->container);
  gfx_text_draw(&this->title);
  gfx_text_draw(&this->press_space);
}
void titlescreen_ondestroy(title_screen_t* this) {
  emitter_off(&this->ondestroy);
  queue_remove(&this->onupdate.queue);
  queue_remove(&this->onmousemove.queue);
  queue_remove(&this->onmousedown.queue);
  queue_remove(&this->onmouseup.queue);
  gfx_stroke_free(&this->stroke_solid);
  gfx_color_free(&this->color_black);
  gfx_color_free(&this->color_green);
  gfx_style_free(&this->title_format);
  gfx_style_free(&this->press_space_format);
  memory_free(this);
}
void titlescreen_load(game_t* game) {
  title_screen_t* this = memory_alloc(sizeof(title_screen_t));
  window_t* window = game->window;
  this->game = game;
  // register events
  this->ondestroy = (event_listener_t) {
    .callback = (listener_t)titlescreen_ondestroy,
    .context = this
  };
  emitter_on(&game->ondestroy, &this->ondestroy);
  this->onupdate = (update_listener_t) {
    .callback = (onupdate_callback_t)titlescreen_onupdate,
    .context = this
  };
  queue_push(&game->onupdate, &this->onupdate.queue);
  this->onmousemove = (mouse_listener_t) {
    .callback = (onmouse_callback_t)titlescreen_onmousemove,
    .context = this
  };
  queue_push(&game->onmousemove, &this->onmousemove.queue);
  this->onmousedown = (mouse_listener_t) {
    .callback = (onmouse_callback_t)titlescreen_onmousedown,
    .context = this
  };
  queue_push(&game->onmousedown, &this->onmousedown.queue);
  this->onmouseup = (mouse_listener_t) {
    .callback = (onmouse_callback_t)titlescreen_onmouseup,
    .context = this
  };
  queue_push(&game->onmouseup, &this->onmouseup.queue);
  // styles
  gfx_stroke_new(&this->stroke_solid, window, (gfx_stroke_props_t) { BORDER_STYLE_SOLID });
  gfx_color_new(&this->color_black, window, COLOR_BLACK);
  gfx_color_new(&this->color_green, window, COLOR_GREEN);
  gfx_color_new(&this->color_green, window, COLOR_GREEN);
  // elements
  this->container = (gfx_rect_t) {
    .window = window,
    .rect = { 230.f },
    .color = &this->color_black,
    .stroke = &this->stroke_solid,
    .border_width = 1.f
  };
  rect_set_width(&this->container.rect, 280.f);
  rect_set_height(&this->container.rect, 42.f);

  gfx_style_new(&this->title_format, (gfx_style_props_t) {
    .window = window,
      .family = L"TLOZ Minish Cap/A Link to the Past/Four Sword",
      .size = 32.f,
      .style = FONT_STYLE_NORMAL,
      .weight = FONT_WEIGHT_BOLD
  });
  this->title = (gfx_text_t) {
    .window = window,
    gfx_text_cwstr(L"DreamShifters"),
    .position = this->container.rect.left_top,
    .color = &this->color_black,
    .format = &this->title_format
  };

  gfx_style_new(&this->press_space_format, (gfx_style_props_t) {
    .window = window,
      .family = L"MegaMan 2",
      .size = 26.f,
      .style = FONT_STYLE_NORMAL,
      .weight = FONT_WEIGHT_BOLD
  });
  this->press_space = (gfx_text_t) {
    .window = window,
    gfx_text_cwstr(L"Press Space"),
    .position = { 500.f, 500.f },
    .format = &this->press_space_format,
    .color = &this->color_black,
  };
}