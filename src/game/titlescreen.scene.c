#include <game/titlescreen.scene.h>
#include <game/tilemap.scene.h>
#include <game/hero_home_2f.map.h>

#include <sdk/window.win32.h>

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
  // vertex
  buffer_t* vertices = buffer_header(global_vertices);
  global_vertices[vertices->length++] = (vertex_t) {
    -.5f, -.5f,
    0.f, 1.f
  };
  global_vertices[vertices->length++] = (vertex_t) {
    -.5f, .5,
    .0f, .0f
  };
  global_vertices[vertices->length++] = (vertex_t) {
    .5f, .5f,
      1.f, 0.f
  };
  global_vertices[vertices->length++] = (vertex_t) {
    .5f, -.5f,
    1.f, 1.f
  };
  // index
  buffer_t* indices = buffer_header(global_indices);
  global_indices[indices->length++] = 0;
  global_indices[indices->length++] = 1;
  global_indices[indices->length++] = 2;
  global_indices[indices->length++] = 0;
  global_indices[indices->length++] = 2;
  global_indices[indices->length++] = 3;
  console_log("titlescreen_render");
}