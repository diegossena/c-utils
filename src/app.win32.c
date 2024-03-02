#include "internal/platform.h"

#if PLATFORM_WINDOWS

#include "internal/console.h"

#include "sdk/application.h"
#include "sdk/error.h"
#include <windows.h>

void app_inicialize() {
  // console
  OSVERSIONINFO os_version = { .dwOSVersionInfoSize = sizeof(OSVERSIONINFO) };
  if (GetVersionEx(&os_version) && os_version.dwMajorVersion >= 10) {
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (handle == INVALID_HANDLE_VALUE) {
      error("GetStdHandle", ERR_INVALID_HANDLE);
    } else {
      DWORD console_mode;

      GetConsoleMode(handle, &console_mode);
      console_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
      SetConsoleMode(handle, console_mode);

      handle = GetStdHandle(STD_ERROR_HANDLE);

      GetConsoleMode(handle, &console_mode);
      console_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
      SetConsoleMode(handle, console_mode);

      console_has_ansi = true;
    }
  }
  // net.socket
  WSADATA wsaData;
  error_last = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (error_last) {
    error("WSAStartup", error_last);
    WSACleanup();
  }
}

void app_shutdown() {
  // net.socket
  WSACleanup();
}

#endif