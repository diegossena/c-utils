#ifndef SDK_WINDOW_H
#define SDK_WINDOW_H

#include <sdk/types.h>
#include <sdk/taskmanager.h>
#ifdef PLATFORM_WINDOWS
#define COBJMACROS 1
#include <initguid.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <windows.h>
#endif

typedef struct window_t {
  void* __handle;
#ifdef PLATFORM_WINDOWS
  ID2D1Factory* __d2d_factory;
  ID2D1HwndRenderTarget* __d2d_render_target;
  IDWriteFactory* __d2d_write_factory;
#endif
} window_t;

void window_constructor(
  window_t* this,
  const char* title,
  i32 width, i32 height
);
void window_redraw(window_t* this);

#endif