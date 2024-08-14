#include <sdk/asn1.h>

SDK_EXPORT u8* ber_sequence_start(byte_t** stream, u8 type) {
  *(*stream)++ = type;
  u8* length_ptr = (*stream)++;
  return length_ptr;
}
SDK_EXPORT void ber_sequence_end(byte_t** stream, u8* sequence) {
  *sequence = (*stream) - sequence - sizeof(u8);
}
SDK_EXPORT void ber_write_i64(byte_t** stream, i64 value) {
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
    byte_t byte = value & 0xFF;
    *(*stream)++ = byte;
    value >>= 8;
  } while (value);
}
SDK_EXPORT void ber_write_var_integer(byte_t** stream, u32 value) {
  *(*stream)++ = ASN1_TYPE_INTEGER;
  ber_write_i64(stream, value);
}
SDK_EXPORT void ber_write_str(byte_t** stream, const char* string, u8 length) {
  *(*stream)++ = ASN1_TYPE_OCTETSTRING;
  *(*stream)++ = length;
  memory_copy((*stream), string, length);
  (*stream) += length;
}

SDK_EXPORT void ber_write_oid_null(byte_t** stream, buffer_t* oid) {
  *(*stream)++ = ASN1_TYPE_OID;
  *(*stream)++ = oid->length;
  memory_copy((*stream), oid->data, oid->length);
  (*stream) += oid->length;
  // value null
  *(*stream)++ = ASN1_TYPE_NULL;
  *(*stream)++ = 0x0;
}

// ber_reader

SDK_EXPORT u64 ber_read_u64(const byte_t* stream, u8 length) {
  u64 number = 0;
  while (length) {
    number <<= 8;
    number |= *stream++ & 0xFF;
    --length;
  }
  return number;
}
SDK_EXPORT u64 ber_read_size(byte_t** this) {
  u64 size = *(*this)++;
  if (size > 127) {
    u8 length = size & 0x7F;
    size = ber_read_u64((*this), length);
    (*this) += length;
  }
  return size;
}
SDK_EXPORT i64 ber_read_i64(const byte_t* stream, u8 length) {
  i64 number = 0;
  while (length) {
    number <<= 8;
    number |= *stream++ & 0xFF;
    --length;
  }
  return number;
}
SDK_EXPORT ber_field_t ber_read_var(byte_t** stream) {
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
SDK_EXPORT varbind_t ber_read_oid(byte_t** stream) {
  ber_field_t varbind_sequence = ber_read_var(stream);
  byte_t* varbind_sequence_end = (byte_t*)varbind_sequence.cstr + varbind_sequence.size;
  ++(*stream);
  varbind_t varbind = { .oid = {.length = *(*stream)++, .data = (*stream) }, };
  (*stream) += varbind.oid.length;
  if ((*stream) < varbind_sequence_end) {
    varbind.value = ber_read_var(stream);
  }
  return varbind;
}