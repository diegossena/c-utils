#ifdef SDK_WINDOW_GFX_COLOR_H
#include <sdk/platform.h>
#ifdef PLATFORM_WINDOWS

#include <sdk/window/win32.h>
#include <sdk/assert.h>

typedef struct gfx_color_t {
  ID2D1SolidColorBrush* __brush;
} gfx_color_t;

void gfx_color_new(gfx_color_t* this, window_t* window, color_t color) {
  assert(window);
  D2D1_BRUSH_PROPERTIES brush_properties;
  ID2D1RenderTarget_CreateSolidColorBrush(
    window->__d2d_render_target, (D2D1_COLOR_F*)&color, null,
    (ID2D1SolidColorBrush**)&this->__brush
  );
#ifdef SDK_DEVELOPMENT
  ++memory_leaks;
#endif
}
void gfx_color_free(gfx_color_t* this) {
  ID2D1SolidColorBrush_Release(this->__brush);
#ifdef SDK_DEVELOPMENT
  --memory_leaks;
#endif
}

#endif
#endif