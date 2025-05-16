#include <sdk/types.h>
#include <sdk/window.h>
#include <sdk/time.h>
#include <game/unity.h>

u16 window_width = 800;
u16 window_height = 600;

u8 pointer_layer = 0;
u8 pointer_tile_id = 0;
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
        (void*)tilemap->tiles,
        tilemap->area * sizeof(tile_t) * TILEMAP_LAYERS
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
    tilemap->offset.y -= speed;
    window_updated = true;
  } else if (window_key_pressed(KEY_DOWN)) {
    tilemap->offset.y += speed;
    window_updated = true;
  }
  if (window_key_pressed(KEY_LEFT)) {
    tilemap->offset.x -= speed;
    window_updated = true;
  } if (window_key_pressed(KEY_RIGHT)) {
    tilemap->offset.x += speed;
    window_updated = true;
  }
  if (window_updated) {
    console_log("offset %f %f", tilemap->offset.x, tilemap->offset.y);
  }
}
void window_onmousemove() {
  // const i32 tile_x = math_floor((mouse_x + tilemap->offset.x * TILE_SIZE) / TILE_SIZE);
  // const i32 tile_y = math_floor((mouse_y + tilemap->offset.y * TILE_SIZE) / TILE_SIZE);
  // console_log("x %d y %d tile[%ld %ld] offset[%f, %f]", mouse_x, mouse_y, tile_x, tile_y, tilemap->offset.x, tilemap->offset.y);

  pointer_x0 = -1 + mouse_x * window_pixel_ndc[0] - tilemap->tile_ndc_pixel[0] / 2;
  pointer_y0 = 1 - mouse_y * window_pixel_ndc[1];
  pointer_x1 = pointer_x0 + tilemap->tile_ndc_pixel[0];
  pointer_y1 = pointer_y0 - tilemap->tile_ndc_pixel[1];
  window_updated = true;
}
extern void window_onmousedown(i32 x, i32 y, mouse_btn_t button) {
  const i32 tile_x = math_floor((x + tilemap->offset.x * TILE_SIZE) / TILE_SIZE);
  const i32 tile_y = math_floor((y + tilemap->offset.y * TILE_SIZE) / (TILE_SIZE - 1));
  console_log("x %d y %d tile[%ld %ld] offset[%f, %f]", x, y, tile_x, tile_y, tilemap->offset.x, tilemap->offset.y);
  if (
    tile_x < 0
    || tile_x >= tilemap->width
    || tile_y < 0
    || tile_y >= tilemap->height
    ) return;
  switch (button) {
    case MOUSE_BUTTON_LEFT:
      tilemap_tile(pointer_layer, tile_x, tile_y).id = pointer_tile_id;
      tilemap_tile(pointer_layer, tile_x, tile_y).flags = pointer_flags;
      break;
    case MOUSE_BUTTON_RIGHT:
      pointer_tile_id = tilemap_tile(pointer_layer, tile_x, tile_y).id;
      pointer_flags = tilemap_tile(pointer_layer, tile_x, tile_y).flags;
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
    u8 tile_x = (pointer_tile_id - 1) % atlas_tiles_in_x;
    u8 tile_y = math_ceil((f32)pointer_tile_id / atlas_tiles_in_y) - 1.f;
    tile_draw(pointer_x0, pointer_y0, pointer_x1, pointer_y1, tile_x, tile_y, pointer_flags);
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
  window_background[0] = 1.f;
  window_background[1] = 1.f;
  window_background[2] = 1.f;
  tilemap->player_hidden = true;
  hero_home_2f_load();
  window_run();
  return 0;
}