#include <sdk/types.h>
#include <sdk/tcp_sync.h>
#include <sdk/string.h>
#include <sdk/console.h>
#include <sdk/math.h>
#include <sdk/unity.h>

i32 main() {
  if (true) {
    // 251882
    console_log("value %f", math_pow(2, 2));
  }
  if (false) {
    // 251926
    char text[TEXT_SIZE + 1];
    string_format(text, TEXT_SIZE, "test %s", "test");
    console_log("text '%s'", text);
  }
  if (false) {
    // 256780
    net_startup();
    tcp_socket_t tcp = socket_new(SOCKET_TYPE_STREAM);
    error_t code = tcp_sync_connect(tcp, ip4_from_bytes(192, 168, 0, 189), net_port_from_short(9100), 1000);
    socket_free(tcp);
    console_log("code %d %s", code, error_cstr(code));
    net_shutdown();
  }
  return 0;
}