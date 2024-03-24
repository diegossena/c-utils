#include <sdk/window.h>
#include <sdk/platform.h>

#if PLATFORM_WINDOWS

#include <sdk/window/gfx/color.h>
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
  ++memory_leaks;
}
void gfx_color_free(gfx_color_t* this) {
  ID2D1SolidColorBrush_Release(this->__brush);
  --memory_leaks;
}

#endif