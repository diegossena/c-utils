#include <sdk/types.h>
#include <sdk/tcp.h>
#include <sdk/string.h>
#include <sdk/console.h>
#include <sdk/math.h>
#include <sdk/unity.h>

i32 main() {
  if (false) {
    // 251864 bytes
    console_log("value %f", math_pow(2, 2));
  }
  if (false) {
    // 251926 bytes
    char text[TEXT_SIZE + 1];
    string_format(text, TEXT_SIZE, "test %s", "test");
    console_log("text '%s'", text);
  }
  if (true) {
    // 256757 bytes
    net_startup();
    tcp_t tcp = tcp_new();
    error_t code = tcp_connect(tcp, ip4_from_bytes(192, 168, 0, 189), net_port_from_short(9100), 1000);
    tcp_free(tcp);
    console_log("code %d %s", code, error_cstr(code));
    net_shutdown();
  }
  return 0;
}