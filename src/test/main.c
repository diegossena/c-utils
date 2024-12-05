#include <sdk/types.h>
#include <sdk/tcp.h>
#include <sdk/udp.h>
#include <sdk/string.h>
#include <sdk/console.h>
#include <sdk/math.h>
#include <sdk/unity.h>

i32 main() {
  error_t error;
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
  if (false) {
    // 256757 bytes
    net_startup();
    tcp_t tcp = tcp_new();
    error = tcp_connect(tcp, ip4_from_bytes(192, 168, 0, 189), net_port_from_short(9100), 1000);
    console_log("tcp_connect %d %s", error, error_cstr(error));
    tcp_free(tcp);
    net_shutdown();
  }
  if (false) {
    // 255845 bytes
    error = net_startup();
    if (error) {
      console_log("net_startup %d %s", error, error_cstr(error));
      goto udp_client_exit;
    }
    udp_t udp = udp_new();
    if (!udp) {
      error = net_error();
      console_log("udp_new %d %s", error, error_cstr(error));
      goto udp_client_exit;
    }
    net_address_t address = {
      .family = NET_FAMILY_IPV4,
      .net_port = net_port_from_short(9100),
      .ip4 = ip4_from_bytes(127, 0, 0, 1)
    };
    const char data [] = "test";
    i32 result = udp_send(udp, data, sizeof(data) - 1, &address);
    if (result < 0) {
      error = net_error();
      console_log("udp_send %d %s", error, error_cstr(error));
    }
    udp_free(udp);
  udp_client_exit:
    net_shutdown();
  }
  if (true) {
    // 255845 bytes
    error = net_startup();
    if (error) {
      console_log("net_startup %d %s", error, error_cstr(error));
      goto udp_server_exit;
    }
    udp_t udp = udp_new();
    if (!udp) {
      error = net_error();
      console_log("udp_new %d %s", error, error_cstr(error));
      goto udp_server_exit;
    }
    error = udp_bind(udp, net_port_from_short(9100));
    if (error) {
      console_log("udp_bind %d %s", error, error_cstr(error));
      goto udp_server_exit;
    }
    char data[TEXT_SIZE];
    net_address_t address;
    i32 result = 0;
    while (result == 0) {
      console_log("udp_read");
      result = udp_read(udp, data, sizeof(data) - 1, &address);
      if (result < 0) {
        error = net_error();
        console_log("udp_read %d %s", error, error_cstr(error));
        goto udp_server_exit;
      }
    }
    console_log("data '%s'", data);
    udp_free(udp);
  udp_server_exit:
    net_shutdown();
  }
  return 0;
}