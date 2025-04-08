#ifndef GFX_IMAGE_H
#define GFX_IMAGE_H

#include <sdk/types.h>

typedef enum img_extend_t {
  BITMAP_EXTEND_REPEAT,
  BITMAP_EXTEND_NO_REPEAT,
  BITMAP_EXTEND_COVER
} img_extend_t;

typedef struct gfx_image_t {
  f32 rect[4]; // [left, top, right, bottom]
  // optional
  img_extend_t extend_mode;

  void* src;
  f32 src_rect[4]; // [left, top, right, bottom]
  u16 src_width, src_height;
} gfx_image_t;

export void gfx_image_src(gfx_image_t* this, const wchar_t* path);
export void gfx_image_draw(const gfx_image_t* this);
export void gfx_image_free(gfx_image_t* this);

#endif