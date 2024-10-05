#ifndef SDK_WINDOW_H
#define SDK_WINDOW_H

#include <sdk/types.h>
#include <sdk/error.h>
#include <sdk/keyboard.h>

extern bool __global_window_running;
extern bool __global_window_focus;
extern u8 __global_window_keyboard_count;
extern u8 __global_window_keyboard_state[32];

void window_startup(
  const char* title,
  i32 width, i32 height
);
void window_run();
void window_redraw();
void window_fill_rectangle(
  f32 left, f32 top, f32 right, f32 bottom,
  f32 r, f32 g, f32 b, f32 a
);
/**
 * - Event Listener
 * - Externally-defined function
 */
extern void window_onupdate(u32 time);
/**
 * - Event Listener
 * - Externally-defined function
 * The `onrender` event occurs when a draw is requested.
 */
extern void window_onrender();
/**
 * - Event Listener
 * - Externally-defined function
 * The `onkeydown` event occurs when the user presses a key on the keyboard.
 * It repeats if the key is kept pressed after a delay.
 */
extern void window_onkeydown();
/**
 * - Event Listener
 * - Externally-defined function
 * The `onkeypress` event occurs when the user presses a key on the keyboard.
 * It repeats while the user keeps the key depressed.
 */
extern void window_onkeypress();
/**
 * - Event Listener
 * - Externally-defined function
 */
extern void window_onkeyup();

bool window_key_pressed(key_code_t key);
void __window_key_press(key_code_t key);
void __window_key_release(key_code_t key);

#endif