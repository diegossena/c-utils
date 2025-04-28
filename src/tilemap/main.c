#include <sdk/types.h>
#include <sdk/window.h>
#include <sdk/time.h>
#include <game/unity.h>

u16 window_width = 800;
u16 window_height = 600;

u8 layer = 0;
u8 tile_id = 0;

void window_onkeydown(key_code_t key) {
  switch (key) {
    case KEY_ESCAPE:
      return window_close();
    case KEY_F5:
      hero_home_2f_load();
      break;
    case KEY_W:
      if (layer > 0)
        --layer;
      console_log("layer %u", layer);
      break;
    case KEY_S:
      if (layer < 3)
        ++layer;
      console_log("layer %u", layer);
      break;
    default:
  }
}
void window_onkeypress() {
  const f32 speed = .1f;
  if (window_key_pressed(KEY_UP)) {
    tilemap.offset.y -= speed;
  } else if (window_key_pressed(KEY_DOWN)) {
    tilemap.offset.y += speed;
  }
  if (window_key_pressed(KEY_LEFT)) {
    tilemap.offset.x -= speed;
  } if (window_key_pressed(KEY_RIGHT)) {
    tilemap.offset.x += speed;
  }
  window_updated = true;
  console_log("offset %f %f", tilemap.offset.x, tilemap.offset.y);
}
void window_onmousemove() {
  window_updated = true;
}
extern void window_onmousedown(i32 x, i32 y, mouse_btn_t button) {
  i32 tile_x = (x + tilemap.offset.x * TILE_SIZE) / 60;
  i32 tile_y = (y + tilemap.offset.y * TILE_SIZE) / 60;
  if (
    (tile_x < 0 && tile_x >= tilemap.width)
    || (tile_y < 0 && tile_y >= tilemap.height)
    ) return;
  switch (button) {
    case MOUSE_BUTTON_LEFT:
      tilemap_tile_get(layer, tile_x, tile_y) = tile_id;
      break;
    case MOUSE_BUTTON_RIGHT:
      tile_id = tilemap_tile_get(layer, tile_x, tile_y);
      break;
    default:
      return;
  }
  console_log(
    "tiles[%u][%ld (%ld, %ld)] = %u",
    layer,
    tile_y * tilemap.width + tile_x,
    tile_x, tile_y,
    tilemap_tile_get(layer, tile_x, tile_y)
  );
  window_updated = true;
}
void window_onscroll(i32 delta) {
  if (delta < 0 && tile_id > 0) {
    --tile_id;
  } else if (delta > 0) {
    ++tile_id;
  }
  window_updated = true;
  console_log("tile_id %u", tile_id);
}
void window_onrender() {
  tilemap_draw();
  // mouse_tile_draw
  f32 x0 = -1 + mouse_x * window_pixel_ndc[0] - tilemap.tile_ndc_pixel[0] / 2;
  f32 y0 = 1 - mouse_y * window_pixel_ndc[1] + tilemap.tile_ndc_pixel[1] / 2;
  f32 x1 = x0 + tilemap.tile_ndc_pixel[0];
  f32 y1 = y0 - tilemap.tile_ndc_pixel[1];
  if (tile_id > 0) {
    u8 tile_x = (tile_id - 1) % 10;
    u8 tile_y = math_ceil((f32)tile_id / 10.f) - 1.f;
    tile_draw(x0, y0, x1, y1, tile_x, tile_y, false, false);
  } else {
    window_rect_fill(
      x0, y0, x1, y1,
      0, 0, 0, 1
    );
  }
}
void window_onmouseup(i32 x, i32 y, mouse_btn_t button) {}
void window_dblclick() {}
void window_onkeyup() {}
void window_onresize() {}
// 508855 bytes
i32 main(i32 argc, char** argv) {
  window_startup("Tilemap", "assets/atlas.bin");
  tilemap_load();
  hero_home_2f_load();
  window_run();
  return 0;
}