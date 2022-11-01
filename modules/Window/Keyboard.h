#pragma once

#include <winuser.h>

typedef enum Keycode {
  KEY_BACK = 0x8,
  KEY_TAB,
  KEY_ENTER = 0xd,
  KEY_SHIFT = 0x10,
  KEY_CTRL,
  KEY_ALT,
  KEY_PAUSE,
  KEY_CAPS,
  KEY_ESC = 0x1b,
  KEY_SPACE = 0x20,
  KEY_LEFT = 0x25,
  KEY_UP,
  KEY_RIGHT,
  KEY_DOWN,
} Keycode;