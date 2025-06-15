#include <sdk/types.h>
#include <sdk/window.h>

#define WINDOW_SCALING 3.5f

u16 window_width = 144 * WINDOW_SCALING;
u16 window_height = 256 * WINDOW_SCALING;
rgba_t window_background = { 0.329411765f, 0.752941176f, 0.788235294f, 1 };
const u16 atlas_width = 256;
const u16 atlas_height = 256;

#include <sdk/unity.h>

typedef enum bird_animate_state_t {
  BIRD_ANIMATE_UP,
  BIRD_ANIMATE_DOWN
} bird_animate_state_t;

typedef enum bird_wing_state_t {
  BIRD_WING_UP,
  BIRD_WING_MIDDLE,
  BIRD_WING_DOWN
} bird_wing_state_t;

const f32 bird_y0 = .4f;
const f32 bird_animate_offset = .04f;
f32 bird_animate_duration = .4f;
f32 bird_animate_timer = 0;
u8 bird_animate_state = BIRD_ANIMATE_DOWN;
u8 bird_wing_state = BIRD_WING_MIDDLE;

void window_dblclick() {}
void window_onmousemove() {}
void window_onmousedown(i32 x, i32 y, mouse_btn_t button) {}
void window_onmouseup(i32 x, i32 y, mouse_btn_t button) {}
void window_onscroll(i32 delta) {}
void window_onkeydown(key_t key) {}
void window_onkeyup(key_t key) {}
void window_onresize() {}
void window_onkeypress() {}

void window_onrender() {
  f32 x0, x1, y0, y1, u0, u1, v0, v1;
  // background_draw
  x0 = -1;
  x1 = 1;
  y0 = .3f;
  y1 = y0 - window_ndc_y * 64 * WINDOW_SCALING;
  u0 = 0;
  u1 = u0 + atlas_ndc_x * 144;
  v0 = atlas_ndc_y * 192;
  v1 = v0 + atlas_ndc_y * 64;
  window_rect_fill(
    -1, y1, 1, -1,
    0.329411765f, 0.878431373f, 0.458823529f, 1
  );
  window_rect_draw(x0, y0, x1, y1, u0, v0, u1, v1);
  // bird_animate
  bird_animate_timer += window_deltatime;
  f32 bird_animate_progress = math_clamp(bird_animate_timer / bird_animate_duration, 0, 1);
  switch (bird_animate_state) {
    case BIRD_ANIMATE_UP:
      y0 = bird_y0 + bird_animate_offset * (1.f - bird_animate_progress);
      if (bird_animate_timer >= bird_animate_duration) {
        bird_animate_state = BIRD_ANIMATE_DOWN;
        bird_animate_timer = 0;
      }
      break;
    case BIRD_ANIMATE_DOWN:
      y0 = bird_y0 + bird_animate_offset * bird_animate_progress;
      if (bird_animate_timer >= bird_animate_duration) {
        bird_animate_state = BIRD_ANIMATE_UP;
        bird_animate_timer = 0;
      }
      break;
  }
  // bird_draw
  x0 = -.25f;
  x1 = x0 + window_ndc_x * 16 * WINDOW_SCALING;
  y1 = y0 - window_ndc_y * 16 * WINDOW_SCALING;
  u0 = v0 = 0;
  u1 = v1 = atlas_ndc_x * 16;
  window_rect_draw(x0, y0, x1, y1, u0, v0, u1, v1);
  // bird_wing_draw
  x0 = x0 - window_ndc_x;
  x1 = x0 + window_ndc_x * 16 * WINDOW_SCALING;
  u0 = 16 * (1 + bird_wing_state) * atlas_ndc_x;
  u1 = u0 + atlas_ndc_x * 16;
  window_rect_draw(x0, y0, x1, y1, u0, v0, u1, v1);
  // end
  window_updated = true;
}

i32 main(i32 argc, char** argv) {
  window_startup("Flappy Bird", "assets/flappy_bird_atlas.bin");
  vertices_reserve(QUAD_VERTEX_COUNT * 4, QUAD_INDEX_COUNT * 4);
  window_run();
  return 0;
}