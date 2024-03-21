#include "./game/game.h"

void onload(window_t* this) {
  this->context = game_new(this);
}
void oncreate(window_t* this) {
  game_oncreate(this->context);
}
void onupdate(window_t* this) {
  game_onupdate(this->context);
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
  console_startup();
  application_t app;
  app_new(&app);
  window_options_t options = {
    .name = "Platform",
    .width = 800,
    .height = 600,
    .x = 0,
    .y = 0,
    .flags = WINDOW_NO_RESIZABLE | WINDOW_NO_MAXIMIZE,
    .onload = onload,
    .oncreate = oncreate,
    .onupdate = onupdate,
    .onmousemove = onmousemove,
    .onmouseup = onmouseup,
    .onmousedown = onmousedown,
    .onclose = onclose
  };
  window_startup(&app, &options);
  app_run(&app);
  console_log_cstr(CONSOLE_FORE_GREEN "SUCCESS" CONSOLE_RESET);
}

// utils