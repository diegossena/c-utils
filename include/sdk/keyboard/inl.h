#include <sdk/keyboard.h>

static u32 __keyboard_state[8] = {};
static u8 __keyboard_count = 0;

bool keyboard_pressed(key_code_t key) {
  return __keyboard_state[key % 8] & key;
}