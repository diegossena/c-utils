#pragma once

#include <sdk/types.h>

#define  WINDOW_POS_CENTERED MAX_U64

typedef struct window_t window_t;

typedef void (*window_create_ev)(window_t* this);
typedef void (*window_update_ev)(window_t* this);
typedef void (*window_keyboard_ev)(window_t* this);
typedef void (*window_ui_ev)(window_t* this);
typedef void (*window_mouse_ev)(window_t* this);

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
  window_update_ev onupdate;
  window_mouse_ev onmousemove;
  window_mouse_ev onmousedown;
  window_mouse_ev onmouseup;
  window_keyboard_ev onkeydown;
  window_keyboard_ev onkeyup;
  window_keyboard_ev ondblclick;
  window_ui_ev onresize;
  window_create_ev oncreate;
} window_opt;

void window_inicialize(window_opt*);