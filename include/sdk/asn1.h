#ifndef SDK_ASN1_H
#define SDK_ASN1_H

#include <sdk/types.h>
#include <sdk/memory.h>
#include <sdk/buffer.h>

#define asn1_int2(number) number >> 7 | 0x80, number & 0x7F
#define varbind_from_const_u8(value) { .oid = (byte_t*)serial_oid, .oid_length = sizeof(serial_oid) }

typedef enum asn1_type_t {
  ASN1_TYPE_BOOLEAN = 0x1,
  ASN1_TYPE_INTEGER,
  ASN1_TYPE_BITSTRING,
  ASN1_TYPE_OCTETSTRING,
  ASN1_TYPE_NULL,
  ASN1_TYPE_OID,
  ASN1_TYPE_SEQUENCE = 0x30,
  ASN1_TYPE_IPADDRESS = 64,
  ASN1_TYPE_COUNTER,
  ASN1_TYPE_GAUGE,
  ASN1_TYPE_TIMETICKS,
  ASN1_TYPE_OPAQUE,
  ASN1_TYPE_COUNTER64 = 70,
  ASN1_TYPE_NOSUCHOBJECT = 128,
  ASN1_TYPE_NOSUCHINSTANCE,
  ASN1_TYPE_ENDOFMIBVIEW
} asn1_type_t;

typedef struct ber_field_t {
  asn1_type_t type;
  u8 size;
  union {
    i64 i64;
    u64 u64;
    f64 f64;
    const char* cstr;
  };
} ber_field_t;

typedef struct varbind_t {
  queue_t queue;
  char* oid;
  u64 oid_length;
  ber_field_t value;
} varbind_t;

SDK_EXPORT u8* ber_sequence_start(char** stream, u8 type);
SDK_EXPORT void ber_sequence_end(char** stream, u8* sequence);
SDK_EXPORT void ber_write_i64(char** stream, i64 value);
SDK_EXPORT void ber_write_var_integer(char** stream, u32 value);
SDK_EXPORT void ber_write_str(char** stream, const char* string, u8 length);
SDK_EXPORT void ber_write_oid_null(char** stream, char* oid, u64 size);
SDK_EXPORT u64 ber_read_u64(const char* stream, u8 length);
SDK_EXPORT ber_field_t ber_read_var(char** stream);
SDK_EXPORT varbind_t ber_read_oid(char** stream);

#endif