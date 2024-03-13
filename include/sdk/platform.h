#pragma once

#if __CYGWIN__
#error "CYGWIN is not supported"
#endif

#if WIN32 || _WIN32 || __WIN32__

#define PLATFORM_WINDOWS 1

// #include <winsock2.h>
// #include <windows.h>
// #include <fileapi.h>
// #include <shlwapi.h>
// #include <wincodec.h>
// #define COBJMACROS 1
// #include <d3d11.h>
// #include <d3dcompiler.h>
// #include <initguid.h>
// #include <d2d1.h>
// #include <dwrite.h>
// #undef COBJMACROS

#else
#error "This platform is not supported"
#endif


#if _WIN64 || __x86_64__
#define PLATFORM_X64 1
#endif