#pragma once
#include <sdk/window.h>
#include <sdk/platform.h>
#include <sdk/memory.h>
#include <sdk/window/gfx/directdraw/IUnknown.win32.h>
#include <sdk/window/gfx/font.h>

typedef struct __FontFileEnumerator {
  IDWriteFontFileEnumeratorVtbl* lpVtbl;
  queue_t* queue; // __font_queue_t
  queue_t* it;
} __FontFileEnumerator;

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