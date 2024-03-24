#include <sdk/window.h>
#include <sdk/platform.h>
#ifdef PLATFORM_WINDOWS

#include <sdk/assert.h>

typedef struct gfx_text_style_t {
  IDWriteTextFormat* __format;
} gfx_text_style_t;

#include <sdk/window/gfx/text/style.h>

void gfx_text_style_new(gfx_text_style_t* this, text_style_props_t props) {
  assert(props.window);
  assert(props.size > 0);
  assert(props.family);
  assert(props.weight);
  IDWriteFactory* factory = props.window->__d2d_write_factory;
  IDWriteFontCollection* collection = props.window->__collection;
  IDWriteFactory_CreateTextFormat(
    factory, props.family, collection, props.weight,
    props.style, DWRITE_FONT_STRETCH_NORMAL, props.size, L"",
    &this->__format
  );
#ifdef SDK_DEVELOPMENT
  ++memory_leaks;
#endif
}

void gfx_text_style_free(gfx_text_style_t* this) {
  IDWriteTextFormat_Release(this->__format);
#ifdef SDK_DEVELOPMENT
  --memory_leaks;
#endif
}

#endif