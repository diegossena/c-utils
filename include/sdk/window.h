#ifndef WINDOW_H
#define WINDOW_H

#include <sdk/types.h>
#include <sdk/error.h>
#include <sdk/keyboard.h>
#include <sdk/console.h>
#include <sdk/thread.h>

#define QUAD_VERTEX_COUNT 4
#define QUAD_INDEX_COUNT 6

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

void window_startup(const char* title, const char* atlas_path);
void window_set_title(const char* title);
void window_close();
void window_run();
void window_rect_draw(
  f32 x0, f32 y0, f32 x1, f32 y1,
  f32 u0, f32 v0, f32 u1, f32 v1
);
void window_rect_fill(
  f32 x0, f32 y0, f32 x1, f32 y1,
  f32 r, f32 g, f32 b, f32 a
);
bool window_key_pressed(key_t);
void vertices_reserve(u64 size);
void indexes_reserve(u64 size);

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

extern u16 window_width; // externally-defined function
extern u16 window_height; // externally-defined function

extern const u16 atlas_width; // externally-defined function
extern const u16 atlas_height; // externally-defined function
extern const u16 atlas_tile_size; // externally-defined function
extern const u16 atlas_tiles_width;
extern const u16 atlas_tiles_height;

extern f32 window_deltatime;

extern bool window_focus;
extern f32 window_background[4];
extern bool window_updated;
extern i32 mouse_x;
extern i32 mouse_y;

extern f32 window_pixel_ndc_x;
extern f32 window_pixel_ndc_y;

extern u8 keyboard_count;
extern u8 keyboard_state[32];

extern vertex_t* vertices_virtual;
extern u64 vertices_length;
extern u64 vertices_capacity;

extern u32* indexes_virtual;
extern u64 indexes_length;
extern u64 indexes_capacity;

#endif