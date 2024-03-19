#include <sdk/window.h>

void* window_get_context(window_t* this) { return this->context; }
void window_set_context(window_t* this, void* context) {
  this->context = context;
}