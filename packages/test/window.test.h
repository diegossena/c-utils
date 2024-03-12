#include <sdk/sdk.h>

struct window_context_t {
  u64 index;
} context;

text_props_t text_props = {
  .family = L"Arial",
  .size = 25.f,
  .style = FONT_STYLE_NORMAL,
  .weight = FONT_WEIGHT_BOLD,
  .color = { 1.f, 0.f, 0.f, 1.f },
  .rect = { 10.f, 10.f, 0.f, 0.f }
};
rect_props_t rect_props = {
  .rect = { 10.f, 10.f },
  .color = { 0.f, 1.f, 0.f, 1.f }
};
ellipse_props_t ellipse_props = {
  60.f, 60.f, 50.f, 50.f,
  .color = { 0.f, 0.f, 1.f, 1.f }
};
bitmap_props_t bitmap_props = {
  .rect = { 10.f, 10.f },
};

void onupdate(window_t* this) {
  float speed = 5.f;
  if (keyboard_pressed(KEY_UP)) {
    switch (context.index) {
      case 0: text_props.rect.top -= speed; break;
      case 1: rect_props.rect.top -= speed;  break;
      case 2: ellipse_props.y -= speed;  break;
      case 3: bitmap_props.rect.top -= speed; break;
    }
  } else if (keyboard_pressed(KEY_DOWN)) {
    switch (context.index) {
      case 0: text_props.rect.top += speed;  break;
      case 1: rect_props.rect.top += speed; break;
      case 2: ellipse_props.y += speed; break;
      case 3: bitmap_props.rect.top += speed; break;
    }
  }
  if (keyboard_pressed(KEY_LEFT)) {
    switch (context.index) {
      case 0: text_props.rect.left -= speed; break;
      case 1: rect_props.rect.left -= speed; break;
      case 2: ellipse_props.x -= speed; break;
      case 3: bitmap_props.rect.left -= speed; break;
    }
  } else if (keyboard_pressed(KEY_RIGHT)) {
    switch (context.index) {
      case 0: text_props.rect.left += speed;  break;
      case 1: rect_props.rect.left += speed; break;
      case 2: ellipse_props.x += speed; break;
      case 3: bitmap_props.rect.left += speed; break;
    }
  }
  gfx_draw_text_cwstr(this, L"Hello World", &text_props);
  gfx_draw_rect(this, &rect_props);
  gfx_draw_ellipse(this, &ellipse_props);
  // gfx_draw_bitmap(this, &bitmap_props);
}
void onresize(window_t* this) {
  u16 width = window_get_width(this);
  u16 height = window_get_height(this);
  text_props.rect.right = width - 20.f;
  text_props.rect.bottom = height;
}
void onkeydown(window_t* this) {
  if (keyboard_pressed(KEY_C)) {
    context.index = (context.index + 1) % 4;
  }
}
void onkeyup(window_t* this) {}
void onmousemove(window_t* this) {}
void onmousedown(window_t* this) {}
void onmouseup(window_t* this) {}
void ondblclick(window_t* this) {}
void oncreate(window_t* this) {
  context.index = 0;
  rect_set_size(&rect_props.rect, 100.f);
  rect_set_size(&bitmap_props.rect, 100.f);
  // bitmap_props.image = gfx_image_new(this, L"1678136103504.jpg");
}
void onclose(window_t* this) {
  // gfx_image_free(bitmap_props.image);
}

void window_test(application_t* app) {
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
  window_startup(app, &options);
}