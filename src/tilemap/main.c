#include <sdk/types.h>
#include <sdk/window.h>
#include <sdk/time.h>

#include <game/unity.h>

u16 window_width = 800;
u16 window_height = 600;

u8 pointer_layer = 0;
u8 pointer_tile_id = 0;
u8 pointer_flags = 0;

void window_onkeydown(key_t key) {
  switch (key) {
    case KEY_ADD:
      ++global_tilemap->tile_size;
      tilemap_onresize();
      break;
    case KEY_SUBTRACT:
      --global_tilemap->tile_size;
      tilemap_onresize();
      break;
    case KEY_ESCAPE:
      return window_close();
    case KEY_F5:
      break;
    case KEY_F12:
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
  f32 distance = .1f;
  f32 distance_x = 0;
  f32 distance_y = 0;
  if (window_key_pressed(KEY_UP)) {
    distance_y -= distance;
  } else if (window_key_pressed(KEY_DOWN)) {
    distance_y += distance;
  }
  if (window_key_pressed(KEY_LEFT)) {
    distance_x -= distance;
  } if (window_key_pressed(KEY_RIGHT)) {
    distance_x += distance;
  }
  tilemap_move(global_tilemap->x + distance_x, global_tilemap->y + distance_y);
}
void window_onmousemove() {
  if (pointer_tile_id) {
    window_updated = true;
  }
}
extern void window_onmousedown(i32 x, i32 y, mouse_btn_t button) {
  tile_t* tile = tile_from_screen();
  if (tile) {
    switch (button) {
      case MOUSE_BUTTON_LEFT: {
        tile->id = pointer_tile_id;
        tile->flags = pointer_flags;
        // chunk_t* chunk = tilemap_chunk_from_tile(tile);
        // if (chunk) {
        //   tilemap_chunk_save(chunk);
        // }
        window_updated = true;
        break;
      }
      case MOUSE_BUTTON_RIGHT:
        pointer_tile_id = tile->id;
        pointer_flags = tile->flags;
        window_updated = true;
        break;
      default:
        return;
    }
  }
}
void window_onscroll(i32 delta) {
  if (delta < 0 && pointer_tile_id > 0) {
    --pointer_tile_id;
  } else if (delta > 0) {
    ++pointer_tile_id;
  }
  window_updated = true;
}
void window_onrender() {
  tilemap_draw();
  // mouse_tile_draw
  if (pointer_tile_id > 0) {
    u8 tile_x = (pointer_tile_id - 1) % atlas_tiles_width;
    u8 tile_y = math_ceil((f32)pointer_tile_id / atlas_tiles_height) - 1.f;
    f32 pointer_x0 = -1 + mouse_x * window_pixel_ndc_x - global_tilemap->tile_ndc_pixel_x / 2;
    f32 pointer_y0 = 1 - mouse_y * window_pixel_ndc_y;
    f32 pointer_x1 = pointer_x0 + global_tilemap->tile_ndc_pixel_x;
    f32 pointer_y1 = pointer_y0 - global_tilemap->tile_ndc_pixel_y;
    tile_draw(pointer_x0, pointer_y0, pointer_x1, pointer_y1, tile_x, tile_y, pointer_flags);
  }
}
void window_onmouseup(i32 x, i32 y, mouse_btn_t button) {}
void window_dblclick() {}
void window_onkeyup() {}
void window_onresize() {
  if (global_tilemap) {
    tilemap_onresize();
  }
}
i32 main(i32 argc, char** argv) {
  window_startup("Tilemap", "assets/atlas.bin");
  tilemap_load();
  window_background[0] = 1.f;
  window_background[1] = 1.f;
  window_background[2] = 1.f;
  global_tilemap->player_hidden = true;
  window_run();
  return 0;
}