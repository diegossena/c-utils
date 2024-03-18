#ifndef SDK_WINDOW_H
#define SDK_WINDOW_H

#include <sdk/types.h>
#include <sdk/math/geometry_2d.h>

#define gfx_textnode_cwstr(text) text, sizeof(text) / sizeof(wchar_t) - 1

#define WINDOW_POS_CENTERED MAX_U64
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
typedef void (*mouse_event_cb)(window_t* this, vector2d_t cursor);

typedef enum window_flags_t {
  WINDOW_HIDDEN = 1,
  WINDOW_NO_RESIZABLE = 2,
  WINDOW_NO_MAXIMIZE = 4,
} window_flags_t;
typedef enum font_weight_t {
  FONT_WEIGHT_NORMAL,
  FONT_WEIGHT_BOLD
} font_weight_t;
typedef enum font_style_t {
  FONT_STYLE_NORMAL
} font_style_t;
typedef enum border_style_t {
  BORDER_STYLE_SOLID
} border_style_t;

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
  mouse_event_cb ondblclick;
  keyboard_event_cb onkeydown;
  keyboard_event_cb onkeyup;
  ui_event_cb onresize;
  window_event_cb oncreate;
  window_event_cb onclose;
} window_options_t;

typedef struct gfx_color_t { f32 r, g, b, a; } gfx_color_t;
typedef struct bitmap_t bitmap_t; // defined based on platform

typedef struct gfx_border_t {
  f32 width;
  border_style_t style;
  gfx_color_t color;
  f32 radius;
} gfx_border_t;
typedef struct gfx_rect_t {
  rect_t rect;
  gfx_color_t color;
  gfx_border_t border;
} gfx_rect_t;
typedef struct gfx_bitmap_t {
  rect_t rect;
  bitmap_t* image;
  f32 border_width;
} gfx_bitmap_t;
typedef struct gfx_text_style_t {
  const wchar_t* family;
  font_weight_t weight;
  font_style_t style;
  float size;
  gfx_color_t color;
} gfx_text_style_t;
typedef struct gfx_textnode_t {
  const wchar_t* text;
  u64 length;
  gfx_text_style_t* style;
  rect_t* parent;
} gfx_textnode_t;

void window_startup(application_t*, window_options_t*);

sdk_inline void* window_get_context(window_t*);
sdk_inline void window_set_context(window_t*, void*);
u16 window_get_width(window_t*);
u16 window_get_height(window_t*);
void window_set_size(window_t*, i32 width, i32 height);
vector2d_t window_get_cursor(window_t*);

void gfx_draw_text(const window_t*, const gfx_textnode_t*);
void gfx_draw_rect(window_t*, gfx_rect_t*);
void gfx_draw_bitmap(window_t*, gfx_bitmap_t*);

bitmap_t* gfx_bitmap_new(window_t*, const wchar_t* path);
void gfx_bitmap_free(bitmap_t*);

void window_free(window_t*);
void window_pooling();

#include <sdk/window/window.win32.inl.h>
#include <sdk/window/window.inl.h>

#endif