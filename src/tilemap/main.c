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
  switch (button) {
    case MOUSE_BUTTON_LEFT:
      tilemap.tiles[layer][tile_y * TILEMAP_WIDTH + tile_x] = tile_id;
      break;
    case MOUSE_BUTTON_RIGHT:
      tile_id = tilemap.tiles[layer][tile_y * TILEMAP_WIDTH + tile_x];
      break;
    default:
  }
  console_log(
    "tiles[%u][%ld (%ld, %ld)] = %u",
    layer,
    tile_y * TILEMAP_WIDTH + tile_x,
    tile_x, tile_y,
    tilemap.tiles[layer][tile_y * TILEMAP_WIDTH + tile_x]
  );
  window_updated = true;
}
void window_onscroll(i32 delta) {
  if (delta < 0 && tile_id > 0) {
    --tile_id;
  } else if (delta > 0) {
    ++tile_id;
  }
  console_log("tile_id %u", tile_id);
}
extern void window_onmouseup(i32 x, i32 y, mouse_btn_t button) {}
void window_dblclick() {}
void window_onkeyup() {}
void window_onresize() {}
// 508855 bytes
i32 main(i32 argc, char** argv) {
  window_startup("assets/atlas.bin");
  window_set_title("Tilemap");
  tilemap_load();
  hero_home_2f_load();
  window_run();
  return 0;
}