#include <sdk/types.h>
#include <sdk/window.h>

#define WINDOW_SCALING 3.8f
#define OBJECTS_COUNT 5

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

f32 bird_animate_timer = 0;
u8 bird_animate_state = BIRD_ANIMATE_DOWN;
u8 bird_wing_state = BIRD_WING_MIDDLE;

f32 ground_animate_timer = 0;

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
  window_rect_draw(x0, y0, x1, y1, u0, v0, u1, v1);
  // ground_animate
  const f32 ground_animate_duration = .2f;
  ground_animate_timer += window_deltatime;
  f32 ground_animate_progress = math_min(ground_animate_timer / ground_animate_duration, 1);
  const f32 ground_animate_offset = window_ndc_x * 12 * WINDOW_SCALING * ground_animate_progress;
  x0 = -1 - ground_animate_offset;
  x1 = 1.f + (window_ndc_x * 16 * WINDOW_SCALING) - ground_animate_offset;
  y0 = y1;
  y1 = y0 - window_ndc_y * 16 * WINDOW_SCALING;
  u0 = 0;
  u1 = atlas_ndc_x * 160;
  v0 = atlas_ndc_y * 160;
  v1 = v0 + atlas_ndc_y * 16;
  if (ground_animate_timer >= ground_animate_duration) {
    ground_animate_timer = 0;
  }
  // ground_draw
  window_rect_draw(x0, y0, x1, y1, u0, v0, u1, v1);
  y0 = y1;
  y1 = -1;
  window_rect_fill(
    x0, y0, x1, y1,
    0.870588235f, 0.843137255f, 0.596078431f, 1
  );
  // bird_animate
  bird_animate_timer += window_deltatime;
  const f32 bird_animate_duration = .4f;
  const f32 bird_animate_progress = math_min(bird_animate_timer / bird_animate_duration, 1);
  const f32 bird_animate_offset = .04f;
  const f32 bird_animate_y0 = .4f;
  switch (bird_animate_state) {
    case BIRD_ANIMATE_UP:
      y0 = bird_animate_y0 + bird_animate_offset * (1.f - bird_animate_progress);
      if (bird_animate_timer >= bird_animate_duration) {
        bird_animate_state = BIRD_ANIMATE_DOWN;
        bird_animate_timer = 0;
      }
      break;
    case BIRD_ANIMATE_DOWN:
      y0 = bird_animate_y0 + bird_animate_offset * bird_animate_progress;
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
  vertices_reserve(QUAD_VERTEX_COUNT * OBJECTS_COUNT, QUAD_INDEX_COUNT * OBJECTS_COUNT);
  window_run();
  return 0;
}