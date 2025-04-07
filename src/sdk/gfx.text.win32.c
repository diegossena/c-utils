#include <sdk/gfx.text.h>
#ifdef PLATFORM_WINDOWS

#include <sdk/window.win32.h>

typedef struct FontFileEnumerator {
  IDWriteFontFileEnumeratorVtbl* lpVtbl;
  IDWriteFontFile* file;
  IDWriteFontFile** files;
  u64 count;
} FontFileEnumerator;
typedef struct FontCollectionLoader {
  IDWriteFontCollectionLoaderVtbl* lpVtbl;
  FontFileEnumerator enumerator;
} FontCollectionLoader;

ULONG STDMETHODCALLTYPE IUnknown_AddRef(IUnknown* This) { return 0; }
export HRESULT STDMETHODCALLTYPE IUnknown_QueryInterface(IUnknown* This, REFIID object_riid, REFIID riid, void** ppvObject) {
  if (IsEqualIID(riid, &IID_IUnknown) || IsEqualIID(riid, object_riid)) {
    *ppvObject = This;
    IUnknown_AddRef(This);
    return S_OK;
  } else {
    *ppvObject = NULL;
    return E_NOINTERFACE;
  }
}
ULONG STDMETHODCALLTYPE IUnknown_Release(IUnknown* this) { return 0; }
export HRESULT STDMETHODCALLTYPE FontFileEnumerator_QueryInterface(FontFileEnumerator* This, REFIID riid, void** ppvObject) {
  return IUnknown_QueryInterface((IUnknown*)This, &IID_IDWriteFontFileEnumerator, riid, ppvObject);
}
HRESULT STDMETHODCALLTYPE FontFileEnumerator_MoveNext(FontFileEnumerator* this, WINBOOL* has_current_file) {
  this->file = 0;
  while (this->count > 0 && this->file == 0) {
    this->file = *this->files++;
    --this->count;
  }
  *has_current_file = this->file != 0;
  return S_OK;
}
HRESULT STDMETHODCALLTYPE FontFileEnumerator_GetCurrentFontFile(
  FontFileEnumerator* this, IDWriteFontFile** font_file
) {
  *font_file = this->file;
  return S_OK;
}
HRESULT STDMETHODCALLTYPE FontCollectionLoader_QueryInterface(FontCollectionLoader* This, const IID* const riid, void** ppvObject) {
  return IUnknown_QueryInterface((IUnknown*)This, &IID_IDWriteFontCollectionLoader, riid, ppvObject);
}
HRESULT STDMETHODCALLTYPE FontCollectionLoader_CreateEnumeratorFromKey(
  FontCollectionLoader* This, IDWriteFactory* factory, const void* key,
  UINT32 key_size, IDWriteFontFileEnumerator** enumerator
) {
  *enumerator = (IDWriteFontFileEnumerator*)&This->enumerator;
  return S_OK;
}

export void gfx_text_update(gfx_text_t* this) {
  IDWriteTextFormat* text_format = (IDWriteTextFormat*)this->style;
  IDWriteTextLayout* text_layout;
  DWRITE_TEXT_METRICS metrics;
  u8 text_length = wstring_length(this->text);
  IDWriteFactory_CreateTextLayout(
    global_dwrite_factory, this->text, text_length, text_format, MAX_F32, MAX_F32,
    &text_layout
  );
  IDWriteTextLayout_GetMetrics(text_layout, &metrics);
  IDWriteTextLayout_Release(text_layout);
  this->rect[2] = this->rect[0] + metrics.width;
  this->rect[3] = this->rect[1] + metrics.height;
}

export void gfx_font_load(const wchar_t** paths, u64 count) {
  IDWriteFontFile* files[TINY_SIZE];
  // CreateFontFileReference
  for (u8 i = 0; i < count;i++) {
    const wchar_t* path = paths[i];
    IDWriteFontFile* file;
    IDWriteFactory_CreateFontFileReference(
      global_dwrite_factory, path, 0, &file
    );
    files[i] = file;
  }
  // FontCollectionLoader_Inicialize
  FontCollectionLoader loader;
  const IDWriteFontCollectionLoaderVtbl g_FontCollectionLoaderVtbl = {
    (void*)FontCollectionLoader_QueryInterface,
    (void*)IUnknown_AddRef,
    (void*)IUnknown_Release,
    (void*)FontCollectionLoader_CreateEnumeratorFromKey
  };
  loader.lpVtbl = (IDWriteFontCollectionLoaderVtbl*)&g_FontCollectionLoaderVtbl;
  // FontFileEnumerator_Inicialize
  const IDWriteFontFileEnumeratorVtbl g_FontFileEnumeratorVtbl = {
    (void*)FontFileEnumerator_QueryInterface,
    (void*)IUnknown_AddRef,
    (void*)IUnknown_Release,
    (void*)FontFileEnumerator_MoveNext,
    (void*)FontFileEnumerator_GetCurrentFontFile
  };
  loader.enumerator.lpVtbl = (IDWriteFontFileEnumeratorVtbl*)&g_FontFileEnumeratorVtbl;
  loader.enumerator.files = (IDWriteFontFile**)files;
  loader.enumerator.count = count;
  // RegisterFontCollectionLoader 
  global_dwrite_factory->lpVtbl->RegisterFontCollectionLoader(
    global_dwrite_factory, (IDWriteFontCollectionLoader*)&loader
  );
  // CreateCustomFontCollection 
  global_dwrite_factory->lpVtbl->CreateCustomFontCollection(
    global_dwrite_factory, (IDWriteFontCollectionLoader*)&loader,
    0, 0, &global_dwrite_collection
  );
  // UnregisterFontCollectionLoader
  global_dwrite_factory->lpVtbl->UnregisterFontCollectionLoader(
    global_dwrite_factory, (IDWriteFontCollectionLoader*)&loader
  );
}

export gfx_text_style_t* gfx_text_style_new(const wchar_t* family, f32 size, font_weight_t weight, font_style_t style) {
  assert(global_window_thread);
  assert(size > 0);
  assert(family);
  assert(weight);
  IDWriteTextFormat* this;
  IDWriteFactory_CreateTextFormat(
    global_dwrite_factory, family, global_dwrite_collection, weight,
    style, DWRITE_FONT_STRETCH_NORMAL, size, L"",
    &this
  );
  return this;
}
export void gfx_text_style_free(gfx_text_style_t* this) {
  assert(this);
  IDWriteTextFormat* format = this;
  IDWriteTextFormat_Release(format);
}

export void gfx_text_draw(const gfx_text_t* this) {
  u64 text_length = wstring_length(this->text);
  ID2D1RenderTarget_DrawText(
    global_d2d_render_target, this->text, text_length, (IDWriteTextFormat*)this->style,
    (D2D1_RECT_F*)&this->rect, (ID2D1Brush*)this->color,
    D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL
  );
}

#endif