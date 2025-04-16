#include <sdk/types.h>
#include <sdk/window.h>
#include <sdk/time.h>
#include <game/unity.h>

u16 window_width = 800;
u16 window_height = 600;

void window_onkeydown(key_code_t key) {
  if (key == KEY_ESCAPE) {
    return window_close();
  }
}
void window_onkeypress() {
  const f32 speed = .1f;
  if (window_key_pressed(KEY_UP)) {
    tilemap.offset.y -= speed;
  } else if (window_key_pressed(KEY_DOWN)) {
    tilemap.offset.y += speed;
  } else if (window_key_pressed(KEY_LEFT)) {
    tilemap.offset.x -= speed;
  } if (window_key_pressed(KEY_RIGHT)) {
    tilemap.offset.x += speed;
  }
  window_updated = true;
  console_log("offset %f %f", tilemap.offset.x, tilemap.offset.y);
}
void window_onrender() {
  tilemap_draw();
}
extern void window_onmousedown(i32 x, i32 y, mouse_btn_t button) {
  i32 tile_x = (x + tilemap.offset.x * TILE_SIZE) / 60;
  i32 tile_y = (y + tilemap.offset.y * TILE_SIZE) / 60;
  console_log("tile %ld %ld", tile_x, tile_y);
}
void window_onscroll(i32 delta) {
  console_log("scroll %ld", delta);
}
extern void window_onmouseup(i32 x, i32 y, mouse_btn_t button) {}
void window_dblclick() {}
void window_onkeyup() {}
void window_onresize() {}
// 508855 bytes
i32 main(i32 argc, char** argv) {
  window_startup("assets/atlas.bin");
  window_set_title("Tilemap");
  hero_home_2f_load();
  window_run();
  return 0;
}