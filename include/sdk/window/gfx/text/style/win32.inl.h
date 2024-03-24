#include <sdk/window.h>
#include <sdk/platform.h>
#ifdef PLATFORM_WINDOWS

#include <sdk/assert.h>

typedef struct gfx_textstyle_t {
  IDWriteTextFormat* __format;
} gfx_textstyle_t;

#include <sdk/window/gfx/text/style.h>

void gfx_textstyle_new(gfx_textstyle_t* this, textstyle_props_t props) {
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

void gfx_textstyle_free(gfx_textstyle_t* this) {
  IDWriteTextFormat_Release(this->__format);
#ifdef SDK_DEVELOPMENT
  --memory_leaks;
#endif
}

#endif