#include <game/transition.scene.h>

#include <sdk/window.win32.h>
transition_t transition;

export void transition_load(function_t unload, function_t load) {
  if (transition.loading)
    return;
  transition.timer = 0;
  transition.unload = unload;
  transition.load = load;
  transition.fade_in_out = true;
  transition.color = gfx_color_new(1, 1, 1, 1);
  transition.loading = true;
  global_repaint = true;
}
export void transition_render() {
  const f32 duration = .85f;
  transition.timer += global_delta_time;
  f32 progress = math_clamp(transition.timer / duration, 0.f, 1.f);
  if (transition.fade_in_out) {
    gfx_color_set_opacity(transition.color, progress);
    if (transition.timer >= duration) {
      transition.unload();
      transition.load();
      transition.timer = 0.f;
      transition.fade_in_out = false;
    }
  } else {
    gfx_color_set_opacity(transition.color, 1.f - progress);
    if (transition.timer >= duration) {
      transition.loading = false;
    }
  }
  const f32 rect[4] = { 0, 0, MAX_RECT, MAX_RECT };
  gfx_fill_rect(rect, transition.color);
  if (!transition.loading) {
    gfx_color_free(transition.color);
  }
  global_repaint = true;
}