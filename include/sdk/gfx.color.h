#ifndef WINDOW_COLOR_H
#define WINDOW_COLOR_H

#include <sdk/types.h>
#include <sdk/window.h>

typedef void gfx_color_t;

export gfx_color_t* gfx_color_new(f32 r, f32 g, f32 b, f32 a);
export void gfx_color_free(gfx_color_t* this);


#endif