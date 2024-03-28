#ifdef SDK_WINDOW_GFX_COLOR_H
#include <sdk/platform.h>
#ifdef PLATFORM_WINDOWS

#include <sdk/window/win32.h>
#include <sdk/assert.h>

typedef struct gfx_color_t {
  ID2D1SolidColorBrush* __brush;
  event_listener_t ondestroy;
} gfx_color_t;

void gfx_color_free(gfx_color_t* this) {
  assert(this->__brush);
  ID2D1SolidColorBrush_Release(this->__brush);
  __leaks_memory_decrement();
  emitter_off(&this->ondestroy);
}
void gfx_color_new(gfx_color_t* this, window_t* window, color_t color) {
  assert(window);
  assert(!this->__brush);
  D2D1_BRUSH_PROPERTIES brush_properties;
  ID2D1RenderTarget_CreateSolidColorBrush(
    window->__d2d_render_target, (D2D1_COLOR_F*)&color, null, &this->__brush
  );
  __leaks_memory_increment();
  // register
  this->ondestroy = (event_listener_t) {
    .callback = (listener_t)gfx_color_free,
    .context = this
  };
  emitter_on(&window->onclose, &this->ondestroy);
}

#endif
#endif