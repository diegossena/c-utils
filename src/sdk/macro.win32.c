#include <sdk/macro.h>
#ifdef PLATFORM_WINDOWS

#include <windows.h>

inline void cursor_set(u32 x, u32 y) { SetCursorPos(x, y); }

#endif