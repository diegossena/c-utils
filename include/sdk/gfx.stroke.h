#ifndef GFX_STROKE_H
#define GFX_STROKE_H

#include <sdk/types.h>
#include <sdk/window.h>

typedef void gfx_stroke_t;

export gfx_stroke_t* gfx_stroke_new();
export void gfx_stroke_free(gfx_stroke_t* this);


#endif