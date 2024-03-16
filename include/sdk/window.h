#ifndef SDK_WINDOW_H
#define SDK_WINDOW_H

#include <sdk/types.h>
#include <sdk/math/geometry_2d.h>

#define WINDOW_POS_CENTERED MAX_U64
#define gfx_draw_text_cwstr(this, text, props) { \
  const wchar_t __text[] = text; \
  gfx_draw_text(this, text, sizeof(text) / sizeof(wchar_t) - 1, props); \
}
#define COLOR_BLACK (gfx_color_t){ 0.f, 0.f, 0.f, 1.f }
#define COLOR_RED (gfx_color_t){ 1.f, 0.f, 0.f, 1.f }
#define COLOR_GREEN (gfx_color_t){ 0.f, 1.f, 0.f, 1.f }
#define COLOR_BLUE (gfx_color_t){ 0.f, 0.f, 1.f, 1.f }
#define COLOR_CYAN (gfx_color_t){ 0.f, 1.f, 1.f, 1.f }
#define COLOR_YELLOW (gfx_color_t){ 1.f, 1.f, 0.f, 1.f }
#define COLOR_ORANGE (gfx_color_t){ 1.f, .4f, 0.f, 1.f }

typedef struct application_t application_t;
typedef struct window_t window_t; // defined based on platform

typedef void (*window_event_cb)(window_t* this);
typedef void (*update_event_cb)(window_t* this);
typedef void (*keyboard_event_cb)(window_t* this);
typedef void (*ui_event_cb)(window_t* this);
typedef void (*mouse_event_cb)(window_t* this);

typedef enum window_flags_t {
  WINDOW_HIDDEN = 1,
  WINDOW_BORDELESS = 2,
  WINDOW_RESIZABLE = 4
} window_flags_t;
typedef enum font_weight_t {
  FONT_WEIGHT_NORMAL,
  FONT_WEIGHT_BOLD
} font_weight_t;
typedef enum font_style_t {
  FONT_STYLE_NORMAL
} font_style_t;

typedef struct window_options_t {
  const char* name;
  i32 x, y, width, height;
  u64 flags;
  void* context;
  // events
  update_event_cb onupdate;
  mouse_event_cb onmousemove;
  mouse_event_cb onmousedown;
  mouse_event_cb onmouseup;
  keyboard_event_cb onkeydown;
  keyboard_event_cb onkeyup;
  keyboard_event_cb ondblclick;
  ui_event_cb onresize;
  window_event_cb oncreate;
  window_event_cb onclose;
} window_options_t;
typedef struct gfx_color_t { f32 r, g, b, a; } gfx_color_t;
typedef struct bitmap_t bitmap_t; // defined based on platform
typedef struct text_props_t {
  const wchar_t* family;
  font_weight_t weight;
  font_style_t style;
  float size;
  rect_t rect;
  gfx_color_t color;
} text_props_t;

typedef struct rect_props_t {
  rect_t rect;
  gfx_color_t color;
} rect_props_t;
typedef struct ellipse_props_t {
  float x, y;
  float radius_x, radius_y;
  gfx_color_t color;
} ellipse_props_t;
typedef struct bitmap_props_t {
  rect_t rect;
  bitmap_t* image;
} bitmap_props_t;

void window_startup(application_t*, window_options_t*);

void* window_get_context(window_t*);
u16 window_get_width(window_t*);
u16 window_get_height(window_t*);

void gfx_draw_text(window_t*, const wchar_t* text, u64 length, text_props_t*);
void gfx_draw_rect(window_t*, rect_props_t*);
void gfx_draw_ellipse(window_t*, ellipse_props_t*);
void gfx_draw_bitmap(window_t*, bitmap_props_t*);

void gfx_bitmap_constructor(bitmap_t*, window_t*, const wchar_t* path);
void gfx_bitmap_free(bitmap_t*);

void window_free(window_t*);
void window_pooling();

#include <sdk/window/window.win32.inl.h>
#include <sdk/window/window.inl.h>

#endif