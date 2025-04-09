#include <game/titlescreen.scene.h>
#include <game/game.h>
#include <game/tilemap.scene.h>

titlescreen_t titlescreen;

export void titlescreen_load() {
  titlescreen.white = gfx_color_new(1, 1, 1, 1);
  // title
  titlescreen.title_style = gfx_textstyle_new(font_family_megaman, 32, FONT_WEIGHT_BOLD, FONT_STYLE_NORMAL);
  // press_space
  titlescreen.press_space_style = gfx_textstyle_new(font_family_megaman, 26, FONT_WEIGHT_BOLD, FONT_STYLE_NORMAL);
  // pallet_town_interiors
  gfx_image_src(&titlescreen.pallet_town_interiors, L"assets/pallet_town_interiors.png");
  // loaded
  titlescreen.loaded = true;
}
export void titlescreen_unload() {
  titlescreen.loaded = false;
  gfx_color_free(titlescreen.white);
  gfx_textstyle_free(titlescreen.title_style);
  gfx_image_free(&titlescreen.pallet_town_interiors);
}
export void titlescreen_onkeydown(key_code_t key) {
  switch (key) {
    case KEY_SPACE:
      transition_load(titlescreen_unload, titlescreen_load);
      break;
    default:
      return;
  }
}
export void titlescreen_render() {
  // title
  const wchar_t title_text [] = L"DreamShifters";
  const f32 title_rect[4] = { 210.f, 10.f, MAX_F32, MAX_F32 };
  gfx_text_draw(
    title_text, sizeof(title_text) / sizeof(wchar_t),
    title_rect, titlescreen.white, titlescreen.title_style
  );
  // press_space
  const wchar_t press_space_text [] = L"PRESS SPACE";
  const f32 press_space_rect[4] = { 500.f, 500.f, MAX_F32, MAX_F32 };
  gfx_text_draw(
    press_space_text, sizeof(press_space_text) / sizeof(wchar_t),
    press_space_rect, titlescreen.white, titlescreen.press_space_style
  );
  // to_play
  const wchar_t to_play_text [] = L"TO PLAY";
  const f32 to_play_rect[4] = { 545.f, 530.f, MAX_F32, MAX_F32 };
  gfx_text_draw(
    to_play_text, sizeof(to_play_text) / sizeof(wchar_t),
    to_play_rect, titlescreen.white, titlescreen.press_space_style
  );
  // pallet_town_interiors
  const f32 pallet_town_interiors_rect[4] = { 0, 0, 76, 76 };
  const f32 pallet_town_interiors_src_rect[4] = { 5, 5, 76, 76 };
  gfx_image_draw(
    &titlescreen.pallet_town_interiors, pallet_town_interiors_rect,
    pallet_town_interiors_src_rect, 1.f
  );
}