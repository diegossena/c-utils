#include <sdk/asn1.h>

export char* ber_sequence_start(char** stream, u8 type) {
  *(*stream)++ = type;
  return (*stream)++;
}
export void ber_sequence_end(char** stream, char* sequence) {
  *sequence = (*stream) - sequence - sizeof(char);
}
export void ber_write_i64(char** stream, i64 value) {
  if (value <= 0xFF) {
    *(*stream)++ = 0x1;
  } else if (value <= 0xFFFF) {
    *(*stream)++ = 0x2;
  } else if (value <= 0xFFFFFF) {
    *(*stream)++ = 0x3;
  } else if (value <= 0xFFFFFFFF) {
    *(*stream)++ = 0x4;
  }
  do {
    char byte = value & 0xFF;
    *(*stream)++ = byte;
    value >>= 8;
  } while (value);
}
export void ber_write_var_integer(char** stream, u32 value) {
  *(*stream)++ = ASN1_TYPE_INTEGER;
  ber_write_i64(stream, value);
}
export void ber_write_str(char** stream, const char* string, u8 length) {
  *(*stream)++ = ASN1_TYPE_OCTETSTRING;
  *(*stream)++ = length;
  memory_copy((*stream), string, length);
  (*stream) += length;
}

export void ber_write_oid_null(char** stream, const u8* oid, u64 size) {
  *(*stream)++ = ASN1_TYPE_OID;
  *(*stream)++ = size;
  memory_copy((*stream), oid, size);
  (*stream) += size;
  // value null
  *(*stream)++ = ASN1_TYPE_NULL;
  *(*stream)++ = 0x0;
}

// ber_reader

export u64 ber_read_u64(const char* stream, u8 length) {
  u64 number = 0;
  while (length) {
    number <<= 8;
    number |= *stream++ & 0xFF;
    --length;
  }
  return number;
}
export u64 ber_read_size(char** this) {
  u64 size = *(*this)++;
  if (size > 127) {
    u8 length = size & 0x7F;
    size = ber_read_u64((*this), length);
    (*this) += length;
  }
  return size;
}
export i64 ber_read_i64(const char* stream, u8 length) {
  i64 number = 0;
  while (length) {
    number <<= 8;
    number |= *stream++ & 0xFF;
    --length;
  }
  return number;
}
export ber_field_t ber_read_var(char** stream) {
  ber_field_t ber_value;
  ber_value.type = *(*stream)++;
  if (ber_value.type != ASN1_TYPE_NULL) {
    ber_value.size = ber_read_size(stream);
  }
  switch (ber_value.type) {
    case ASN1_TYPE_BOOLEAN:
      ber_value.u64 = **stream;
      (*stream) += ber_value.size;
      break;
    case ASN1_TYPE_INTEGER:
      ber_value.i64 = ber_read_i64(*stream, ber_value.size);
      (*stream) += ber_value.size;
      break;
    case ASN1_TYPE_COUNTER64:
    case ASN1_TYPE_COUNTER:
      ber_value.u64 = ber_read_u64(*stream, ber_value.size);
      (*stream) += ber_value.size;
      break;
    case ASN1_TYPE_OID:
    case ASN1_TYPE_OCTETSTRING:
      ber_value.cstr = *stream;
      (*stream) += ber_value.size;
      break;
    default:
      ber_value.cstr = *stream;
    // case ASN1_TYPE_BITSTRING:
    //   break;
    // case ASN1_TYPE_NULL:
    //   break;
  }
  return ber_value;
}
export varbind_t ber_read_oid(char** stream) {
  ber_field_t varbind_sequence = ber_read_var(stream);
  char* varbind_sequence_end = (char*)varbind_sequence.cstr + varbind_sequence.size;
  ++(*stream);
  varbind_t varbind = { .oid_length = *(*stream)++, .oid = (u8*)(*stream) };
  (*stream) += varbind.oid_length;
  if ((*stream) < varbind_sequence_end) {
    varbind.value = ber_read_var(stream);
  }
  return varbind;
}