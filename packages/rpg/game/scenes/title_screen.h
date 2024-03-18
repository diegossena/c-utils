#pragma once

#include "../game.h"
#include "./scene.h"

typedef struct title_screen_t {
  scene_t scene;
  gfx_rect_t container;
  bool container_clicking;
  gfx_text_style_t text_style;
  gfx_textnode_t title;
} title_screen_t;

void titlescreen_onmousemove(game_t* this, vector2d_t cursor) {
  window_t* window = this->window;
  title_screen_t* scene = (title_screen_t*)this->scene;
  if (rect_pointin(&scene->container.rect, cursor)) {
    scene->text_style.color = COLOR_GREEN;
  } else {
    scene->text_style.color = COLOR_BLACK;
  }
}
void titlescreen_onmousedown(game_t* this, vector2d_t cursor) {
  window_t* window = this->window;
  title_screen_t* scene = (title_screen_t*)this->scene;
  console_log("titlescreen_onmousedown");
  if (rect_pointin(&scene->container.rect, cursor)) {
    scene->container_clicking = true;
  }
}
void titlescreen_onmouseup(game_t* this, vector2d_t cursor) {
  console_log("titlescreen_onmouseup");
  window_t* window = this->window;
  title_screen_t* scene = (title_screen_t*)this->scene;
  if (scene->container_clicking
    && rect_pointin(&scene->container.rect, cursor)) {
    console_log("onclick");
  }
  scene->container_clicking = false;
}
void titlescreen_onupdate(game_t* this) {
  window_t* window = this->window;
  title_screen_t* scene = (title_screen_t*)this->scene;
  gfx_draw_rect(window, &scene->container);
  gfx_draw_text(window, &scene->title);
}

void title_screen_load(game_t* game) {
  title_screen_t* this = memory_alloc(sizeof(title_screen_t));
  this->scene = (scene_t) {
    .onupdate = titlescreen_onupdate,
    .onmousemove = titlescreen_onmousemove,
    .onmouseup = titlescreen_onmouseup,
    .onmousedown = titlescreen_onmousedown
  };
  this->text_style = (gfx_text_style_t) {
    .size = 40,
    .family = L"Arial",
    .color = COLOR_BLACK,
    .weight = FONT_WEIGHT_BOLD
  };
  this->container = (gfx_rect_t) {
    .rect = { 230.f },
    .color = COLOR_BLACK,
    .border = { 1.f, BORDER_STYLE_SOLID, COLOR_BLACK }
  };
  this->title = (gfx_textnode_t) {
    gfx_textnode_cwstr(L"DreamShifters"),
    .style = &this->text_style,
    .parent = &this->container.rect
  };
  rect_set_width(&this->container.rect, 290.f);
  rect_set_height(&this->container.rect, 42.f);
  game->scene = (scene_t*)this;
}