#ifndef WINDOW_H
#define WINDOW_H

#include <sdk/types.h>
#include <sdk/error.h>
#include <sdk/keyboard.h>
#include <sdk/console.h>
#include <sdk/thread.h>

typedef struct vertex_t {
  f32 x, y; // position
  f32 u, v; // textcoord
} vertex_t;

export void window_startup();
export void window_set_title(const char* title);
export void window_close();
extern void window_run();

extern void window_atlas_load(const char* path, const u64 width, const u64 height);

// DRAW

export void window_draw(const f32 color[4]);

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

export void _window_resize();
export void _window_thread();

extern bool global_window_repaint;
extern bool global_window_resize;

extern f32 global_window_deltatime;
extern thread_t* global_window_thread;

extern bool global_window_focus;
extern u16 global_window_width;
extern u16 global_window_height;
extern u8 global_window_keyboard_count;
extern u8 global_window_keyboard_state[32];

extern vertex_t* global_vertices_virtual;
extern u64 global_vertices_length;
extern u64 global_vertices_capacity;
extern u64 global_vertices_used;

extern u32* global_indexes_virtual;
extern u64 global_indexes_length;
extern u64 global_indexes_capacity;
extern u64 global_indexes_used;

#endif