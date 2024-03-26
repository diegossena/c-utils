#ifdef SDK_WINDOW_GFX_TEXT_H

#include <sdk/assert.h>

void gfx_text_new(gfx_text_t* this) {
  window_t* window = this->window;
  assert(window);
  wstring_new(&this->text);
}
void gfx_text_free(gfx_text_t* this) {
  wstring_free(&this->text);
}

#endif