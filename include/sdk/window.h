#ifndef SDK_WINDOW_H
#define SDK_WINDOW_H

#include <sdk/types.h>
#include <sdk/error.h>
#include <sdk/keyboard.h>

extern bool __global_window_running;
extern bool __global_window_focus;
extern u8 __global_window_keyboard_count;
extern u8 __global_window_keyboard_state[32];

// METHODS

SDK_EXPORT void window_startup(
  const char* title,
  i32 width, i32 height
);
SDK_EXPORT void window_run();
SDK_EXPORT void window_clear();
SDK_EXPORT void window_close();

// DRAW

SDK_EXPORT void window_redraw();
SDK_EXPORT void window_fill_rectangle(
  f32 left, f32 top, f32 right, f32 bottom,
  f32 r, f32 g, f32 b, f32 a
);

// EVENTS

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
extern void window_onkeydown(key_code_t);
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

// KEYBOARD

SDK_EXPORT bool window_key_pressed(key_code_t);
SDK_EXPORT void __window_key_press(key_code_t);
SDK_EXPORT void __window_key_release(key_code_t);

// GETTERS

SDK_EXPORT bool window_focused();

#endif