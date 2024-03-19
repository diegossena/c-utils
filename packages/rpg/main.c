#include "./game/game.h"

void oncreate(window_t* this) {
  this->context = game_new(this);
}
void onupdate(window_t* this) {
  game_render(this->context);
}
void onmousemove(window_t* this, vector2d_t cursor) {
  game_onmousemove(this->context, cursor);
}
void onmouseup(window_t* this, vector2d_t cursor) {
  game_onmouseup(this->context, cursor);
}
void onmousedown(window_t* this, vector2d_t cursor) {
  game_onmousedown(this->context, cursor);
}
void onclose(window_t* this) {
  game_free(this->context);
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
    .flags = WINDOW_NO_RESIZABLE | WINDOW_NO_MAXIMIZE,
    .oncreate = oncreate,
    .onupdate = onupdate,
    .onmousemove = onmousemove,
    .onmouseup = onmouseup,
    .onmousedown = onmousedown,
    .onclose = onclose
  };
  window_startup(&app, &options);
  return app_run(&app);
}

// utils