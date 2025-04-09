#ifndef WINDOW_H
#define WINDOW_H

#include <sdk/types.h>
#include <sdk/error.h>
#include <sdk/keyboard.h>
#include <sdk/console.h>
#include <sdk/thread.h>
#include <sdk/gfx.text.h>
#include <sdk/gfx.color.h>

// METHODS

export void window_startup();
export void window_shutdown();
export void window_set_title(const char* title);
export void window_clear();
export void window_close();

// DRAW

export void window_redraw();

// EVENTS

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

export bool window_key_pressed(key_code_t);

void __window_thread();

extern bool global_window_focus;
extern const u16 global_window_width;
extern const u16 global_window_height;
extern sync_t* global_window_onload_sync;
extern u8 global_window_keyboard_count;
extern u8 global_window_keyboard_state[32];
extern thread_t* global_window_thread;

#endif