#ifndef WINDOW_H
#define WINDOW_H

#include <sdk/types.h>
#include <sdk/error.h>
#include <sdk/keyboard.h>
#include <sdk/console.h>
#include <sdk/thread.h>

#define vertices_push(vertex) vertices_virtual[vertices_length++] = vertex
#define indexes_push(index) indexes_virtual[indexes_length++] = index

typedef struct vertex_t {
  f32 x, y; // position
  union {
    f32 textcoord[2]; // textcoord
    f32 color[4]; // color
  };
} vertex_t;
typedef enum mouse_btn_t {
  MOUSE_BUTTON_LEFT,
  MOUSE_BUTTON_RIGHT,
  MOUSE_BUTTON_MIDDLE,
  MOUSE_BUTTON_AUX
} mouse_btn_t;

export void window_startup(const char* title, const char* atlas_path);
export void window_set_title(const char* title);
export void window_close();
extern void window_run();
extern void window_rect_draw(
  f32 x0, f32 y0, f32 x1, f32 y1,
  f32 u0, f32 v0, f32 u1, f32 v1
);
extern void window_rect_fill(
  f32 x0, f32 y0, f32 x1, f32 y1,
  f32 r, f32 g, f32 b, f32 a
);
extern void window_dblclick();
extern void window_onmousemove();
extern void window_onmouseup(i32 x, i32 y, mouse_btn_t);
extern void window_onmousedown(i32 x, i32 y, mouse_btn_t);
extern void window_onscroll(i32 delta);
extern void window_onresize();
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
extern void window_onkeydown(key_t);
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
export bool window_key_pressed(key_t);

export void vertices_reserve(u64 size);
export void indexes_reserve(u64 size);

extern f32 window_deltatime;
/**
 * - Externally-defined function
 */
extern u16 window_width;
/**
 * - Externally-defined function
 */
extern u16 window_height;
extern bool window_focus;
extern f32 window_background[4];
extern bool window_updated;
extern i32 mouse_x;
extern i32 mouse_y;

extern f32 window_pixel_ndc[2];

extern u8 keyboard_count;
extern u8 keyboard_state[32];

extern vertex_t* vertices_virtual;
extern u64 vertices_length;
extern u64 vertices_capacity;

extern u32* indexes_virtual;
extern u64 indexes_length;
extern u64 indexes_capacity;

extern const u16 atlas_width;
extern const u16 atlas_height;


#endif