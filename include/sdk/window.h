#ifndef SDK_WINDOW_H
#define SDK_WINDOW_H

#include <sdk/types.h>
#include <sdk/math/geometry_2d.h>
#include <sdk/events.h>

#define WINDOW_POS_CENTERED MAX_U64

typedef struct application_t application_t;

typedef struct window_t window_t;

typedef enum window_flags_t {
  WINDOW_HIDDEN = 1,
  WINDOW_NO_RESIZABLE = 2,
  WINDOW_NO_MAXIMIZE = 4,
} window_flags_t;

typedef struct window_options_t {
  const char* name;
  i32 x, y;
  u16 width, height;
  u64 flags;
  void* context;
  // events
  listener_t onupdate;
  listener_t onmousemove;
  listener_t onmousedown;
  listener_t onmouseup;
  listener_t ondblclick;
  listener_t onkeydown;
  listener_t onkeyup;
  listener_t onresize;
  listener_t onload;
  listener_t oncreate;
  listener_t onclose;
} window_options_t;

void window_startup(application_t*, window_options_t*);
void window_free(window_t*);
bool window_pooling();

vector2d_t window_get_cursor(window_t*);
void window_set_size(window_t*, u32 width, u32 height);
bool window_set_viewport(window_t* this, u32 width, u32 height);

#include <sdk/application.h>

#include <sdk/window/win32.inl.h>

#endif