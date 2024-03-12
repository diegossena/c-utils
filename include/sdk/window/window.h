#pragma once

#include <sdk/types.h>

#define  WINDOW_POS_CENTERED MAX_U64

typedef struct window_t window_t;

typedef void (*window_event_cb)(window_t* this);
typedef void (*update_event_cb)(window_t* this);
typedef void (*keyboard_event_cb)(window_t* this);
typedef void (*ui_event_cb)(window_t* this);
typedef void (*mouse_event_cb)(window_t* this);

typedef enum window_flags {
  WINDOW_HIDDEN = 1,
  WINDOW_BORDELESS = 2,
  WINDOW_RESIZABLE = 4
} window_flags;

typedef struct window_opt {
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
} window_opt;

void window_startup(application_t*, window_opt*);
// getters
void* window_get_context(window_t*);
u16 window_get_width(window_t*);
u16 window_get_height(window_t*);
// application handlers
void window_free(window_t*);
void window_pooling();

#include <sdk/window/window.win32.inl.h>
#include <sdk/window/window.inl.h>