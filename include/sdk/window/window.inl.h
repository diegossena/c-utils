#include <sdk/window/window.h>

void* window_get_context(window_t* this) { return this->context; }

void rect_set_width(gfx_rect_t* this, f32 width) {
  this->right = this->left + width;
}
void rect_set_height(gfx_rect_t* this, f32 height) {
  this->bottom = this->top + height;
}
void rect_set_size(gfx_rect_t* this, f32 size) {
  rect_set_width(this, size);
  rect_set_height(this, size);
}