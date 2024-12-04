#include <sdk/net.h>

export net_address_t ip4_address(u32 ip4, u16 net_port) {
  return (net_address_t) { NET_FAMILY_IPV4, net_port, { ip4 } };
}
export u32 ip4_from_bytes(u8 f1, u8 f2, u8 f3, u8 f4) {
  return f1 | f2 << 8 | f3 << 16 | f4 << 24;
}
export u32 ip4_from_cstr(const char* ip4_str) {
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
export void ip4_to_cstr(u32 ip4, ip4_string_t cstr) {
  char* ptr = cstr;
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
      *ptr++ = *--int_str_ptr;
    } while (int_str_ptr != int_str);
    // iterate
    if (++place == 4) {
      *ptr = '\0';
      break;
    }
    *ptr++ = '.';
  }
}
/**
 * `ip4++`
 */
export u32 ip4_increment(u32 ip4) {
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
export bool ip4_lessequal(u32 ip4_left, u32 ip4_right) {
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