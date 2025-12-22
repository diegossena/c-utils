#include <sdk/net.h>

inline u16 net_port_short(u16 port) {
  return (port << 8) | (port >> 8);
}
inline ip4_t ip4_from_bytes(u8 oct1, u8 oct2, u8 oct3, u8 oct4) {
  return oct1 | oct2 << 8 | oct3 << 16 | oct4 << 24;
}
u32 ip4_from_str(const char* ip4_str) {
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
/**
 * @return out[16]
 */
void ip4_to_str(u32 ip4, char* out) {
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
      *out++ = *--int_str_ptr;
    } while (int_str_ptr != int_str);
    // iterate
    if (++place == 4) {
      *out = '\0';
      break;
    }
    *out++ = '.';
  }
}
/**
 * `ip4++`
 */
u32 ip4_increment(u32 ip4) {
  u8* ip4_u8 = (u8*)&ip4;
  for (u8 i = 3; i >= 0; i--) {
    ip4_u8[i] += 1;
    if (ip4_u8[i])
      break;
  }
  return ip4;
}
/**
 * `ip4_left <= ip4_right`
 */
bool ip4_lessequal(u32 ip4_left, u32 ip4_right) {
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