#include "./game/game_context.h"

void oncreate(window_t* this) {
  game_context_t* context = game_new(this);
  window_set_context(this, context);
}
void onupdate(window_t* this) {
  game_context_t* context = window_get_context(this);
  game_update(context);
}
void onclose(window_t* this) {
  game_context_t* context = window_get_context(this);
  game_free(context);
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
    .onupdate = onupdate,
    .onclose = onclose
  };
  window_startup(&app, &options);
  return app_run(&app);
}

// utils