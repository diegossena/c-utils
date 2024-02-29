#include "base/platform.h"

#if PLATFORM_WINDOWS

#include "sdk/console.h" // console_inicialize
#include "sdk/string.h" // string_format

#include "base/console.win32.h" // has_ansi
/*
GetStdHandle, GetConsoleMode, SetConsoleMode, DWORD, HANDLE, OSVERSIONINFO
ENABLE_VIRTUAL_TERMINAL_PROCESSING,
*/
#include <windows.h> // 

error_code console_inicialize() {
  OSVERSIONINFO os_version = { .dwOSVersionInfoSize = sizeof(OSVERSIONINFO) };
  if (GetVersionEx(&os_version) && os_version.dwMajorVersion >= 10) {
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (handle == INVALID_HANDLE_VALUE)
      return ERR_INVALID_HANDLE;

    DWORD console_mode;

    GetConsoleMode(handle, &console_mode);
    console_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(handle, console_mode);

    handle = GetStdHandle(STD_ERROR_HANDLE);

    GetConsoleMode(handle, &console_mode);
    console_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(handle, console_mode);

    has_ansi = true;
  }
}

#endif