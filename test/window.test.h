#include <sdk/sdk.h>

struct window_context_t {
  char* level;
  i32 level_width;
  i32 level_height;
  f32 camera_x, camera_y;
  f32 player_x, player_y;
  f32 player_vel_x, player_vel_y;
  i32 index;
  f64 last_update;
} context;

// utils

char get_tile(i32 x, i32 y) {
  if (x >= 0 && x < context.level_width && y >= 0 && y < context.level_height) {
    return context.level[y * context.level_width + x];
  }
  return ' ';
}

// events

f64 elapsed = 0.;
void onupdate(window_t* this) {
  // timer
  f64 now = time_absolute();
  f64 delta_time = now - context.last_update;
  context.last_update = now;
  elapsed += delta_time;
  // handle input
  context.player_vel_x = 0.f;
  context.player_vel_y = 0.f;
  f32 velocity = 6.f;
  if (is_key_pressed(KEY_UP)) {
    context.player_vel_y = -velocity;
  } else if (is_key_pressed(KEY_DOWN)) {
    context.player_vel_y = velocity;
  }
  if (is_key_pressed(KEY_LEFT)) {
    context.player_vel_x = -velocity;
  } else if (is_key_pressed(KEY_RIGHT)) {
    context.player_vel_x = velocity;
  }
  context.player_x = context.player_x + context.player_vel_x * delta_time;
  context.player_y = context.player_y + context.player_vel_y * delta_time;
  context.camera_x = context.player_x;
  context.camera_y = context.player_y;
  // window info
  u16 window_width = window_get_screen_width(this);
  u16 window_height = window_get_screen_height(this);
  // level info
  i32 tile_size = 50;
  i32 visible_tiles_x = window_width / tile_size;
  i32 visible_tiles_y = window_height / tile_size;
  // calculate top-leftmost visible tile
  f32 offset_x = context.camera_x - (f32)visible_tiles_x / 2.f;
  f32 offset_y = context.camera_y - (f32)visible_tiles_y / 2.f;
  // clamp camera to game boudaries
  offset_x = math_clamp(offset_x, 0.f, context.level_width - visible_tiles_x);
  offset_y = math_clamp(offset_y, 0.f, context.level_height - visible_tiles_y);
  // Get offsets for smooth movement
  f32 tile_offset_x = (offset_x - (i32)offset_x) * tile_size;
  f32 tile_offset_y = (offset_y - (i32)offset_y) * tile_size;
  // draw visible tile map
  for (i32 x = -1; x < visible_tiles_x + 1; x++) {
    rect_props_t rect_props = {
      .rect = { 10.f, 10.f, .size = 100.f},
      .color = { 0.f, 1.f, 0.f, 1.f }
    };
    for (i32 y = -1; y < visible_tiles_y + 1; y++) {
      char tile_id = get_tile(x + offset_x, y + offset_y);
      rect_props_t tile_props = {
        .rect = {
          .left = x * tile_size - tile_offset_x,
          .top = y * tile_size - tile_offset_y,
          .size = tile_size
        },
        .color = {.a = 1.f }
      };
      if (x == 0 && y == 0) {
        if (elapsed >= 0.1) {
          elapsed = 0.;
          console_log("x=%d", x);
          console_log("tile_size=%d", tile_size);
          console_log("offset_x=%f", offset_x);
          console_log("tile_props.rect.left=%f", tile_props.rect.left);
          console_log("tile_props.rect.top=%f", tile_props.rect.top);
        }
      } else {
      }
      switch (tile_id) {
        case '.': // Sky
          tile_props.color.r = 0.f;
          tile_props.color.g = 1.f;
          tile_props.color.b = 1.f;
          break;
        case '#': // Solid Block
          tile_props.color.r = 1.f;
          break;
        default:
          break;
      }
      gfx_draw_rect(this, &tile_props);
    }
  }

  // draw player
  rect_props_t player_props = {
    .rect = {
      .left = (context.player_x - offset_x) * tile_size,
      .top = (context.player_y - offset_y) * tile_size,
      .size = tile_size
    },
    .color = { 1.f, 0.f, 0.f, 1.f }
  };
  gfx_draw_rect(this, &player_props);
}
void onresize(window_t* this) {
  u16 width = window_get_screen_width(this);
  u16 height = window_get_screen_height(this);
}
void onkeydown(window_t* this) {
  if (is_key_pressed(KEY_C)) {
    context.index = (context.index + 1) % 4;
  }
}
void onkeyup(window_t* this) {}
void onmousemove(window_t* this) {}
void onmousedown(window_t* this) {}
void onmouseup(window_t* this) {}
void ondblclick(window_t* this) {}
void oncreate(window_t* this) {
  context.last_update = time_absolute();
  context.index = 0;
  // bitmap_props.image = gfx_image_new(this, L"1678136103504.jpg");
  context.level = (
    "................................................................"
    ".......................GGGGGGGG................................."
    ".......ooooo...................................................."
    "........ooo....................................................."
    ".......................########................................."
    ".....BB?BBBB?BB.......###..............#.#......................"
    "....................###................#.#......................"
    "...................####........................................."
    "####################################.##############.....########"
    "...................................#.#...............###........"
    "........................############.#............###..........."
    "........................#............#.........###.............."
    "........................#.############......###................."
    "........................#................###...................."
    "........................#################......................."
    "................................................................"
    );
  context.level_width = 64;
  context.level_height = 16;
  context.camera_x = 0.f;
  context.camera_y = 0.f;
  context.player_x = 0.f;
  context.player_y = 0.f;
  context.player_vel_x = 0.f;
  context.player_vel_y = 0.f;
}
void onclose(window_t* this) {
  // gfx_image_free(bitmap_props.image);
}

void window_test() {
  window_opt options = {
    .name = "Window",
    .width = 800,
    .height = 600,
    .x = 0,
    .y = 0,
    .oncreate = oncreate,
    .onupdate = onupdate,
    .onkeydown = onkeydown,
    .onkeyup = onkeyup,
    .onresize = onresize,
    .onmousemove = onmousemove,
    .onmousedown = onmousedown,
    .onmouseup = onmouseup,
    .ondblclick = ondblclick,
    .onclose = onclose
  };
  window_inicialize(&options);
}