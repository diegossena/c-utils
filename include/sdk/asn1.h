#ifndef ASN1_H
#define ASN1_H

#include <sdk/types.h>
#include <sdk/memory.h>
#include <sdk/queue.h>
#include <sdk/string.h>

#define asn1_int2(number) number >> 7 | 0x80, number & 0x7F
#define varbind_from_const_u8(value) { .oid = (char*)serial_oid, .oid_length = sizeof(serial_oid) }

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
    bool boolean;
    i64 i64;
    u64 u64;
    f64 f64;
    u8 oid[TINY_SIZE];
    char str[TINY_SIZE + 1];
  };
} ber_field_t;

typedef struct varbind_t {
  const u8* oid;
  u64 oid_length;
  ber_field_t value;
} varbind_t;

u8* ber_sequence_start(u8** stream, u8 type);
void ber_sequence_end(u8** stream, u8* sequence);

void ber_write_i64(u8** stream, i64 value);
void ber_write_var_integer(u8** stream, u32 value);
void ber_write_str(u8** stream, const char* string, u8 length);
void ber_write_oid_null(u8** stream, const u8* oid, u64 size);

u64 ber_read_size(u8** this);
i64 ber_read_i64(u8** stream, u8 length);
u64 ber_read_u64(u8** stream, u8 length);

#endif