#include <sdk/types.h>
#include <sdk/tcp.h>
#include <sdk/udp.h>
#include <sdk/string.h>
#include <sdk/console.h>
#include <sdk/math.h>
#include <sdk/snmp.h>
#include <sdk/unity.h>

i32 main() {
  i32 result;
  error_t error = net_startup();
  if (error) {
    console_log("net_startup %d %s", error, error_cstr(error));
    goto exit;
  }
  if (false) {
    // 251864 bytes
    console_log("value %f", math_pow(2, 2));
  }
  if (false) {
    // 251926 bytes
    char text[TINY_SIZE + 1];
    string_format(text, TINY_SIZE, "test %s", "test");
    console_log("text '%s'", text);
  }
  if (false) {
    // 256757 bytes
    tcp_t tcp = tcp_new();
    error = tcp_connect(tcp, ip4_from_bytes(192, 168, 0, 189), net_port_from_short(9100), 1000);
    console_log("tcp_connect %d %s", error, error_cstr(error));
    tcp_free(tcp);
    net_shutdown();
  }
  if (false) {
    // 256677 bytes
    udp_t udp_server = udp_new();
    if (!udp_server) {
      error = net_error();
      console_log("udp_new %d %s", error, error_cstr(error));
      udp_free(udp_server);
      goto udp_server_exit;
    }
    error = udp_bind(udp_server, net_port_from_short(9100));
    if (error) {
      console_log("udp_bind %d %s", error, error_cstr(error));
      goto udp_server_exit;
    }

    udp_t udp_client = udp_new();
    if (!udp_client) {
      error = net_error();
      console_log("udp_new %d %s", error, error_cstr(error));
      goto udp_client_exit;
    }

    net_address_t server_address = {
      .family = NET_FAMILY_IPV4,
      .net_port = net_port_from_short(9100),
      .ip4 = ip4_from_bytes(127, 0, 0, 1)
    };
    const char server_data [] = "Hello World";
    result = udp_send(udp_client, server_data, sizeof(server_data) - 1, &server_address);
    console_log("udp server says  '%s'", server_data);
    if (result < 0) {
      error = net_error();
      console_log("udp_send %d %s", error, error_cstr(error));
    }

    char client_data[TEXT_SIZE];
    net_address_t client_address;
    result = udp_read(udp_server, client_data, sizeof(client_data) - 1, &client_address);
    if (result < 0) {
      error = net_error();
      console_log("udp_read %d %s", error, error_cstr(error));
      goto udp_client_exit;
    }
    console_log("udp client hears '%s'", client_data);
  udp_client_exit:
    udp_free(udp_client);
  udp_server_exit:
    udp_free(udp_server);
  }
  if (true) {
    // 255496 bytes
    udp_t udp = udp_new();
    if (!udp) {
      error = net_error();
      console_log("udp_new %d %s", error, error_cstr(error));
      udp_free(udp);
      goto snmp_udp_exit;
    }
    const char community [] = "public";
    const u8 serial_oid [] = { OID_PREFIX, 6, 1, 2, 1, 43, 5, 1, 1, 17, 1 };
    varbind_t varbinds [] = {
      { serial_oid, sizeof(serial_oid) }
    };
    pdu_t pdu = {
      .type = PDU_TYPE_GET_REQUEST,
      .version = SNMP_VERSION_1,
      .varbinds = varbinds,
      .varbinds_length = sizeof(varbinds) / sizeof(varbind_t),
      .community = community,
      .community_length = sizeof(community) - 1,
    };
    error = snmp_request(udp, &pdu, ip4_from_bytes(192, 168, 0, 255));
    if (error) {
      console_log("snmp_request %d %s", error, error_cstr(error));
      goto snmp_udp_exit;
    }
  snmp_udp_exit:
    udp_free(udp);
  }
exit:
  net_shutdown();
  return 0;
}