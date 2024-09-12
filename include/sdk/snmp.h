#ifndef SDK_SNMP_H
#define SDK_SNMP_H

#include <sdk/asn1.h>
#include <sdk/hashset.h>
#include <sdk/udp.h>

#define SNMP_PORT 161
#define OID_MAX_SIZE 128
#define OID_PREFIX 0x2b // 1.3
#define SYSDESCR_OID { OID_PREFIX, 6, 1, 2, 1, 1, 2, 0 }

typedef u32 oid_t;
typedef struct snmp_message_t snmp_message_t;

typedef void (*snmp_onmessage_t)(snmp_message_t*);

typedef enum snmp_version_t {
  SNMP_VERSION_1,
} snmp_version_t;
typedef enum pdu_type_t {
  PDU_TYPE_GET_REQUEST = 0xA0,
  PDU_TYPE_GET_NEXT_REQUEST,
  PDU_TYPE_GET_RESPONSE,
  PDU_TYPE_SET_REQUEST,
} pdu_type_t;
typedef struct snmp_request_t {
  queue_t queue;
  u64 id;
  u64 updatedAt;
} snmp_request_t;
typedef struct snmp_pdu_t {
  snmp_version_t version;
  byte_t* community;
  u64 community_length;
  // pdu
  pdu_type_t type;
  u32 request_id;
  u8 error, error_index;
  // varbind_t
  queue_t varbinds;
} snmp_pdu_t;
typedef struct snmp_t {
  snmp_onmessage_t onmessage;
  queue_t pending; // queue_t<snmp_request_t>
  udp_t udp;
  u64 timeout;
} snmp_t;
/**
 * snmp: snmp_t*
 * pdu: snmp_pdu_t*
 * address: net_address_t
 */
typedef struct snmp_send_t {
  snmp_t* snmp;
  snmp_pdu_t* pdu;
  net_address_t address;
} snmp_send_t;
typedef struct snmp_message_t {
  snmp_t* snmp;
  snmp_pdu_t pdu;
  udp_message_t* udp_message;
} snmp_message_t;
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

SDK_EXPORT void snmp_constructor(snmp_t* this, taskmanager_t* taskmanager);
SDK_EXPORT void _snmp_deconstructor(snmp_t* this);
SDK_EXPORT snmp_t* snmp_new(taskmanager_t* taskmanager);
SDK_EXPORT void snmp_free(snmp_t* this);
SDK_EXPORT void _snmp_service(snmp_t* this);
SDK_EXPORT void snmp_onmessage(udp_message_t* udp_message);
SDK_EXPORT void snmp_onwrite(udp_send_t* this);
SDK_EXPORT void snmp_request_await(snmp_t* this, u64 request_id);
SDK_EXPORT bool snmp_request_resolve(snmp_t* this, u64 request_id);

SDK_EXPORT void snmp_pdu_constructor(snmp_pdu_t* this);
SDK_EXPORT byte_t* snmp_pdu_to_buffer(snmp_pdu_t* this);

#endif