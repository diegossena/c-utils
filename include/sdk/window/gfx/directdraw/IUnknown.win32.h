#pragma once
#include <sdk/platform.h>
#include <sdk/memory.h>

typedef struct SDK_IUnknown {
  void* lpVtbl;
} SDK_IUnknown;

ULONG STDMETHODCALLTYPE IUnknown_AddRef(SDK_IUnknown* This) {
  return 0;
}
HRESULT STDMETHODCALLTYPE IUnknown_QueryInterface(SDK_IUnknown* This, REFIID object_riid, REFIID riid, void** ppvObject) {
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
  return 0;
}