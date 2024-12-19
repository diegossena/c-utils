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

export u64 ber_read_u64(char** stream, u8 length) {
  u64 number = 0;
  while (length) {
    number <<= 8;
    number |= *(*stream)++ & 0xFF;
    --length;
  }
  return number;
}
export u64 ber_read_size(char** this) {
  u64 size = *(*this)++;
  if (size > 127) {
    u8 length = size & 0x7F;
    size = ber_read_u64(this, length);
    (*this) += length;
  }
  return size;
}
export i64 ber_read_i64(char** stream, u8 length) {
  i64 number = 0;
  while (length) {
    number <<= 8;
    number |= *(*stream)++ & 0xFF;
    --length;
  }
  return number;
}