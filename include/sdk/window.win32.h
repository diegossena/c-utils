#include <sdk/types.h>

#ifdef PLATFORM_WINDOWS

#include <windowsx.h>
#include <winuser.h>
#define COBJMACROS 1
#include <d3d11.h>
#include <d3dcompiler.h>
#include <initguid.h>
#include <wincodec.h>
#include <d2d1.h>
#include <dwrite.h>
#undef COBJMACROS

extern HWND global_window;
extern ID2D1RenderTarget* global_d2d_render_target;
extern IDWriteFactory* global_dwrite_factory;
extern IDWriteFontCollection* global_dwrite_collection;

#endif