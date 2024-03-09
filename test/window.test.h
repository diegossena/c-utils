#include <sdk/sdk.h>

struct window_context_t {
  float pixel_width, pixel_height;
  text_style_t* text_style;
} window_context;
gfx_frect_t rect = { 10.0f, 10.0f, 0, 0 };

void onupdate(window_t* this) {
  gfx_draw_text_cstr(this, L"Hello World", &rect, window_context.text_style);

  float speed = 5.f;
  if (is_key_pressed(KEY_UP)) {
    rect.y -= speed;
  } else if (is_key_pressed(KEY_DOWN)) {
    rect.y += speed;
  }
  if (is_key_pressed(KEY_LEFT)) {
    rect.x -= speed;
  } else if (is_key_pressed(KEY_RIGHT)) {
    rect.x += speed;
  }
}
void onresize(window_t* this) {
  u16 width = window_get_screen_width(this);
  u16 height = window_get_screen_height(this);
  rect.width = width - 20.f;
  rect.height = height;
}
void onkeydown(window_t* this) {}
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