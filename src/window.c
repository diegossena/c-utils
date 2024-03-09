#include "sdk/window/window.h"
#include "internal/platform.h"

#if PLATFORM_WINDOWS
#include "internal/window.win32.h"
#endif

void* window_get_context(window_t* this) { return this->context; }