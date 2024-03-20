#pragma once
#include <sdk/window.h>
#include <sdk/platform.h>
#include <sdk/window/gfx/directdraw/IUnknown.win32.h>
#include <sdk/window/gfx/directdraw/FontFileEnumerator.win32.h>

typedef struct SDK_FontCollectionLoader {
  IDWriteFontCollectionLoaderVtbl* lpVtbl;
  SDK_FontFileEnumerator __enumerator;
} SDK_FontCollectionLoader;
HRESULT STDMETHODCALLTYPE FontCollectionLoader_QueryInterface(SDK_FontCollectionLoader* This, const IID* const riid, void** ppvObject) {
  return IUnknown_QueryInterface((SDK_IUnknown*)This, &IID_IDWriteFontCollectionLoader, riid, ppvObject);
}
HRESULT STDMETHODCALLTYPE FontCollectionLoader_CreateEnumeratorFromKey(
  SDK_FontCollectionLoader* This, IDWriteFactory* factory, const void* key,
  UINT32 key_size, IDWriteFontFileEnumerator** enumerator
) {
  *enumerator = (IDWriteFontFileEnumerator*)&This->__enumerator;
  return S_OK;
}
static const IDWriteFontCollectionLoaderVtbl g_FontCollectionLoaderVtbl = {
  (void*)FontCollectionLoader_QueryInterface,
  (void*)IUnknown_AddRef,
  (void*)IUnknown_Release,
  (void*)FontCollectionLoader_CreateEnumeratorFromKey
};
void FontCollectionLoader_Inicialize(SDK_FontCollectionLoader* This, IDWriteFontFile* font_file) {
  This->lpVtbl = (IDWriteFontCollectionLoaderVtbl*)&g_FontCollectionLoaderVtbl;
  FontFileEnumerator_Inicialize(&This->__enumerator, font_file);
}