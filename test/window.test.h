#include <sdk/sdk.h>

void oncreate(window_t* this) {
  // console_log("oncreate");
}
void onupdate(window_t* this) {
  // console_log("onupdate");
}
void onkeydown(window_t* this) {
  // console_log("onkeydown");
}
void onkeyup(window_t* this) {
  // console_log("onkeyup");
}
void onresize(window_t* this) {
  // console_log("onresize");
}
void onmousemove(window_t* this) {
  // console_log("onmousemove");
}
void onmousedown(window_t* this) {
  // console_log("onmousedown");
}
void onmouseup(window_t* this) {
  // console_log("onmouseup");
}
void ondblclick(window_t* this) {
  // console_log("ondblclick");
}

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
    .ondblclick = ondblclick
  };
  window_inicialize(&options);
}