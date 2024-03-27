#ifndef SDK_WINDOW_H
#define SDK_WINDOW_H

#include <sdk/application.h>
#include <sdk/math/geometry_2d.h>
#include <sdk/events.h>
#include <sdk/time.h>

#define WINDOW_POS_CENTERED MAX_U64

typedef struct window_t window_t;

typedef enum window_flags_t {
  WINDOW_FLAG_HIDDEN = 1,
  WINDOW_FLAG_NO_RESIZABLE = 2,
  WINDOW_FLAG_NO_MAXIMIZE = 4,
} window_flags_t;

typedef void (*window_load_listener_t)(window_t* context);

typedef struct window_props_t {
  const char* name;
  i32 x, y;
  u16 width, height;
  u64 flags;
  void* context;
  // events
  window_load_listener_t onpreload;
  window_load_listener_t onload;
} window_props_t;

static bool __window_running = false;

#endif