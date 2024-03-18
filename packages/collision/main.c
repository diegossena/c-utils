#include <sdk/window.h>
#include <sdk/time.h>
#include <sdk/keyboard.h>

f64 last_update;
gfx_rect_t rect = { .color = COLOR_BLACK };

gfx_rect_t rect_ = {
  .rect.left_top.x = 200,
  .rect.left_top.y = 200,
  .color = COLOR_ORANGE
};

void oncreate(window_t* this) {
  last_update = time_absolute();
}
void onupdate(window_t* this) {
  // timer
  f64 now = time_absolute();
  f32 elapsed = now - last_update;
  last_update = now;
  // handle inputs
  static const f32 velocity_per_tick = 200.f;
  if (keyboard_pressed(KEY_UP)) {
    rect.rect.left_top.y -= velocity_per_tick * elapsed;
  } else if (keyboard_pressed(KEY_DOWN)) {
    rect.rect.left_top.y += velocity_per_tick * elapsed;
  }
  if (keyboard_pressed(KEY_LEFT)) {
    rect.rect.left_top.x -= velocity_per_tick * elapsed;
  } else if (keyboard_pressed(KEY_RIGHT)) {
    rect.rect.left_top.x += velocity_per_tick * elapsed;
  }
  // update
  overlap_2d_t overlap = overlap_rect_rect(&rect.rect, &rect_.rect);
  if (overlap.distance) {
    f32 force = overlap.distance / 2;
    rect.rect.left_top = vector2d_sum(
      rect.rect.left_top,
      vector2d_mul_scalar(overlap.axis, force)
    );
    rect_.rect.left_top = vector2d_sum(
      rect_.rect.left_top,
      vector2d_mul_scalar(overlap.axis, -force)
    );
  }
  // render
  rect_set_size(&rect.rect, 100.f);
  rect_set_size(&rect_.rect, 100.f);
  gfx_draw_rect(this, &rect);
  gfx_draw_rect(this, &rect_);
}

i32 main() {
  application_t app;
  app_constructor(&app);
  window_options_t options = {
    .name = "Platform",
    .width = 800,
    .height = 600,
    .x = 0,
    .y = 0,
    .oncreate = oncreate,
    .onupdate = onupdate
  };
  window_startup(&app, &options);
  return app_run(&app);
}

// utils