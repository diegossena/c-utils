#include <sdk/types.h>
#include <sdk/error.h>
#include <sdk/assert.h>
#include <sdk/window.h>
#include <sdk/date.h>
#include <game/unity.h>

void window_onkeyup() {}
void window_onkeydown(key_code_t key) {
  if (state.titlescreen.loaded) {
    titlescreen_onkeydown(key);
  }
}
void window_onkeypress() {}

void window_onrender() {
  window_clear();
  gfx_color_t* white = gfx_color_new(1, 1, 1, 1);
  gfx_text_style_t* style = gfx_text_style_new("ARIAL", 16, FONT_WEIGHT_MEDIUM, FONT_STYLE_NORMAL);
  gfx_text_t text = {
    .text = L"TEST",
    .color = white,
    .rect = { 0, 0, 0, 0 },
    .style = style
  };
  gfx_text_adjust(&text);
  gfx_text_draw(&text);
  gfx_color_free(white);
  gfx_text_style_free(style);
  if (state.titlescreen.loaded) {
    titlescreen_render();
  }
}
void window_onclose() {
  if (state.titlescreen.loaded) {
    titlescreen_unload();
  }
}

i32 main(i32 argc, char** argv) {
  window_startup();
  sync_wait(global_window_onload_sync);
  window_set_title("Game");
  u64 time = date_now();

  while (global_window_thread) {
    u64 now = date_now();
    u64 delta_time = now - time;
    if (delta_time > 15) {
      time = now;
      if (state.repaint) {
        window_redraw();
        state.repaint = false;
      }
    }
  }
  return 0;
}