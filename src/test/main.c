#include <sdk/types.h>
#include <sdk/tcp.h>
#include <sdk/udp.h>
#include <sdk/string.h>
#include <sdk/console.h>
#include <sdk/math.h>
#include <sdk/snmp.h>
#include <sdk/hashset.h>
#include <sdk/unity.h>

i32 main() {
  error_t error = net_startup();
  if (error) {
    console_log("net_startup %d %s", error, error_cstr(error));
    goto exit;
  }
  if (false) {
    // 256100 bytes 130ms
    hashset_t* hashset = hashset_new();
    for (u16 i = 0; i < MAX_U16; i++) {
      hashset_add(&hashset, i);
    }
    for (u16 i = 0; i < MAX_U16; i++) {
      if (!hashset_contains(hashset, i)) {
        console_log("contains %d", i);
        break;
      }
    }
    if (hashset_contains(hashset, MAX_U16 + 1)) {
      console_log("hashset_contains %llu", MAX_U16 + 1);
    }
    hashset_free(hashset);
  }
  if (false) {
    // 255412 bytes 80ms
    console_log("value %lf", math_pow(2, 2));
  }
  if (false) {
    // 251926 bytes
    char text[TINY_SIZE + 1];
    string_format(text, sizeof(text), "test %s 123456789", "test");
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
    // 255495 bytes 100ms
    u8 array [] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    console_write_buffer(array, sizeof(array));
    console_write_cstr("\n");
  }
  if (false) {
    // 257701 bytes 720ms
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
    const u8 server_data [] = "Hello World";
    error = udp_send(udp_client, server_data, sizeof(server_data) - 1, &server_address);
    console_log("udp server says  '%s'", server_data);
    if (error) {
      console_log("udp_send %d %s", error, error_cstr(error));
    }

    u8 client_data[TEXT_SIZE + 1];
    net_address_t client_address;
    error = udp_read(udp_server, client_data, sizeof(client_data) - 1, &client_address);
    if (error) {
      console_log("udp_read %d %s", error, error_cstr(error));
      goto udp_client_exit;
    }
    console_log("udp client hears '%s'", client_data);
  udp_client_exit:
    udp_free(udp_client);
  udp_server_exit:
    udp_free(udp_server);
  }
  if (false) {
    // 257041 bytes
    udp_t udp = udp_new();
    if (!udp) {
      error = net_error();
      console_log("udp_new %d %s", error, error_cstr(error));
      udp_free(udp);
      goto snmp_udp_exit;
    }
    const char community [] = "public";
    const u8 serial_oid [] = { OID_PREFIX, 6, 1, 2, 1, 43, 5, 1, 1, 17, 1 };
    const u8 brand_name_oid [] = { OID_PREFIX, 6, 1, 2, 1, 43, 8, 2, 1, 14, 1, 1 };
    const u8 model_name_oid [] = { OID_PREFIX, 6, 1, 4, 1, asn1_int2(641), 2, 1, 2, 1, 2, 1 };
    const u8 total_oid [] = { OID_PREFIX, 6, 1, 4, 1, asn1_int2(641), 6, 4, 2, 1, 1, 4, 1, 3 };
    varbind_t varbinds [] = {
      { serial_oid, sizeof(serial_oid) },
      { brand_name_oid, sizeof(brand_name_oid) },
      { model_name_oid, sizeof(model_name_oid) },
      { total_oid, sizeof(total_oid) }
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
    console_log("serial '%s'", pdu.varbinds[0].value.str);
    console_log("brand_name '%s'", pdu.varbinds[1].value.str);
    console_log("model_name '%s'", pdu.varbinds[2].value.str);
    console_log("total %llu", pdu.varbinds[3].value.u64);
  snmp_udp_exit:
    udp_free(udp);
  }
  if (true) {
    console_write_cstr("TCP TEST\n");
    // 259005 bytes 5950ms
    tcp_t tcp = tcp_new();
    if (!tcp) {
      error = net_error();
      console_log("tcp_new %d %s", error, error_cstr(error));
      tcp_free(tcp);
      goto http_exit;
    }
    error = tcp_connect(tcp, ip4_from_bytes(142, 251, 129, 78), net_port_from_short(80), 1000);
    if (error) {
      console_log("tcp_connect %d %s", error, error_cstr(error));
      goto http_exit;
    }
    const char http_request [] =
      "GET / HTTP/1.0\r\n"
      "Host: www.google.com.br\r\n"
      "Connection: close\r\n"
      "\r\n";
    error = tcp_send(tcp, http_request, sizeof(http_request) - 1);
    if (error) {
      console_log("tcp_send %d %s", error, error_cstr(error));
      goto http_exit;
    }
    char http_response[TEXT_SIZE + 1];
    error = tcp_read(tcp, http_response, sizeof(http_response) - 1);
    if (error) {
      console_log("tcp_read %d %s", error, error_cstr(error));
      goto http_exit;
    }
    console_log("http_response\n%s", http_response);
  http_exit:
    tcp_free(tcp);
  }
exit:
  net_shutdown();
  debug();
  return 0;
}