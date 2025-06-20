#include <game/transition.scene.h>

#define TRANSITION_VERTICES_USED 4
#define TRANSITION_INDEXES_USED 6

transition_t transition;

void transition_load(function_t unload, function_t load) {
  if (transition.loading)
    return;
  vertices_reserve(
    vertices_capacity + TRANSITION_VERTICES_USED,
    indexes_capacity + TRANSITION_INDEXES_USED
  );
  transition.timer = 0;
  transition.unload = unload;
  transition.load = load;
  transition.loading = true;
  transition.state = TRANSITION_ENTERING;
}
void transition_render() {
  const f32 duration = .85f;
  transition.timer += window_deltatime;
  f32 progress = transition.timer / duration;
  progress = math_clamp(transition.timer / duration, 0.f, 1.f);
  f32 opacity;
  switch (transition.state) {
    case TRANSITION_ENTERING:
      opacity = progress;
      if (transition.timer >= duration) {
        transition.unload();
        transition.load();
        transition.timer = 0.f;
        transition.state = TRANSITION_EXITING;
      }
      break;
    case TRANSITION_EXITING:
      opacity = math_max(1.f - progress, 0.001f);
      if (transition.timer >= duration) {
        transition.state = TRANSITION_EXITED;
      }
      break;
    case TRANSITION_EXITED:
    default:
      vertices_reserve(
        vertices_capacity - TRANSITION_VERTICES_USED,
        indexes_capacity - TRANSITION_INDEXES_USED
      );
      transition.loading = false;
      return;
  }
  window_rect_fill(
    -1, 1, 1, -1,
    1, 1, 1, opacity
  );
}