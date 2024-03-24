#include <sdk/window.h>
#include <sdk/platform.h>

#ifdef PLATFORM_WINDOWS

#include <sdk/window/gfx/font.h>

typedef struct __font_queue_t {
  queue_t queue;
  IDWriteFontFile* file;
} __font_queue_t;

void gfx_font_load(window_t* this, const wchar_t* path) {
  __font_queue_t* item = (__font_queue_t*)memory_alloc(sizeof(__font_queue_t));
  this->__d2d_write_factory->lpVtbl->CreateFontFileReference(
    this->__d2d_write_factory, path, 0, &item->file
  );
  queue_push(&this->__fonts, (queue_t*)item);
}

#endif