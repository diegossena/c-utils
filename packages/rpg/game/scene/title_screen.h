#pragma once

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
  gfx_font_t font;
  // elements
  gfx_rect_t container;
  bool container_clicking;
  gfx_text_t title;
  gfx_text_t press_space;
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
  window_t* window = this->game->window;
  gfx_draw_rect(window, &this->container);
  gfx_draw_text(window, &this->title);
  gfx_draw_text(window, &this->press_space);
}
void titlescreen_ondestroy(title_screen_t* this) {
  window_t* window = this->game->window;
  emitter_off(&this->ondestroy);
  queue_remove(&this->onupdate.queue);
  queue_remove(&this->onmousemove.queue);
  queue_remove(&this->onmousedown.queue);
  queue_remove(&this->onmouseup.queue);
  gfx_stroke_free(&this->stroke_solid);
  gfx_color_free(&this->color_black);
  gfx_color_free(&this->color_green);
  gfx_font_free(&this->font);
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
  gfx_stroke_new(&this->stroke_solid, window, (stroke_t) { BORDER_STYLE_SOLID });
  gfx_color_new(&this->color_black, window, COLOR_BLACK);
  gfx_color_new(&this->color_green, window, COLOR_GREEN);
  gfx_font_load(
    &this->font, window, L"TLOZ Minish Cap/A Link to the Past/Four Sword",
    L"C:\\Users\\Diego\\Documents\\Projects\\c-utils\\assets\\fonts\\zelda-font.ttf"
  );
  // gfx_font_load(
  //   &this->font, window, L"MegaMan 2",
  //   L"C:\\Users\\Diego\\Documents\\Projects\\c-utils\\assets\\fonts\\megaman_2.ttf"
  // );
  gfx_color_new(&this->color_green, window, COLOR_GREEN);
  // gfx_font_new(&this->font, window, L"TLOZ Minish Cap/A Link to the Past/Four Sword");
  // elements
  this->container = (gfx_rect_t) {
    .rect = { 230.f },
    .color = &this->color_black,
    .stroke = &this->stroke_solid,
    .border_width = 1.f
  };
  rect_set_width(&this->container.rect, 280.f);
  rect_set_height(&this->container.rect, 42.f);
  gfx_text_new(&this->title, window, (text_t) {
    gfx_textnode_cwstr(L"DreamShifters"),
      .font = &this->font,
      .position = this->container.rect.left_top,
      .color = &this->color_black
  });
  gfx_text_set_size(&this->title, 32.f);
  gfx_text_set_weight(&this->title, FONT_WEIGHT_BOLD);
  gfx_text_new(&this->press_space, window, (text_t) {
    gfx_textnode_cwstr(L"Press Space"),
      .font = &this->font,
      .position = { 100.f, 100.f },
      .color = &this->color_black
  });
}