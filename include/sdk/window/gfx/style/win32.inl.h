#include <sdk/window.h>
#include <sdk/platform.h>
#if PLATFORM_WINDOWS

#include <sdk/assert.h>

typedef struct gfx_style_t {
  IDWriteTextFormat* __format;
} gfx_style_t;

#include <sdk/window/gfx/style.h>

void gfx_style_new(gfx_style_t* this, style_props_t props) {
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
  ++memory_leaks;
}

void gfx_style_free(gfx_style_t* this) {
  IDWriteTextFormat_Release(this->__format);
  --memory_leaks;
}

#endif