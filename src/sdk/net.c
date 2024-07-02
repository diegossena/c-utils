#include <sdk/net.h>

u64 __net_max_fd = 0;

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
SDK_EXPORT u32 ip4_increment(u32 ip4) {
  if (ip4 >> 24 == 0xFF) {
    if ((ip4 >> 16 & 0xFF) == 0xFF) {
      if ((ip4 >> 8 & 0xFF) == 0xFF) {
        if ((ip4 & 0xFF) == 0xFF) {
          return 0;
        }
        ip4 += 1;
      }
      ip4 += 1 << 8;
    }
    ip4 += 1 << 16;
  }
  return ip4 + (1 << 24);
}