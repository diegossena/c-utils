#ifndef GFX_IMAGE_H
#define GFX_IMAGE_H

#include <sdk/types.h>

typedef struct gfx_image_t {
  void* src;
  u16 width, height;
} gfx_image_t;

export void gfx_image_src(gfx_image_t* this, const wchar_t* path);
export void gfx_image_draw(const gfx_image_t* this, const f32 rect[4], const f32 src_rect[4], f32 opacity);
export void gfx_image_repeat(const gfx_image_t* this, const f32 rect[4], const f32 src_rect[4]);
export void gfx_image_free(gfx_image_t* this);

#endif