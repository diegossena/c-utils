#pragma once

#include "../entities/entity.h"
#include "../game.h"

typedef struct showdialog_t {
  // init
  game_t* game;
  /**
   * modify position and content of display
   */
  gfx_text_t display;
  // private
  gfx_rect_t __background, __border;
} showdialog_t;
void showdialog_update(showdialog_t* this) {
  // text
  gfx_text_update(&this->display);
  // background
  this->__background.rect.left_top.x = this->display.rect.left_top.x - 1;
  this->__background.rect.left_top.y = this->display.rect.left_top.y - 1;
  this->__background.rect.right_bottom.x = this->display.rect.right_bottom.x + 1;
  this->__background.rect.right_bottom.y = this->display.rect.right_bottom.y + 1;
  // border
  this->__border.rect.left_top.x = this->display.rect.left_top.x - 2;
  this->__border.rect.left_top.y = this->display.rect.left_top.y - 2;
  this->__border.rect.right_bottom.x = this->display.rect.right_bottom.x + 2;
  this->__border.rect.right_bottom.y = this->display.rect.right_bottom.y + 2;
}
void showdialog_new(showdialog_t* this) {
  assert(this->game);
  game_t* game = this->game;
  window_t* window = game->window;
  // display
  this->display.window = window;
  this->display.style = &game->dialog_style;
  this->display.color = &game->white;
  gfx_text_new(&this->display);
  // background
  this->__background = (gfx_rect_t) {
    .window = window,
    .color = &game->darkblue,
  };
  // border
  this->__border = (gfx_rect_t) {
    .window = window,
    .color = &game->white,
    .stroke = &game->stroke_solid,
    .border_width = 1.f
  };
}
void showdialog_free(showdialog_t* this) {
  gfx_text_free(&this->display);
}
void showdialog_draw(showdialog_t* this) {
  gfx_rect_draw(&this->__background);
  gfx_rect_draw(&this->__border);
  gfx_text_draw(&this->display);
}