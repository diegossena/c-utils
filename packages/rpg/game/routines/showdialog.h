#pragma once

#include "../entities/entity.h"
#include "../game.h"
#include <sdk/string/wide.h>
#include <sdk/window/gfx/text.h>
#include <sdk/window/gfx/rect.h>

typedef struct showdialog_t {
  // new
  game_t* game;
  // props
  wstring_t text;
  vector2d_t position;
  // private
  gfx_text_t __text;
  gfx_rect_t __background, __border;
} showdialog_t;
void showdialog_update(showdialog_t* this) {
  // text
  this->__text.text = this->text.__data;
  this->__text.length = this->text.__length;
  this->__text.rect.left_top.x = this->position.x;
  this->__text.rect.left_top.y = this->position.y;
  gfx_text_adjust(&this->__text);
  // background
  this->__background.rect.left_top.x = this->__text.rect.left_top.x - 1;
  this->__background.rect.left_top.y = this->__text.rect.left_top.y - 1;
  this->__background.rect.right_bottom.x = this->__text.rect.right_bottom.x + 1;
  this->__background.rect.right_bottom.y = this->__text.rect.right_bottom.y + 1;
  // border
  this->__border.rect.left_top.x = this->__text.rect.left_top.x - 2;
  this->__border.rect.left_top.y = this->__text.rect.left_top.y - 2;
  this->__border.rect.right_bottom.x = this->__text.rect.right_bottom.x + 2;
  this->__border.rect.right_bottom.y = this->__text.rect.right_bottom.y + 2;
}
void showdialog_new(showdialog_t* this) {
  game_t* game = this->game;
  window_t* window = game->window;
  wstring_new(&this->text);
  this->__text.window = window;
  this->__text.style = &game->dialog_style;
  this->__text.color = &game->white;
  this->__background.window = window;
  this->__background.color = &game->darkblue;
  this->__border.window = window;
  this->__border.color = &game->white;
  this->__border.stroke = &game->stroke_solid;
  this->__border.border_width = 1.f;
}
void showdialog_free(showdialog_t* this) {
  wstring_free(&this->text);
}
void showdialog_draw(showdialog_t* this) {
  gfx_rect_draw(&this->__background);
  gfx_rect_draw(&this->__border);
  gfx_text_draw(&this->__text);
}