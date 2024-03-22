#pragma once
#include <sdk/window.h>
#include <sdk/platform.h>
#include <sdk/window/gfx/directdraw/IUnknown.win32.h>
#include <sdk/window/gfx/directdraw/FontFileEnumerator.win32.h>

typedef struct __FontCollectionLoader {
  IDWriteFontCollectionLoaderVtbl* lpVtbl;
  __FontFileEnumerator __enumerator;
} __FontCollectionLoader;
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