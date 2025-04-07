#include <sdk/gfx.color.h>
#ifdef PLATFORM_WINDOWS
#include <sdk/window.win32.h>

export gfx_color_t* gfx_color_new(f32 r, f32 g, f32 b, f32 a) {
  D2D1_COLOR_F color = { r,g,b,a };
  // D2D1_BRUSH_PROPERTIES brush_properties;
  ID2D1SolidColorBrush* brush;
  ID2D1RenderTarget_CreateSolidColorBrush(
    global_d2d_render_target, (D2D1_COLOR_F*)&color, null, &brush
  );
  return brush;
}
export void gfx_color_free(gfx_color_t* this) {
  assert(this);
  ID2D1SolidColorBrush_Release((ID2D1SolidColorBrush*)this);
}

#endif