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
  if (true) {
    // 255845 bytes
    i32 result;
    error = net_startup();
    if (error) {
      console_log("net_startup %d %s", error, error_cstr(error));
      goto udp_exit;
    }

    udp_t udp_server = udp_new();
    if (!udp_server) {
      error = net_error();
      console_log("udp_new %d %s", error, error_cstr(error));
      udp_free(udp_server);
      goto udp_exit;
    }
    error = udp_bind(udp_server, net_port_from_short(9100));
    if (error) {
      console_log("udp_bind %d %s", error, error_cstr(error));
      goto udp_exit;
    }

    udp_t udp_client = udp_new();
    if (!udp_client) {
      error = net_error();
      console_log("udp_new %d %s", error, error_cstr(error));
      udp_free(udp_client);
      udp_free(udp_server);
      goto udp_exit;
    }

    net_address_t server_address = {
      .family = NET_FAMILY_IPV4,
      .net_port = net_port_from_short(9100),
      .ip4 = ip4_from_bytes(127, 0, 0, 1)
    };
    const char client_data [] = "test";
    result = udp_send(udp_client, client_data, sizeof(client_data) - 1, &server_address);
    if (result < 0) {
      error = net_error();
      console_log("udp_send %d %s", error, error_cstr(error));
    }

    char server_data[TEXT_SIZE];
    net_address_t client_address;
    result = 0;
    while (result == 0) {
      console_log("udp_read");
      result = udp_read(udp_server, server_data, sizeof(server_data) - 1, &client_address);
      if (result < 0) {
        error = net_error();
        console_log("udp_read %d %s", error, error_cstr(error));
        goto udp_exit;
      }
    }
    console_log("data '%s'", server_data);
    udp_free(udp_client);
    udp_free(udp_server);
    net_shutdown();
  udp_exit:
  }
  return 0;
}