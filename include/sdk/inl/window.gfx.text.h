#ifdef SDK_WINDOW_GFX_TEXT_H

#include <sdk/assert.h>

void gfx_text_free(gfx_text_t* this) {
  wstring_free(&this->text);
  emitter_off(&this->__ondestroy);
}
void gfx_text_new(gfx_text_t* this) {
  window_t* window = this->window;
  assert(window);
  wstring_new(&this->text);
  // register
  this->__ondestroy = (event_listener_t) {
    .callback = (listener_t)gfx_stroke_free,
    .context = this
  };
  emitter_on(&window->onclose, &this->__ondestroy);
}

#endif