#pragma once
#include <sdk/window.h>
#include <sdk/platform.h>
#include <sdk/memory.h>
#include <sdk/window/IUnknown.h>

typedef struct SDK_FontFileEnumerator {
  IDWriteFontFileEnumeratorVtbl* lpVtbl;
  ULONG __ref_count;
  IDWriteFontFile* __font_file;
  u64 __next_index;
} SDK_FontFileEnumerator;

HRESULT STDMETHODCALLTYPE FontFileEnumerator_QueryInterface(SDK_FontFileEnumerator* This, REFIID riid, void** ppvObject) {
  return IUnknown_QueryInterface((SDK_IUnknown*)This, &IID_IDWriteFontFileEnumerator, riid, ppvObject);
}
HRESULT STDMETHODCALLTYPE FontFileEnumerator_MoveNext(SDK_FontFileEnumerator* This, WINBOOL* has_current_file) {
  console_log("FontFileEnumerator_MoveNext=%d", This->__ref_count);
  *has_current_file = This->__next_index == 0;
  ++This->__next_index;
  return S_OK;
}
HRESULT STDMETHODCALLTYPE FontFileEnumerator_GetCurrentFontFile(
  SDK_FontFileEnumerator* This, IDWriteFontFile** font_file
) {
  console_log("FontFileEnumerator_GetCurrentFontFile=%d", This->__ref_count);
  *font_file = This->__font_file;
  return S_OK;
}
static const IDWriteFontFileEnumeratorVtbl g_FontFileEnumeratorVtbl = {
  FontFileEnumerator_QueryInterface,
  IUnknown_AddRef,
  IUnknown_Release,
  FontFileEnumerator_MoveNext,
  FontFileEnumerator_GetCurrentFontFile
};

void FontFileEnumerator_Inicialize(
  SDK_FontFileEnumerator* This,
  IDWriteFontFile* font_file
) {
  console_log("FontFileEnumerator_Inicialize=%d", This->__ref_count);
  This->lpVtbl = &g_FontFileEnumeratorVtbl;
  This->__next_index = 0;
  This->__font_file = font_file;
  return This;
}