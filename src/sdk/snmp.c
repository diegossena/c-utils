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
  char* varbind_list_sequence = ber_sequence_start(&stream, ASN1_TYPE_SEQUENCE);
  for (u64 i = 0; i < pdu->varbinds_length; i++) {
    char* varbind_start = ber_sequence_start(&stream, ASN1_TYPE_SEQUENCE);
    ber_write_oid_null(&stream, pdu->varbinds[i].oid, pdu->varbinds[i].oid_length);
    ber_sequence_end(&stream, varbind_start);
  }
  ber_sequence_end(&stream, varbind_list_sequence);
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
  if (result < 0) {
    return net_error();
  }
  // response udp_read
  net_address_t response_address;
  result = udp_read(udp, buffer, sizeof(buffer), &response_address);
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
    /**
     * 30: ASN.1 Sequence
     *  ?: varbind.size
     * 06: ASN.1 OID
     * ...: OID size
     */
    stream += 3;
    u64 size = ber_read_size(&stream);
    stream += size;
    // value
    pdu->varbinds[i].value.type = *stream++;
    if (pdu->varbinds[i].value.type != ASN1_TYPE_NULL) {
      pdu->varbinds[i].value.size = ber_read_size(&stream);
    }
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
      // case ASN1_TYPE_BITSTRING:
      //   break;
      // case ASN1_TYPE_NULL:
      //   break;
      default:
    }
  }
  if (result < 0) {
    return net_error();
  }
  return ERR_SUCCESS;
}
