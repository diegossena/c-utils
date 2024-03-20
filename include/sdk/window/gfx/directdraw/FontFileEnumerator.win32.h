#pragma once
#include <sdk/window.h>
#include <sdk/platform.h>
#include <sdk/memory.h>
#include <sdk/window/gfx/directdraw/IUnknown.win32.h>

typedef struct SDK_FontFileEnumerator {
  IDWriteFontFileEnumeratorVtbl* lpVtbl;
  IDWriteFontFile* __font_file;
} SDK_FontFileEnumerator;

HRESULT STDMETHODCALLTYPE FontFileEnumerator_QueryInterface(SDK_FontFileEnumerator* This, REFIID riid, void** ppvObject) {
  return IUnknown_QueryInterface((SDK_IUnknown*)This, &IID_IDWriteFontFileEnumerator, riid, ppvObject);
}
HRESULT STDMETHODCALLTYPE FontFileEnumerator_MoveNext(SDK_FontFileEnumerator* This, WINBOOL* has_current_file) {
  *has_current_file = !!This->__font_file;
  return S_OK;
}
HRESULT STDMETHODCALLTYPE FontFileEnumerator_GetCurrentFontFile(
  SDK_FontFileEnumerator* This, IDWriteFontFile** font_file
) {
  *font_file = This->__font_file;
  This->__font_file = 0;
  return S_OK;
}
static const IDWriteFontFileEnumeratorVtbl g_FontFileEnumeratorVtbl = {
  (void*)FontFileEnumerator_QueryInterface,
  (void*)IUnknown_AddRef,
  (void*)IUnknown_Release,
  (void*)FontFileEnumerator_MoveNext,
  (void*)FontFileEnumerator_GetCurrentFontFile
};

void FontFileEnumerator_Inicialize(
  SDK_FontFileEnumerator* This,
  IDWriteFontFile* font_file
) {
  This->lpVtbl = (IDWriteFontFileEnumeratorVtbl*)&g_FontFileEnumeratorVtbl;
  This->__font_file = font_file;
}