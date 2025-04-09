#include <game/titlescreen.scene.h>
#include <game/tilemap.scene.h>
#include <game/hero_home_2f.map.h>

titlescreen_t titlescreen;

export void titlescreen_load() {
  // titlescreen.white = gfx_color_new(1, 1, 1, 1);
  // // title
  // titlescreen.title_style = gfx_textstyle_new(font_family_megaman, 32, FONT_WEIGHT_BOLD, FONT_STYLE_NORMAL);
  // // press_space
  // titlescreen.press_space_style = gfx_textstyle_new(font_family_megaman, 26, FONT_WEIGHT_BOLD, FONT_STYLE_NORMAL);
  // loaded
  titlescreen.loaded = true;
  global_window_repaint = true;
}
export void titlescreen_unload() {
  titlescreen.loaded = false;
}
export void titlescreen_onkeydown(key_code_t key) {
  switch (key) {
    case KEY_SPACE:
      // transition_load(titlescreen_unload, hero_home_2f_load);
      break;
    default:
      return;
  }
}
export void titlescreen_render() {
  console_log("titlescreen_render");
  // // title
  // const wchar_t title_text [] = L"DreamShifters";
  // const f32 title_rect[4] = { 210.f, 10.f, MAX_F32, MAX_F32 };
  // // press_space
  // const wchar_t press_space_text [] = L"PRESS SPACE";
  // const f32 press_space_rect[4] = { 500.f, 500.f, MAX_F32, MAX_F32 };
  // // to_play
  // const wchar_t to_play_text [] = L"TO PLAY";
  // const f32 to_play_rect[4] = { 545.f, 530.f, MAX_F32, MAX_F32 };
}