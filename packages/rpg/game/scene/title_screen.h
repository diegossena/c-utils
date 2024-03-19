#pragma once

#include "../game.h"

typedef struct title_screen_t {
  game_t* game;
  // events
  game_update_event_t onupdate;
  game_mouse_event_t onmousemove;
  game_mouse_event_t onmousedown;
  game_mouse_event_t onmouseup;
  game_event_t ondestroy;
  // styles
  gfx_color_t* color_black;
  gfx_color_t* color_green;
  gfx_stroke_t* stroke_solid;
  gfx_text_style_t* text_style;
  // elements
  gfx_rect_t container;
  bool container_clicking;
  gfx_text_t title;
} title_screen_t;

void titlescreen_onmousemove(title_screen_t* this, vector2d_t cursor) {
  window_t* window = this->game->window;
  if (rect_pointin(&this->container.rect, cursor)) {
    this->title.color = this->color_green;
  } else {
    this->title.color = this->color_black;
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
    console_log("onclick");
  }
  this->container_clicking = false;
}
void titlescreen_onupdate(title_screen_t* this) {
  window_t* window = this->game->window;
  gfx_draw_rect(window, &this->container);
  gfx_draw_text(window, &this->title);
}
void titlescreen_ondestroy(title_screen_t* this) {
  window_t* window = this->game->window;
  queue_remove(&this->ondestroy.queue);
  queue_remove(&this->onupdate.queue);
  queue_remove(&this->onmousemove.queue);
  queue_remove(&this->onmousedown.queue);
  queue_remove(&this->onmouseup.queue);
  gfx_stroke_free(this->stroke_solid);
  gfx_color_free(this->color_black);
  gfx_color_free(this->color_green);
  gfx_text_style_free(this->text_style);
  memory_free(this);
}
void title_screen_load(game_t* game) {
  title_screen_t* this = memory_alloc0(sizeof(title_screen_t));
  window_t* window = game->window;
  this->game = game;
  // register events
  this->ondestroy = (game_event_t) {
    .callback = (game_onevent_cb)titlescreen_ondestroy,
    .context = this
  };
  queue_push(&game->ondestroy, &this->ondestroy.queue);
  this->onupdate = (game_update_event_t) {
    .callback = (game_onupdate_cb)titlescreen_onupdate,
    .context = this
  };
  queue_push(&game->onupdate, &this->onupdate.queue);
  this->onmousemove = (game_mouse_event_t) {
    .callback = (game_onmouse_cb)titlescreen_onmousemove,
    .context = this
  };
  queue_push(&game->onmousemove, &this->onmousemove.queue);
  this->onmousedown = (game_mouse_event_t) {
    .callback = (game_onmouse_cb)titlescreen_onmousedown,
    .context = this
  };
  queue_push(&game->onmousedown, &this->onmousedown.queue);
  this->onmouseup = (game_mouse_event_t) {
    .callback = (game_onmouse_cb)titlescreen_onmouseup,
    .context = this
  };
  queue_push(&game->onmouseup, &this->onmouseup.queue);
  // styles
  this->stroke_solid = gfx_stroke_new(window, (stroke_t) { BORDER_STYLE_SOLID });
  this->color_black = gfx_color_new(window, COLOR_BLACK);
  this->color_green = gfx_color_new(window, COLOR_GREEN);
  this->text_style = gfx_text_style_new(window, (text_style_props_t) {
    .size = 40, .family = L"Arial", .weight = FONT_WEIGHT_BOLD
  });
  // elements
  this->container = (gfx_rect_t) {
    .rect = { 230.f },
    .color = this->color_black,
    .stroke = this->stroke_solid,
    .border_width = 1.f
  };
  this->title = (gfx_text_t) {
    gfx_textnode_cwstr(L"DreamShifters"),
    .style = this->text_style,
    .position = this->container.rect.left_top,
    .color = this->color_black
  };
  rect_set_width(&this->container.rect, 280.f);
  rect_set_height(&this->container.rect, 42.f);
}