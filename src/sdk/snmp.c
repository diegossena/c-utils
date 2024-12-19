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
  // console_write_buffer((u8*)buffer, result);
  // console_log();
  /*
  30 3c - Sequence
  02 01 00 - Version
  04 06 70 75 62 6c 69 63 - [ASN.1 Octet String] [size] [community="public"]
  a2 2f - [PDU TYPE = GetResponse] [size]
  02 01 00 - Request ID
  02 01 00 - Error
  02 01 00 - Error Index
  30 24 - [VARBIND LIST] [size]
  30 22 - sequence
  06 0b 2b 06 01 02 01 2b 05 01 01 11 01 - OID
  04 13 37 34 36 33 36 43 36 36 30 32 30 4d 43 2d 31 33 30 2d 30
  */
  stream = buffer + 7 + pdu->community_length;
  // pdu.type
  pdu->type = *stream;
  stream += 2;
  // request_id
  stream += 3;
  // error
  stream += 2; // 0x02 0x01
  pdu->error = ber_read_u64(&stream, 1);
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
