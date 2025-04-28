#include <sdk/types.h>
#include <sdk/window.h>
#include <sdk/time.h>
#include <game/unity.h>

u16 window_width = 800;
u16 window_height = 600;

u8 pointer_layer = 0;
u8 pointer_tile_id = 0;
u8 pointer_tile_index = 0;
u8 pointer_flags = 0;
f32 pointer_x0, pointer_y0, pointer_x1, pointer_y1;

void window_onkeydown(key_t key) {
  switch (key) {
    case KEY_ESCAPE:
      return window_close();
    case KEY_F5:
      hero_home_2f_load();
      break;
    case KEY_F12:
      console_write_str("{ ", 2);
      console_write_buffer(
        (void*)tilemap.tiles,
        tilemap.area * sizeof(tileblock_t) * TILEMAP_LAYERS
      );
      console_write_str(" }\n", 3);
      break;
    case KEY_W:
      if (pointer_layer > 0)
        --pointer_layer;
      console_log("layer %u", pointer_layer);
      break;
    case KEY_S:
      if (pointer_layer < 3)
        ++pointer_layer;
      console_log("layer %u", pointer_layer);
      break;
    case KEY_1:
      pointer_tile_index = 0;
      console_log("pointer_tile_index 0");
      break;
    case KEY_2:
      pointer_tile_index = 1;
      console_log("pointer_tile_index 1");
      break;
    case KEY_3:
      pointer_tile_index = 2;
      console_log("pointer_tile_index 2");
      break;
    case KEY_4:
      pointer_tile_index = 3;
      console_log("pointer_tile_index 3");
      break;
    case KEY_R:
      pointer_flags = (pointer_flags + 1) % 4;
      console_log("pointer_flags %u", pointer_flags);
      window_updated = true;
      break;
    default:
  }
}
void window_onkeypress() {
  const f32 speed = .1f;
  if (window_key_pressed(KEY_UP)) {
    tilemap.offset.y -= speed;
    window_updated = true;
  } else if (window_key_pressed(KEY_DOWN)) {
    tilemap.offset.y += speed;
    window_updated = true;
  }
  if (window_key_pressed(KEY_LEFT)) {
    tilemap.offset.x -= speed;
    window_updated = true;
  } if (window_key_pressed(KEY_RIGHT)) {
    tilemap.offset.x += speed;
    window_updated = true;
  }
  if (window_updated) {
    console_log("offset %f %f", tilemap.offset.x, tilemap.offset.y);
  }
}
void window_onmousemove() {
  pointer_x0 = -1 + mouse_x * window_pixel_ndc[0] - tilemap.tile_ndc_pixel[0] / 4;
  pointer_y0 = 1 - mouse_y * window_pixel_ndc[1];
  pointer_x1 = pointer_x0 + tilemap.tile_ndc_pixel[0] / 2;
  pointer_y1 = pointer_y0 - tilemap.tile_ndc_pixel[1] / 2;
  window_updated = true;
}
extern void window_onmousedown(i32 x, i32 y, mouse_btn_t button) {
  const i32 tile_x = math_floor((x + tilemap.offset.x * TILE_SIZE) / 60);
  const i32 tile_y = math_floor((y + tilemap.offset.y * TILE_SIZE) / 60);
  if (
    (tile_x < 0 && tile_x >= tilemap.width)
    || (tile_y < 0 && tile_y >= tilemap.height)
    ) return;
  switch (button) {
    case MOUSE_BUTTON_LEFT:
      tilemap_tile(pointer_layer, tile_x, tile_y).tiles[pointer_tile_index].id = pointer_tile_id;
      break;
    case MOUSE_BUTTON_RIGHT:
      pointer_tile_id = tilemap_tile(pointer_layer, tile_x, tile_y).tiles[pointer_tile_index].id;
      pointer_flags = tilemap_tile(pointer_layer, tile_x, tile_y).tiles[pointer_tile_index].flags;
      break;
    default:
      return;
  }
  window_updated = true;
}
void window_onscroll(i32 delta) {
  if (delta < 0 && pointer_tile_id > 0) {
    --pointer_tile_id;
  } else if (delta > 0) {
    ++pointer_tile_id;
  }
  window_updated = true;
  console_log("tile_id %u", pointer_tile_id);
}
void window_onrender() {
  tilemap_draw();
  // mouse_tile_draw
  if (pointer_tile_id > 0) {
    u8 tile_x = (pointer_tile_id - 1) % 10;
    u8 tile_y = math_ceil((f32)pointer_tile_id / 10.f) - 1.f;
    tile_draw(pointer_x0, pointer_y0, pointer_x1, pointer_y1, tile_x, tile_y, pointer_flags);
  } else {
    window_rect_fill(
      pointer_x0, pointer_y0, pointer_x1, pointer_y1,
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
  tilemap.player_hidden = true;
  hero_home_2f_load();
  window_run();
  return 0;
}