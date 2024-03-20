#pragma once
#include <sdk/platform.h>
#include <sdk/memory.h>

typedef struct SDK_IUnknown {
  void* lpVtbl;
  ULONG __ref_count;
} SDK_IUnknown;

ULONG STDMETHODCALLTYPE IUnknown_AddRef(SDK_IUnknown* This) {
  InterlockedIncrement((LONG*)&This->__ref_count);
  console_log("IUnknown_AddRef=%d", This->__ref_count);
  return This->__ref_count;
}
HRESULT STDMETHODCALLTYPE IUnknown_QueryInterface(SDK_IUnknown* This, REFIID object_riid, REFIID riid, void** ppvObject) {
  console_log("IUnknown_QueryInterface=%d", &This->__ref_count);
  if (IsEqualIID(riid, &IID_IUnknown) || IsEqualIID(riid, object_riid)) {
    *ppvObject = This;
    IUnknown_AddRef(This);
    return S_OK;
  } else {
    *ppvObject = NULL;
    return E_NOINTERFACE;
  }
}
ULONG STDMETHODCALLTYPE IUnknown_Release(SDK_IUnknown* This) {
  ULONG newCount = InterlockedDecrement(&This->__ref_count);
  console_log("IUnknown_Release=%d", newCount);
  // if (newCount == 0) {
  //   memory_free(This);
  // }
  return newCount;
}