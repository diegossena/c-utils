#include <sdk/sdk.h>

struct window_context_t {
  float pixel_width, pixel_height;
} window_context;

void onresize(window_t* this) {
}
void oncreate(window_t* this) {

}
void onclose(window_t* this) {}
void onupdate(window_t* this) {}
void onkeydown(window_t* this) {}
void onkeyup(window_t* this) {}
void onmousemove(window_t* this) {}
void onmousedown(window_t* this) {}
void onmouseup(window_t* this) {}
void ondblclick(window_t* this) {}

void window_test() {
  window_opt options = {
    .name = "Window",
    .width = 800,
    .height = 600,
    .x = 0,
    .y = 0,
    .oncreate = oncreate,
    .onupdate = onupdate,
    .onkeydown = onkeydown,
    .onkeyup = onkeyup,
    .onresize = onresize,
    .onmousemove = onmousemove,
    .onmousedown = onmousedown,
    .onmouseup = onmouseup,
    .ondblclick = ondblclick,
    .onclose = onclose
  };
  window_inicialize(&options);
}