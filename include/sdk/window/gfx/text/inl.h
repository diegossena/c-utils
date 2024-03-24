#include <sdk/window/gfx/text.h>

void gfx_text_new(gfx_text_t* this) {
  wstring_new(&this->text);
}
void gfx_text_free(gfx_text_t* this) {
  wstring_free(&this->text);
}