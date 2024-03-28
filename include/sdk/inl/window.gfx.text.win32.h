#ifdef SDK_WINDOW_GFX_TEXT_H
#include <sdk/platform.h>
#ifdef PLATFORM_WINDOWS

#include <sdk/window/win32.h>
#include <sdk/assert.h>
#include <sdk/leaks.h>

typedef struct SDK_IUnknown {
  void* lpVtbl;
} SDK_IUnknown;
typedef struct __FontFileEnumerator {
  IDWriteFontFileEnumeratorVtbl* lpVtbl;
  queue_t* queue; // __font_queue_t
  queue_t* it;
} __FontFileEnumerator;
typedef struct __FontCollectionLoader {
  IDWriteFontCollectionLoaderVtbl* lpVtbl;
  __FontFileEnumerator __enumerator;
} __FontCollectionLoader;

typedef struct __font_queue_t {
  queue_t queue;
  IDWriteFontFile* file;
} __font_queue_t;

typedef struct gfx_text_style_t {
  IDWriteTextFormat* __format;
} gfx_text_style_t;

void gfx_font_load(window_t* this, const wchar_t* path) {
  __font_queue_t* item = (__font_queue_t*)memory_alloc(sizeof(__font_queue_t));
  this->__d2d_write_factory->lpVtbl->CreateFontFileReference(
    this->__d2d_write_factory, path, 0, &item->file
  );
  queue_push(&this->__fonts, (queue_t*)item);
}

void gfx_text_style_free(gfx_text_style_t* this) {
  assert(this->__format);
  IDWriteTextFormat_Release(this->__format);
  __leaks_memory_decrement();
}
void gfx_text_style_new(gfx_text_style_t* this, text_style_props_t props) {
  assert(props.window);
  assert(props.size > 0);
  assert(props.family);
  assert(props.weight);
  window_t* window = props.window;
  IDWriteFactory* factory = window->__d2d_write_factory;
  IDWriteFontCollection* collection = window->__collection;
  IDWriteFactory_CreateTextFormat(
    factory, props.family, collection, props.weight,
    props.style, DWRITE_FONT_STRETCH_NORMAL, props.size, L"",
    &this->__format
  );
  __leaks_memory_increment();
}
void gfx_text_adjust(gfx_text_t* this) {
  IDWriteTextFormat* text_format = this->style->__format;
  IDWriteFactory* factory = this->window->__d2d_write_factory;
  IDWriteTextLayout* text_layout;
  DWRITE_TEXT_METRICS metrics;
  IDWriteFactory_CreateTextLayout(
    factory, this->text.__data, this->text.__length, text_format, MAX_F32, MAX_F32,
    &text_layout
  );
  IDWriteTextLayout_GetMetrics(text_layout, &metrics);
  IDWriteTextLayout_Release(text_layout);
  this->rect.right_bottom.x = this->rect.left_top.x + metrics.width;
  this->rect.right_bottom.y = this->rect.left_top.y + metrics.height;
}

void gfx_text_draw(const gfx_text_t* this) {
  ID2D1RenderTarget* render_target = this->window->__d2d_render_target;
  IDWriteTextFormat* text_format = this->style->__format;
  ID2D1RenderTarget_DrawText(
    render_target, this->text.__data, this->text.__length, this->style->__format,
    (D2D1_RECT_F*)&this->rect, (ID2D1Brush*)this->color->__brush,
    D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL
  );
}

ULONG STDMETHODCALLTYPE __IUnknown_AddRef(SDK_IUnknown* This) {
  return 0;
}
HRESULT STDMETHODCALLTYPE __IUnknown_QueryInterface(SDK_IUnknown* This, REFIID object_riid, REFIID riid, void** ppvObject) {
  if (IsEqualIID(riid, &IID_IUnknown) || IsEqualIID(riid, object_riid)) {
    *ppvObject = This;
    __IUnknown_AddRef(This);
    return S_OK;
  } else {
    *ppvObject = NULL;
    return E_NOINTERFACE;
  }
}
ULONG STDMETHODCALLTYPE __IUnknown_Release(SDK_IUnknown* This) {
  return 0;
}

HRESULT STDMETHODCALLTYPE __FontFileEnumerator_QueryInterface(__FontFileEnumerator* This, REFIID riid, void** ppvObject) {
  return __IUnknown_QueryInterface((SDK_IUnknown*)This, &IID_IDWriteFontFileEnumerator, riid, ppvObject);
}
HRESULT STDMETHODCALLTYPE __FontFileEnumerator_MoveNext(__FontFileEnumerator* This, WINBOOL* has_current_file) {
  This->it = This->it->next;
  *has_current_file = This->it != This->queue;
  return S_OK;
}
HRESULT STDMETHODCALLTYPE __FontFileEnumerator_GetCurrentFontFile(
  __FontFileEnumerator* This, IDWriteFontFile** font_file
) {
  *font_file = ((__font_queue_t*)This->it)->file;
  return S_OK;
}
static const IDWriteFontFileEnumeratorVtbl g_FontFileEnumeratorVtbl = {
  (void*)__FontFileEnumerator_QueryInterface,
  (void*)__IUnknown_AddRef,
  (void*)__IUnknown_Release,
  (void*)__FontFileEnumerator_MoveNext,
  (void*)__FontFileEnumerator_GetCurrentFontFile
};

void FontFileEnumerator_Inicialize(
  __FontFileEnumerator* This,
  queue_t* queue
) {
  This->lpVtbl = (IDWriteFontFileEnumeratorVtbl*)&g_FontFileEnumeratorVtbl;
  This->queue = queue;
  This->it = queue;
}

HRESULT STDMETHODCALLTYPE __FontCollectionLoader_QueryInterface(__FontCollectionLoader* This, const IID* const riid, void** ppvObject) {
  return __IUnknown_QueryInterface((SDK_IUnknown*)This, &IID_IDWriteFontCollectionLoader, riid, ppvObject);
}
HRESULT STDMETHODCALLTYPE __FontCollectionLoader_CreateEnumeratorFromKey(
  __FontCollectionLoader* This, IDWriteFactory* factory, const void* key,
  UINT32 key_size, IDWriteFontFileEnumerator** enumerator
) {
  *enumerator = (IDWriteFontFileEnumerator*)&This->__enumerator;
  return S_OK;
}
static const IDWriteFontCollectionLoaderVtbl g_FontCollectionLoaderVtbl = {
  (void*)__FontCollectionLoader_QueryInterface,
  (void*)__IUnknown_AddRef,
  (void*)__IUnknown_Release,
  (void*)__FontCollectionLoader_CreateEnumeratorFromKey
};
void __FontCollectionLoader_Inicialize(__FontCollectionLoader* This, queue_t* queue) {
  This->lpVtbl = (IDWriteFontCollectionLoaderVtbl*)&g_FontCollectionLoaderVtbl;
  FontFileEnumerator_Inicialize(&This->__enumerator, queue);
}

#endif
#endif