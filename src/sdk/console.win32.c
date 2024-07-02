#include <sdk/console.h>
#ifdef PLATFORM_WINDOWS
#include <windows.h>

SDK_EXPORT void console_color(ansi_t ansi) {
  HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO info;
  GetConsoleScreenBufferInfo(handle, &info);
  DWORD back = (info.wAttributes >> 4) & 0x0f;
  DWORD fore = info.wAttributes & 0x0f;
  info.wAttributes = info.wAttributes & FOREGROUND_INTENSITY;
  if (ansi == 0) {
      // reset
    info.wAttributes = 0;
    back = 0;
    fore = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
  } else if (ansi == 1) {
    // bold
    fore |= FOREGROUND_INTENSITY;
  } else if (ansi == 4) {
    // underline
    info.wAttributes |= COMMON_LVB_UNDERSCORE;
  } else if (ansi >= 30 && ansi <= 39 || ansi >= 90 && ansi <= 99) {
    // foreground
    fore = 0;
    if (ansi >= 90 && ansi <= 99) {
      fore |= FOREGROUND_INTENSITY;
      ansi -= 60;
    }
    switch (ansi) {
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
  } else if (ansi >= 40 && ansi <= 49 || ansi >= 100 && ansi <= 109) {
    // background
    back = 0;
    if (ansi >= 100 && ansi <= 109) {
      back |= FOREGROUND_INTENSITY;
      ansi -= 60;
    }
    switch (ansi) {
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
}
SDK_EXPORT void console_write_str(const char* message, u64 length) {
  HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
  WriteConsoleA(handle, message, length, 0, 0);
}

#endif