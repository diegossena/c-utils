#pragma once

#include <sdk/math/geometry_2d.h>
#include <sdk/queue.h>

typedef struct game_t game_t;

typedef struct scene_t {
  void (*onupdate)(game_t*);
  void (*onmousemove)(game_t*, vector2d_t cursor);
  void (*onmouseup)(game_t*, vector2d_t cursor);
  void (*onmousedown)(game_t*, vector2d_t cursor);
} scene_t;