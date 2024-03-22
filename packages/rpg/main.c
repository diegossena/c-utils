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
void onmousemove(window_t* this) {
  game_onmousemove(this->context);
}
void onmouseup(window_t* this) {
  game_onmouseup(this->context);
}
void onmousedown(window_t* this) {
  game_onmousedown(this->context);
}
void onkeydown(window_t* this) {
  game_onkeydown(this->context);
}
void onclose(window_t* this) {
  game_free(this->context);
}

i32 main() {
  console_startup();
  application_t app;
  app_new(&app);
  window_options_t options = {
    .name = "RPG",
    .width = 800,
    .height = 600,
    .x = 0,
    .y = 0,
    .flags = WINDOW_NO_RESIZABLE | WINDOW_NO_MAXIMIZE,
    .onload = (listener_t)onload,
    .oncreate = (listener_t)oncreate,
    .onupdate = (listener_t)onupdate,
    .onmousemove = (listener_t)onmousemove,
    .onmouseup = (listener_t)onmouseup,
    .onmousedown = (listener_t)onmousedown,
    .onkeydown = (listener_t)onkeydown,
    .onclose = (listener_t)onclose
  };
  window_startup(&app, &options);
  app_run(&app);
  console_log_cstr(CONSOLE_FORE_GREEN "SUCCESS" CONSOLE_RESET);
}

// utils