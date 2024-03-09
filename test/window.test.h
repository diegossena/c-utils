#include <sdk/sdk.h>

struct window_context_t {
  float pixel_width, pixel_height;
  text_style_t* text_style;
  u64 index;
} window_context;
gfx_frect_t rect_text = { 10.0f, 10.0f, 0, 0 };
gfx_frect_t rect = { 10.0f, 10.0f, 100.f, 100.f };

void onupdate(window_t* this) {
  float speed = 5.f;
  if (is_key_pressed(KEY_UP)) {
    rect_text.y -= speed;
    rect.y -= speed;
  } else if (is_key_pressed(KEY_DOWN)) {
    rect_text.y += speed;
    rect.y += speed;
  }
  if (is_key_pressed(KEY_LEFT)) {
    rect_text.x -= speed;
    rect.x -= speed;
  } else if (is_key_pressed(KEY_RIGHT)) {
    rect_text.x += speed;
    rect.x += speed;
  }

  switch (window_context.index) {
    case 0:
      gfx_draw_text_cwstr(this, L"Hello World", &rect_text, window_context.text_style);
      break;
    case 1:
      gfx_draw_rect(this, &rect);
      break;
  }
}
void onresize(window_t* this) {
  u16 width = window_get_screen_width(this);
  u16 height = window_get_screen_height(this);
  rect_text.width = width - 20.f;
  rect_text.height = height;
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

void oncreate(window_t* this) {
  text_style_properties_t text_style_properties = {
    .family = L"Roboto",
    .size = 25.f,
    .style = FONT_STYLE_NORMAL,
    .weight = FONT_WEIGHT_BOLD
  };
  window_context.text_style = text_style_new(&text_style_properties, this);
}
void onclose(window_t* this) {
  text_style_free(window_context.text_style);
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
    .ondblclick = ondblclick,
    .onclose = onclose
  };
  window_inicialize(&options);
}