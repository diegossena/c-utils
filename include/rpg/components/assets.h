#pragma once

#include <rpg/game.h>

void pallet_town_interiors_load(game_t* this) {

}
void character_png_load(game_t* this) {

}
void dialog_style_load(game_t* this) {
  gfx_text_style_new((gfx_text_style_t*)&this->dialog_style, (text_style_props_t) {
    .window = this->window,
      .size = 28.f,
      .family = this->font_megaman_family,
      .weight = FONT_WEIGHT_NORMAL
  });
}
void stroke_solid_load(game_t* this) {
  gfx_stroke_new(&this->stroke_solid, (gfx_stroke_props_t) {
    this->window, STROKE_STYLE_SOLID
  });
}
void white_load(game_t* this) {
  gfx_color_new(&this->white, this->window, COLOR_WHITE);
}
void green_load(game_t* this) {

}
void darkblue_load(game_t* this) {

}