#include <sdk/asn1.h>

export u8* ber_sequence_start(u8** stream, u8 type) {
  *(*stream)++ = type;
  return (*stream)++;
}
export void ber_sequence_end(u8** stream, u8* sequence) {
  *sequence = (*stream) - sequence - sizeof(u8);
}
export void ber_write_i64(u8** stream, i64 value) {
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
    u8 byte = value & 0xFF;
    *(*stream)++ = byte;
    value >>= 8;
  } while (value);
}
export void ber_write_var_integer(u8** stream, u32 value) {
  *(*stream)++ = ASN1_TYPE_INTEGER;
  ber_write_i64(stream, value);
}
export void ber_write_str(u8** stream, const char* string, u8 length) {
  *(*stream)++ = ASN1_TYPE_OCTETSTRING;
  *(*stream)++ = length;
  memory_copy((*stream), string, length);
  (*stream) += length;
}

export void ber_write_oid_null(u8** stream, const u8* oid, u64 size) {
  *(*stream)++ = ASN1_TYPE_OID;
  *(*stream)++ = size;
  memory_copy((*stream), oid, size);
  (*stream) += size;
  // value null
  *(*stream)++ = ASN1_TYPE_NULL;
  *(*stream)++ = 0x0;
}

// ber_reader

export u64 ber_read_u64(u8** stream, u8 length) {
  u64 number = 0;
  while (length) {
    number <<= 8;
    number |= *(*stream)++ & 0xFF;
    --length;
  }
  return number;
}
export u64 ber_read_size(u8** this) {
  u64 size = *(*this)++;
  if (size > 127) {
    u8 length = size & 0x7F;
    size = ber_read_u64(this, length);
    (*this) += length;
  }
  return size;
}
export i64 ber_read_i64(u8** stream, u8 length) {
  i64 number = 0;
  while (length) {
    number <<= 8;
    number |= *(*stream)++ & 0xFF;
    --length;
  }
  return number;
}