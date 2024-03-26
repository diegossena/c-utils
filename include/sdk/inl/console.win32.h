#ifdef SDK_CONSOLE_H
#include <sdk/platform.h>
#ifdef PLATFORM_WINDOWS

static bool console_has_ansi = false;

i32 console_write(log_level_t level, const char* message, ...);

void console_startup() {
  OSVERSIONINFO os_version = { .dwOSVersionInfoSize = sizeof(OSVERSIONINFO) };
  if (GetVersionEx(&os_version) && os_version.dwMajorVersion >= 10) {
    HANDLE handle;
    DWORD console_mode;
    handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (handle == INVALID_HANDLE_VALUE) {
      error("GetStdHandle(STD_OUTPUT_HANDLE)", ERR_INVALID_HANDLE);
    } else {
      GetConsoleMode(handle, &console_mode);
      console_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
      SetConsoleMode(handle, console_mode);
    }
    handle = GetStdHandle(STD_ERROR_HANDLE);
    if (handle == INVALID_HANDLE_VALUE) {
      error("GetStdHandle(STD_ERROR_HANDLE)", ERR_INVALID_HANDLE);
    } else {
      GetConsoleMode(handle, &console_mode);
      console_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
      SetConsoleMode(handle, console_mode);
    }
    console_has_ansi = true;
  }
}

i32 console_write_str(log_level_t level, const char* message, u64 length) {
  DWORD nStdHandle = level < LOG_LEVEL_WARN ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE;
  HANDLE handle = GetStdHandle(nStdHandle);
  if (handle == INVALID_HANDLE_VALUE) {
    error("GetStdHandle", ERR_INVALID_HANDLE);
    return ERR_INVALID_HANDLE;
  }
  if (!console_has_ansi && message[0] == '\033' && message[1] == '[') {
    u8 ansi_code = message[4] == 'm'
      ? (message[2] - '0') * 10 + message[3] - '0'
      : message[2] - '0';
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(handle, &info);
    DWORD back = (info.wAttributes >> 4) & 0x0f;
    DWORD fore = info.wAttributes & 0x0f;
    info.wAttributes = info.wAttributes & FOREGROUND_INTENSITY;
    if (ansi_code == 0) {
      // reset
      info.wAttributes = 0;
      back = 0;
      fore = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    } else if (ansi_code == 1) {
      // bold
      fore |= FOREGROUND_INTENSITY;
    } else if (ansi_code == 4) {
      // underline
      info.wAttributes |= COMMON_LVB_UNDERSCORE;
    } else if (ansi_code >= 30 && ansi_code <= 39 || ansi_code >= 90 && ansi_code <= 99) {
      // foreground
      fore = 0;
      if (ansi_code >= 90 && ansi_code <= 99) {
        fore |= FOREGROUND_INTENSITY;
        ansi_code -= 60;
      }
      switch (ansi_code) {
        case 31: // red
          fore |= FOREGROUND_RED;
          break;
        case 32: // green
          fore |= FOREGROUND_GREEN;
          break;
        case 33: // yellow
          fore |= FOREGROUND_RED | FOREGROUND_GREEN;
          break;
        case 34: // blue
          fore |= FOREGROUND_BLUE;
          break;
        case 35: // magenta
          fore |= FOREGROUND_RED | FOREGROUND_BLUE;
          break;
        case 36: // cyan
          fore |= FOREGROUND_GREEN | FOREGROUND_BLUE;
          break;
        case 37: // white
        case 39: // reset
          fore |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
          break;
      }
    } else if (ansi_code >= 40 && ansi_code <= 49 || ansi_code >= 100 && ansi_code <= 109) {
      // background
      back = 0;
      if (ansi_code >= 100 && ansi_code <= 109) {
        back |= FOREGROUND_INTENSITY;
        ansi_code -= 60;
      }
      switch (ansi_code) {
        case 41: // red
          back |= FOREGROUND_RED;
          break;
        case 42: // green
          back |= FOREGROUND_GREEN;
          break;
        case 43: // yellow
          back |= FOREGROUND_RED | FOREGROUND_GREEN;
          break;
        case 44: // blue
          back |= FOREGROUND_BLUE;
          break;
        case 45: // magenta
          back |= FOREGROUND_RED | FOREGROUND_BLUE;
          break;
        case 46: // cyan
          back |= FOREGROUND_GREEN | FOREGROUND_BLUE;
          break;
        case 47: // white
        case 49: // reset
          back |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
          break;
      }
    }
    SetConsoleTextAttribute(handle, info.wAttributes | (back << 4) | fore);
    return ERR_SUCCESS;
  }

  WriteConsoleA(handle, message, length, 0, 0);
  return ERR_SUCCESS;
}

#endif
#endif