#include <game/scenes/titlescreen.h>
#include <game/game.h>

titlescreen_t titlescreen = {
  .title = {
    .text = L"DreamShifters",
    .rect = { 210.f, 10.f }
  }
};

export void titlescreen_load() {
  titlescreen.loaded = true;
  // title
  titlescreen.title.color = gfx_color_new(1, 1, 1, 1);
  titlescreen.title.style = gfx_text_style_new(font_family_megaman, 32, FONT_WEIGHT_BOLD, FONT_STYLE_NORMAL);
  gfx_text_update(&titlescreen.title);
}
export void titlescreen_unload() {
  titlescreen.loaded = false;
  gfx_color_free(titlescreen.title.color);
  gfx_text_style_free(titlescreen.title.style);
}
export void titlescreen_onkeydown(key_code_t key) {
  switch (key) {
    case KEY_UP:
      --titlescreen.title.rect[1];
      break;
    case KEY_DOWN:
      ++titlescreen.title.rect[1];
      break;
    case KEY_LEFT:
      --titlescreen.title.rect[0];
      break;
    case KEY_RIGHT:
      ++titlescreen.title.rect[0];
      break;
    default:
      return;
  }
  gfx_text_update(&titlescreen.title);
  console_log("title %.0f %.0f", titlescreen.title.rect[0], titlescreen.title.rect[1]);
  global_repaint = true;
}
export void titlescreen_render() {
  gfx_text_draw(&titlescreen.title);
  // gfx_text_draw(&titlescreen.press_space);
  // gfx_text_draw(&titlescreen.to_play);
}