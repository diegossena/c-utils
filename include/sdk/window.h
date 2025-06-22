#ifndef WINDOW_H
#define WINDOW_H

#include <sdk/types.h>
#include <sdk/error.h>
#include <sdk/keyboard.h>
#include <sdk/console.h>
#include <sdk/thread.h>
#include <sdk/fs.h>

#ifndef ATLAS_WIDTH
#define ATLAS_WIDTH 256.f
#endif
#ifndef ATLAS_HEIGHT
#define ATLAS_HEIGHT 256.f
#endif

#define QUAD_VERTEX_COUNT 4
#define QUAD_INDEX_COUNT 6

#define vertices_push(vertex) _vertices_virtual[_vertices_length++] = vertex
#define indexes_push(index) _indexes_virtual[_indexes_length++] = index

typedef struct vertex_t {
  f32 x, y; // position
  union {
    f32 texcoord[2]; // textcoord
    f32 color[4]; // color
  };
} vertex_t;
typedef enum mouse_btn_t {
  MOUSE_BUTTON_LEFT,
  MOUSE_BUTTON_RIGHT,
  MOUSE_BUTTON_MIDDLE,
  MOUSE_BUTTON_AUX
} mouse_btn_t;

void window_startup(const char* title, const char* atlas_path, u16 atlas_width, u16 atlas_height);
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
inline bool window_key_pressed(key_t);

void vertices_reserve(u64 vertices_size, u64 indexes_size);

extern void window_dblclick();
extern void window_onmousemove();
extern void window_onmouseup(mouse_btn_t button);
extern void window_onmousedown(mouse_btn_t button);
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
extern void window_onkeydown(key_t key);
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
extern void window_onkeyup(key_t key);

void _window_resize();
void _window_render();
void _gfx_inicialize(const char* atlas_path, u16 atlas_width, u16 atlas_height);
void _gfx_destroy();

extern u16 window_width; // externally-defined function
extern u16 window_height; // externally-defined function
extern rgba_t window_background;

const f32 atlas_ndc_x;
const f32 atlas_ndc_y;

u16 mouse_x;
u16 mouse_y;

f32 window_deltatime;
bool window_resized;

bool window_focus;

f32 window_ndc_x;
f32 window_ndc_y;

u8 _keyboard_count;
u8 _keyboard_state[32];

u64 vertices_capacity;
vertex_t* _vertices_virtual;
u64 _vertices_length;

u64 indexes_capacity;
u32* _indexes_virtual;
u64 _indexes_length;

f64 _render_time;

#endif