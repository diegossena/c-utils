#include <sdk/sdk.h>

struct window_context_t {
  float pixel_width, pixel_height;
  u64 index;
} window_context;

text_props_t text_props = {
  .family = L"Arial",
  .size = 25.f,
  .style = FONT_STYLE_NORMAL,
  .weight = FONT_WEIGHT_BOLD,
  .color = { 1.f, 0.f, 0.f, 1.f },
  .rect = { 10.f, 10.f, 0.f, 0.f }
};
rect_props_t rect_props = {
  .rect = { 10.f, 10.f, .width = 100.f, .height = 100.f},
  .color = { 0.f, 1.f, 0.f, 1.f }
};

void onupdate(window_t* this) {
  u16 width = window_get_screen_width(this);
  u16 height = window_get_screen_height(this);
  float speed = 5.f;
  if (is_key_pressed(KEY_UP)) {
    text_props.rect.top -= speed;
    rect_props.rect.top -= speed;
  } else if (is_key_pressed(KEY_DOWN)) {
    text_props.rect.top += speed;
    rect_props.rect.top += speed;
  }
  if (is_key_pressed(KEY_LEFT)) {
    text_props.rect.left -= speed;
    rect_props.rect.left -= speed;
  } else if (is_key_pressed(KEY_RIGHT)) {
    text_props.rect.left += speed;
    rect_props.rect.left += speed;
  }

  switch (window_context.index) {
    case 0: gfx_draw_text_cwstr(this, L"Hello World", &text_props); break;
    case 1: gfx_draw_rect(this, &rect_props); break;
  }
}
void onresize(window_t* this) {
  u16 width = window_get_screen_width(this);
  u16 height = window_get_screen_height(this);
  text_props.rect.right = width - 20.f;
  text_props.rect.bottom = height;
}
void onkeydown(window_t* this) {
  if (is_key_pressed(KEY_C)) {
    window_context.index = (window_context.index + 1) % 2;
  }
}
void onkeyup(window_t* this) {}
void onmousemove(window_t* this) {}
void onmousedown(window_t* this) {}
void onmouseup(window_t* this) {}
void ondblclick(window_t* this) {}
void oncreate(window_t* this) {}
void onclose(window_t* this) {}

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