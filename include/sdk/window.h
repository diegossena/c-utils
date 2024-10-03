#ifndef SDK_WINDOW_H
#define SDK_WINDOW_H

#include <sdk/types.h>

extern bool _sdk_window_running;

void window_startup(
  const char* title,
  i32 width, i32 height
);
void window_run();
void window_redraw();
/** @brief Externally-defined function
 */
extern void window_onupdate(u32 time);

#endif