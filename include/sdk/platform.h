#pragma once

#if WIN32 || _WIN32 || __WIN32__

#define PLATFORM_WINDOWS

#ifdef SDK_NET_H
#include <winsock2.h>
#endif // !SDK_NET_H

#include <windows.h>

#ifdef SDK_WINDOW_H
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
#endif // !SDK_WINDOW_H

#endif // !PLATFORM_WINDOWS

#if _WIN64 || __x86_64__
#define PLATFORM_X64 1
#endif