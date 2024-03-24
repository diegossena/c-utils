#include <sdk/keyboard.h>

static u32 __keyboard_state[8] = {};
static u8 __keyboard_count = 0;

bool keyboard_pressed(key_code_t key) {
  u8 index = key % 8;
  return __keyboard_state[index] & key;
}
void __keyboard_press(key_code_t key) {
  u8 index = key % 8;
  __keyboard_state[index] |= key;
}
void __keyboard_release(key_code_t key) {
  u8 index = key % 8;
  __keyboard_state[index] &= ~key;
}