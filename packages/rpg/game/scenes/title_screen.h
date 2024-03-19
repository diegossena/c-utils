#pragma once

#include "../game.h"
#include "./scene.h"

typedef struct title_screen_t {
  scene_t scene;
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

void titlescreen_onmousemove(game_t* this, vector2d_t cursor) {
  window_t* window = this->window;
  title_screen_t* scene = (title_screen_t*)this->scene;
  if (rect_pointin(&scene->container.rect, cursor)) {
    scene->title.color = scene->color_green;
  } else {
    scene->title.color = scene->color_black;
  }
}
void titlescreen_onmousedown(game_t* this, vector2d_t cursor) {
  window_t* window = this->window;
  title_screen_t* scene = (title_screen_t*)this->scene;
  if (rect_pointin(&scene->container.rect, cursor)) {
    scene->container_clicking = true;
  }
}
void titlescreen_onmouseup(game_t* this, vector2d_t cursor) {
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
void titlescreen_destroy(game_t* this) {
  window_t* window = this->window;
  title_screen_t* scene = (title_screen_t*)this->scene;
  gfx_stroke_free(scene->stroke_solid);
  gfx_color_free(scene->color_black);
  gfx_color_free(scene->color_green);
  gfx_text_style_free(scene->text_style);
  memory_free(scene);
}
void title_screen_load(game_t* game) {
  title_screen_t* this = memory_alloc(sizeof(title_screen_t));
  window_t* window = game->window;
  // events
  this->scene = (scene_t) {
    .onupdate = titlescreen_onupdate,
    .onmousemove = titlescreen_onmousemove,
    .onmouseup = titlescreen_onmouseup,
    .onmousedown = titlescreen_onmousedown,
    .destroy = titlescreen_destroy
  };
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
  game->scene = (scene_t*)this;
}