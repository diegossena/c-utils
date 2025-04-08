#include <game/titlescreen.scene.h>
#include <game/game.h>

titlescreen_t titlescreen = {
  .title = {
    .text = L"DreamShifters",
    .rect = { 210.f, 10.f, MAX_F32, MAX_F32 }
  },
  .press_space = {
    .text = L"PRESS SPACE",
    .rect = { 500.f, 500.f, MAX_F32, MAX_F32 }
  },
  .to_play = {
    .text = L"TO PLAY",
    .rect = { 545.f, 530.f,MAX_F32, MAX_F32 }
  },
  .pallet_town_interiors = {
    .rect = { 0, 0, 76, 76},
    .src_rect = { 5, 5, 16, 16 },
    .extend_mode = BITMAP_EXTEND_COVER,
  }
};

export void titlescreen_load() {
  gfx_color_t* white = gfx_color_new(1, 1, 1, 1);
  // title
  titlescreen.title.color = white;
  titlescreen.title.style = gfx_textstyle_new(font_family_megaman, 32, FONT_WEIGHT_BOLD, FONT_STYLE_NORMAL);
  // press_space
  titlescreen.press_space.color = white;
  titlescreen.press_space.style = gfx_textstyle_new(font_family_megaman, 26, FONT_WEIGHT_BOLD, FONT_STYLE_NORMAL);
  // to_play
  titlescreen.to_play.color = white;
  titlescreen.to_play.style = titlescreen.press_space.style;
  // pallet_town_interiors
  gfx_image_src(&titlescreen.pallet_town_interiors, L"assets/pallet_town_interiors.png");
  // loaded
  titlescreen.loaded = true;
}
export void titlescreen_unload() {
  titlescreen.loaded = false;
  gfx_color_free(titlescreen.title.color);
  gfx_textstyle_free(titlescreen.title.style);
  gfx_textstyle_free(titlescreen.press_space.style);
  gfx_image_free(&titlescreen.pallet_town_interiors);
}
export void titlescreen_onkeydown(key_code_t key) {
  switch (key) {
    case KEY_SPACE:
      break;
    default:
      return;
  }
}
export void titlescreen_render() {
  gfx_text_draw(&titlescreen.title);
  gfx_text_draw(&titlescreen.press_space);
  gfx_text_draw(&titlescreen.to_play);
  gfx_image_draw(&titlescreen.pallet_town_interiors);
}