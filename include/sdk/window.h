#ifndef SDK_WINDOW_H
#define SDK_WINDOW_H

#include <sdk/types.h>
#include <sdk/math/geometry_2d.h>

#define gfx_textnode_cwstr(text) text, sizeof(text) / sizeof(wchar_t) - 1

#define WINDOW_POS_CENTERED MAX_U64
#define COLOR_BLACK (color_t){ 0.f, 0.f, 0.f, 1.f }
#define COLOR_RED (color_t){ 1.f, 0.f, 0.f, 1.f }
#define COLOR_GREEN (color_t){ 0.f, 1.f, 0.f, 1.f }
#define COLOR_BLUE (color_t){ 0.f, 0.f, 1.f, 1.f }
#define COLOR_CYAN (color_t){ 0.f, 1.f, 1.f, 1.f }
#define COLOR_YELLOW (color_t){ 1.f, 1.f, 0.f, 1.f }
#define COLOR_ORANGE (color_t){ 1.f, .4f, 0.f, 1.f }

typedef struct application_t application_t;
typedef struct window_t window_t; // defined based on platform
typedef struct bitmap_t bitmap_t;
typedef struct gfx_color_t gfx_color_t;
typedef struct gfx_stroke_t gfx_stroke_t;
typedef struct gfx_textformat_t gfx_textformat_t;

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
  FONT_WEIGHT_THIN = 100,
  FONT_WEIGHT_EXTRA_LIGHT = 200,
  FONT_WEIGHT_LIGHT = 300,
  FONT_WEIGHT_NORMAL = 400,
  FONT_WEIGHT_MEDIUM = 500,
  FONT_WEIGHT_SEMI_BOLD = 600,
  FONT_WEIGHT_BOLD = 700,
  FONT_WEIGHT_EXTRA_BOLD = 800,
  FONT_WEIGHT_BLACK = 900,
  FONT_WEIGHT_EXTRA_BLACK = 950
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
  window_event_cb onload;
  window_event_cb oncreate;
  window_event_cb onclose;
} window_options_t;
typedef struct color_t { f32 r, g, b, a; } color_t;
typedef struct stroke_t {
  border_style_t style;
} stroke_t;
typedef struct gfx_bitmap_t {
  rect_t rect;
  bitmap_t* image;
} gfx_bitmap_t;
typedef struct gfx_rect_t {
  rect_t rect;
  gfx_color_t* color;
  gfx_stroke_t* stroke;
  f32 border_width;
  f32 border_radius;
} gfx_rect_t;
typedef struct gfx_textformat_props_t {
  window_t* window;
  f32 size;
  const wchar_t* family;
  font_weight_t weight;
  font_style_t style;
} gfx_textformat_props_t;
typedef struct gfx_text_t {
  const wchar_t* text;
  u64 length;
  vector2d_t position;
  gfx_color_t* color;
  gfx_textformat_t* format;
} gfx_text_t;

void window_startup(application_t*, window_options_t*);

sdk_inline void* window_get_context(window_t*);
sdk_inline void window_set_context(window_t*, void*);
u16 window_get_width(window_t*);
u16 window_get_height(window_t*);
void window_set_size(window_t*, i32 width, i32 height);
vector2d_t window_get_cursor(window_t*);

void gfx_stroke_new(gfx_stroke_t*, window_t*, stroke_t);
void gfx_stroke_free(gfx_stroke_t*);

void gfx_color_new(gfx_color_t*, window_t*, color_t);
void gfx_color_free(gfx_color_t*);

void gfx_draw_text(const window_t*, const gfx_text_t*);
void gfx_draw_rect(window_t*, gfx_rect_t*);
void gfx_draw_bitmap(window_t*, gfx_bitmap_t*);

void gfx_textformat_new(gfx_textformat_t* this, gfx_textformat_props_t);
void gfx_textformat_set_family(gfx_textformat_t*, const wchar_t*);
void gfx_textformat_free(gfx_textformat_t*);

void gfx_font_load(window_t*, const wchar_t* path);

void gfx_bitmap_new(bitmap_t*, window_t*, const wchar_t* path);
void gfx_bitmap_free(bitmap_t*);

void window_free(window_t*);
bool window_pooling();

#include <sdk/window/win32.inl.h>
#include <sdk/window/inl.h>

#endif