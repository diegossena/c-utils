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
  // transition.color = gfx_color_new(1, 1, 1, 1);
  transition.loading = true;
  window_has_update = true;
}
export void transition_render() {
  const f32 duration = .85f;
  transition.timer += window_deltatime;
  f32 progress = math_clamp(transition.timer / duration, 0.f, 1.f);
  f32 opacity;
  if (transition.fade_in_out) {
    opacity = progress;
    if (transition.timer >= duration) {
      transition.unload();
      transition.load();
      transition.timer = 0.f;
      transition.fade_in_out = false;
    }
  } else {
    opacity = 1.f - progress;
    if (transition.timer >= duration) {
      transition.loading = false;
    }
  }
  rect_fill_draw(
    -1, 1, 1, -1,
    1, 1, 1, opacity
  );
  window_has_update = true;
}