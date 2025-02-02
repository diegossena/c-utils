#ifndef SNMP_H
#define SNMP_H

#include <sdk/asn1.h>
#include <sdk/udp.h>
#include <sdk/string.h>

#define SNMP_PORT 161
#define OID_MAX_SIZE 128
#define OID_PREFIX 0x2b // 1.3
#define SYSDESCR_OID { OID_PREFIX, 6, 1, 2, 1, 1, 2, 0 }

typedef u32 oid_t;

typedef enum snmp_version_t {
  SNMP_VERSION_1
} snmp_version_t;

typedef enum pdu_type_t {
  PDU_TYPE_GET_REQUEST = 0xA0,
  PDU_TYPE_GET_NEXT_REQUEST,
  PDU_TYPE_GET_RESPONSE,
  PDU_TYPE_SET_REQUEST,
} pdu_type_t;
/**
 * Protocol Data Unit
 */
typedef struct pdu_t {
  snmp_version_t version;
  const char* community;
  u64 community_length;
  // pdu
  pdu_type_t type;
  u32 request_id;
  u8 error, error_index;
  // varbind_t
  varbind_t* varbinds;
  u64 varbinds_length;
} pdu_t;

typedef enum snmp_error_t {
  SNMP_ERR_NOERROR,
  SNMP_ERR_TOOBIG,
  SNMP_ERR_NOSUCHNAME,
  SNMP_ERR_BADVALUE,
  SNMP_ERR_READONLY,
  SNMP_ERR_GENERALERROR,
  SNMP_ERR_NOACCESS,
  SNMP_ERR_WRONGTYPE,
  SNMP_ERR_WRONGLENGTH,
  SNMP_ERR_WRONTENCONDING,
  SNMP_ERR_WRONGVALUE,
  SNMP_ERR_NOCREATION,
  SNMP_ERR_INCONSISTENTVALUE,
  SNMP_ERR_RESOURCEUNAVAILABLE,
  SNMP_ERR_COMMITVALUE,
  SNMP_ERR_UNDOFAILED,
  SNMP_ERR_AUTHORIZATIONERROR,
  SNMP_ERR_NOTWRITABLE,
  SNMP_ERR_INCONSISTENTNAME,
} snmp_error_t;

export error_t snmp_pdu_to_buffer(pdu_t* this, u8* target, u64 size);

export error_t snmp_request(udp_t, pdu_t*, ip4_t host);

#endif