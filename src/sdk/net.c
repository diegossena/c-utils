#include <sdk/net.h>

u64 __net_max_fd = 0;

SDK_EXPORT u32 ip4_from_bytes(byte_t f1, byte_t f2, byte_t f3, byte_t f4) {
  return f1 | f2 << 8 | f3 << 16 | f4 << 24;
}
SDK_EXPORT u32 ip4_from_cstr(const char* ip4_str) {
  u32 ip4 = 0;
  u8* ip4_u8 = (u8*)&ip4;
  u8 i = 0;
  while (*ip4_str != '\0') {
    if (*ip4_str >= '0' && *ip4_str <= '9') {
      ip4_u8[i] = ip4_u8[i] * 10 + *ip4_str - '0';
    } else if (*ip4_str == '.') {
      ip4 |= ip4_u8[i] << 8 * i;
      ++i;
    } else {
      break;
    }
    ++ip4_str;
  }
  return ip4;
}
SDK_EXPORT const char* ip4_to_cstr(u32 ip4) {
  char* str_ptr = buffer_shared;
  char int_str[3], * int_str_ptr;
  u8 place = 0, number;
  while (true) {
    number = (ip4 >> 8 * place) & 0xFF;
    // number stringify
    int_str_ptr = int_str;
    do {
      *int_str_ptr++ = '0' + number % 10;
      number /= 10;
    } while (number != 0);
    // copy number
    do {
      *str_ptr++ = *--int_str_ptr;
    } while (int_str_ptr != int_str);
    // iterate
    if (++place == 4) {
      *str_ptr = '\0';
      break;
    }
    *str_ptr++ = '.';
  }
  return buffer_shared;
}
/**
 * `ip4++`
 */
SDK_EXPORT u32 ip4_increment(u32 ip4) {
  u8* ip4_u8 = (u8*)&ip4;
  for (int i = 3; i >= 0; i--) {
    ip4_u8[i] += 1;
    if (ip4_u8[i])
      break;
  }
  return ip4;
}
/**
 * `ip4_left <= ip4_right`
 */
SDK_EXPORT bool ip4_lessequal(u32 ip4_left, u32 ip4_right) {
  u8* ip4_left_u8 = (u8*)&ip4_left;
  u8* ip4_right_u8 = (u8*)&ip4_right;
  for (u8 i = 0; i < 4; i++) {
    if (ip4_left_u8[i] > ip4_right_u8[i]) {
      return false;
    } else if (ip4_left_u8[i] < ip4_right_u8[i]) {
      return true;
    }
  }
  return true;
}