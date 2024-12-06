#include <sdk/snmp.h>

export error_t snmp_request(udp_t udp, pdu_t* pdu, ip4_t host) {
  // pdu->bytes
  char buffer[TEXT_SIZE];
  char* stream = buffer;
  char* sequence = ber_sequence_start(&stream, ASN1_TYPE_SEQUENCE);
  ber_write_var_integer(&stream, pdu->version);
  ber_write_str(&stream, pdu->community, pdu->community_length);
  char* pdu_sequence = ber_sequence_start(&stream, pdu->type);
  ber_write_var_integer(&stream, pdu->request_id);
  ber_write_var_integer(&stream, pdu->error);
  ber_write_var_integer(&stream, pdu->error_index);
  char* varbind_list = ber_sequence_start(&stream, ASN1_TYPE_SEQUENCE);
  for (u64 i = 0; i < pdu->varbinds_length; i++) {
    char* varbind_start = ber_sequence_start(&stream, ASN1_TYPE_SEQUENCE);
    ber_write_oid_null(&stream, pdu->varbinds[i].oid, pdu->varbinds[i].oid_length);
    ber_sequence_end(&stream, varbind_start);
  }
  ber_sequence_end(&stream, varbind_list);
  ber_sequence_end(&stream, pdu_sequence);
  ber_sequence_end(&stream, sequence);
  // buffer
  u64 size = stream - buffer;
  // udp_send
  net_address_t address = {
    .family = NET_FAMILY_IPV4,
    .ip4 = host,
    .net_port = net_port_from_short(SNMP_PORT)
  };
  i32 result = udp_send(udp, buffer, size, &address);
  console_log("udp_send %d size %d", result, size);
  if (result < 0) {
    return net_error();
  }
  memory_fill(buffer, '\0', size);
  // udp_read
  net_address_t response_address;
  result = udp_read(udp, buffer, TEXT_SIZE, &response_address);
  console_log("udp_read %d", result);
  if (result < 0) {
    return net_error();
  }
  return ERR_SUCCESS;
}
