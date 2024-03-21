#ifndef SDK_WINDOW_H
#define SDK_WINDOW_H

#include <sdk/types.h>
#include <sdk/math/geometry_2d.h>

#define WINDOW_POS_CENTERED MAX_U64

typedef struct application_t application_t;

typedef struct window_t window_t;

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

void window_startup(application_t*, window_options_t*);
void window_free(window_t*);
bool window_pooling();

u16 window_get_width(window_t*);
u16 window_get_height(window_t*);
vector2d_t window_get_cursor(window_t*);
void window_set_size(window_t*, u32 width, u32 height);
bool window_set_viewport(window_t* this, u32 width, u32 height);

#include <sdk/application.h>

#include <sdk/window/win32.inl.h>

#endif