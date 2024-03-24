#include <sdk/window/gfx/text.h>

void gfx_text_new(gfx_text_t* this) {
  assert(this->window);
  assert(this->color);
  assert(this->style);
  window_t* window = this->window;
  wstring_new(&this->text);
  // register
  this->__ondraw = (event_listener_t) {
    .callback = (listener_t)__gfx_text_draw,
    .context = this
  };
  emitter_on(&window->ondraw, &this->__ondraw);
  this->__ondestroy = (event_listener_t) {
    .callback = (listener_t)gfx_text_free,
    .context = this
  };
  emitter_on(&window->onclose, &this->__ondestroy);
}
void gfx_text_free(gfx_text_t* this) {
  emitter_off(&this->__ondestroy);
  emitter_off(&this->__ondraw);
  wstring_free(&this->text);
}