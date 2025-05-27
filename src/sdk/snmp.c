#include <sdk/snmp.h>

error_t snmp_request(udp_t udp, pdu_t* pdu, ip4_t host) {
  error_t error;
  u8 buffer[TEXT_SIZE] = { '\0' };
  u8* stream;
  u16 buffer_size = 0;
  // pdu_to_buffer
  stream = buffer;
  u8* sequence = ber_sequence_start(&stream, ASN1_TYPE_SEQUENCE);
  ber_write_var_integer(&stream, pdu->version);
  ber_write_str(&stream, pdu->community, pdu->community_length);
  u8* pdu_sequence = ber_sequence_start(&stream, pdu->type);
  ber_write_var_integer(&stream, pdu->request_id);
  ber_write_var_integer(&stream, pdu->error);
  ber_write_var_integer(&stream, pdu->error_index);
  u8* varbind_list_sequence = ber_sequence_start(&stream, ASN1_TYPE_SEQUENCE);
  for (u64 i = 0; i < pdu->varbinds_length; i++) {
    u8* varbind_start = ber_sequence_start(&stream, ASN1_TYPE_SEQUENCE);
    ber_write_oid_null(&stream, pdu->varbinds[i].oid, pdu->varbinds[i].oid_length);
    ber_sequence_end(&stream, varbind_start);
  }
  ber_sequence_end(&stream, varbind_list_sequence);
  ber_sequence_end(&stream, pdu_sequence);
  ber_sequence_end(&stream, sequence);
  buffer_size = stream - buffer;
  assert(buffer_size <= TEXT_SIZE);
  // snmp_request
  net_address_t address = {
    .family = NET_FAMILY_IPV4,
    .ip4 = host,
    .net_port = net_port_from_short(SNMP_PORT)
  };
  error = udp_send(udp, buffer, buffer_size, &address);
  if (error)
    return error;
  // snmp_response
  net_address_t response_address;
  error = udp_read(udp, buffer, sizeof(buffer), &response_address);
  if (error)
    return error;
  stream = buffer + 7 + pdu->community_length;
  // pdu.type
  pdu->type = *stream;
  stream += 2;
  // request_id
  stream += 3;
  // error
  stream += 2; // 0x02 0x01
  pdu->error = ber_read_u64(&stream, 1);
  assert(pdu->error == 0);
  // error_index
  stream += 2; // 0x02 0x01
  pdu->error_index = ber_read_u64(&stream, 1);
  // varbind_list
  stream += 2;
  for (u64 i = 0; i < pdu->varbinds_length; i++) {
    // OID
    stream += 3;
    u64 size = ber_read_size(&stream);
    stream += size;
    // value
    pdu->varbinds[i].value.type = *stream++;
    if (pdu->varbinds[i].value.type == ASN1_TYPE_NULL) {
      ++stream;
      continue;
    }
    pdu->varbinds[i].value.size = ber_read_size(&stream);
    switch (pdu->varbinds[i].value.type) {
      case ASN1_TYPE_BOOLEAN:
        pdu->varbinds[i].value.boolean = *stream++;
        break;
      case ASN1_TYPE_INTEGER:
        pdu->varbinds[i].value.i64 = ber_read_i64(&stream, pdu->varbinds[i].value.size);
        break;
      case ASN1_TYPE_COUNTER64:
      case ASN1_TYPE_COUNTER:
        pdu->varbinds[i].value.u64 = ber_read_u64(&stream, pdu->varbinds[i].value.size);
        break;
      case ASN1_TYPE_OID:
      case ASN1_TYPE_OCTETSTRING: {
        u64 value_size = pdu->varbinds[i].value.size;
        u8* target = pdu->varbinds[i].value.oid;
        while (value_size--)
          *target++ = *stream++;
        break;
      }
      default:
    }
  }
  return ERR_SUCCESS;
}
