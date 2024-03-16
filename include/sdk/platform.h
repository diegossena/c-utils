#pragma once

#if __CYGWIN__
#error "CYGWIN is not supported"
#endif

#if WIN32 || _WIN32 || __WIN32__

#define PLATFORM_WINDOWS 1

#else
#error "This platform is not supported"
#endif


#if _WIN64 || __x86_64__
#define PLATFORM_X64 1
#endif