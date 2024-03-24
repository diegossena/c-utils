#include <sdk/keyboard.h>

static u8 __keyboard_count = 0;

static u8 __keyboard_state[32] = {};
bool keyboard_pressed(key_code_t key) {
  u8 byte_index = key / 8;
  u8 bit_index = key % 8;
  return __keyboard_state[byte_index] & (1 << bit_index);
}
void __keyboard_press(key_code_t key) {
  u8 byte_index = key / 8;
  u8 bit_index = key % 8;
  __keyboard_state[byte_index] |= (1 << bit_index);
}
void __keyboard_release(key_code_t key) {
  u8 byte_index = key / 8;
  u8 bit_index = key % 8;
  __keyboard_state[byte_index] &= ~(1 << bit_index);
}

// static bool __keyboard_state[255] = {};
// bool keyboard_pressed(key_code_t key) {
//   return __keyboard_state[key];
// }
// void __keyboard_press(key_code_t key) {
//   __keyboard_state[key] = true;
// }
// void __keyboard_release(key_code_t key) {
//   __keyboard_state[key] = false;
// }