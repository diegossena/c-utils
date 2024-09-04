#include <sdk/snmp.h>

SDK_EXPORT void snmp_constructor(snmp_t* this, taskmanager_t* taskmanager) {
  this->timeout = NET_DEFAULT_TIMEOUT;
  // pending
  queue_constructor(&this->pending);
  // udp
  udp_constructor(&this->udp, taskmanager);
  this->udp.onmessage = snmp_onmessage;
  this->udp.context = this;
  udp_bind(&this->udp, 0);
  // service
  this->udp._service.handle = (function_t)snmp_service;
  this->udp._service.destroy = (function_t)snmp_deconstructor;
  this->udp._service.context = this;
}
SDK_EXPORT void snmp_deconstructor(snmp_t* this) {
  udp_deconstructor(&this->udp);
  queue_foreach(snmp_request_t, &this->pending, it) {
    memory_free(it);
  }
}
SDK_EXPORT snmp_t* snmp_new(taskmanager_t* taskmanager) {
  snmp_t* this = memory_alloc0(sizeof(snmp_t));
  snmp_constructor(this, taskmanager);
  this->udp._service.destroy = (function_t)snmp_free;
  return this;
}
SDK_EXPORT void snmp_free(snmp_t* this) {
  snmp_deconstructor(this);
  memory_free(this);
}
SDK_EXPORT void snmp_service(snmp_t* this) {
  queue_foreach(snmp_request_t, &this->pending, it) {
    u64 delta = date_now() - it->updatedAt;
    if (delta > this->timeout) {
      queue_remove(&it->queue);
      memory_free(it);
    }
  }
  _udp_service(&this->udp);
}
SDK_EXPORT void snmp_onmessage(udp_message_t* udp_message) {
  snmp_t* this = (snmp_t*)udp_message->udp->context;
  snmp_message_t snmp_message;
  snmp_pdu_constructor(&snmp_message.pdu);
  snmp_message.udp_message = udp_message;
  byte_t* ptr = udp_message->data;
  ber_field_t field;
  ber_read_var(&ptr);
  // version
  ptr += 2; // 0x02 0x01
  snmp_message.pdu.version = ber_read_u64(ptr, 1);
  ++ptr;
  // community
  field = ber_read_var(&ptr);
  snmp_message.pdu.community = (byte_t*)field.cstr;
  snmp_message.pdu.community_length = field.size;
  // type
  field = ber_read_var(&ptr);
  snmp_message.pdu.type = field.type;
  // request_id
  ptr += 2;
  snmp_message.pdu.request_id = ber_read_u64(ptr, 1);
  ++ptr;
  if (!snmp_request_resolve(this, snmp_message.pdu.request_id))
    return;
  // error
  ptr += 2;
  snmp_message.pdu.error = ber_read_u64(ptr, 1);
  ++ptr;
  // error_index
  ptr += 2;
  snmp_message.pdu.error_index = ber_read_u64(ptr, 1);
  ++ptr;
  // varbind_list
  ber_field_t varbind_list = ber_read_var(&ptr);
  byte_t* varbind_list_end = (byte_t*)varbind_list.cstr + varbind_list.size;
  while (ptr < varbind_list_end) {
    varbind_t* varbind = memory_alloc(sizeof(varbind_t));
    *varbind = ber_read_oid(&ptr);
    queue_push(&snmp_message.pdu.varbinds, &varbind->queue);
  }
  if (this->onmessage) {
    this->onmessage(&snmp_message);
  }
  queue_foreach(varbind_t, &snmp_message.pdu.varbinds, it) {
    memory_free(it);
  }
}

SDK_EXPORT void snmp_request_await(snmp_t* this, u64 request_id) {
  snmp_request_t* request = memory_alloc(sizeof(snmp_request_t));
  request->id = request_id;
  request->updatedAt = date_now();
  queue_push(&this->pending, &request->queue);
}
SDK_EXPORT bool snmp_request_resolve(snmp_t* this, u64 request_id) {
  queue_foreach(snmp_request_t, &this->pending, it) {
    if (it->id == request_id) {
      queue_remove(&it->queue);
      memory_free(it);
      return true;
    }
    return false;
  }
}

SDK_EXPORT void snmp_pdu_constructor(snmp_pdu_t* this) {
  queue_constructor(&this->varbinds);
}
SDK_EXPORT byte_t* snmp_pdu_to_buffer(snmp_pdu_t* this) {
  byte_t* buffer = buffer_new(BUFFER_DEFAULT_SIZE);
  byte_t* ptr = buffer;
  u8* sequence = ber_sequence_start(&ptr, ASN1_TYPE_SEQUENCE);
  ber_write_var_integer(&ptr, this->version);
  ber_write_str(&ptr, this->community, this->community_length);
  u8* pdu_sequence = ber_sequence_start(&ptr, this->type);
  ber_write_var_integer(&ptr, this->request_id);
  ber_write_var_integer(&ptr, this->error);
  ber_write_var_integer(&ptr, this->error_index);
  u8* varbind_list = ber_sequence_start(&ptr, ASN1_TYPE_SEQUENCE);
  queue_foreach(varbind_t, &this->varbinds, it) {
    u8* varbind = ber_sequence_start(&ptr, ASN1_TYPE_SEQUENCE);
    ber_write_oid_null(&ptr, it->oid, it->oid_length);
    ber_sequence_end(&ptr, varbind);
  }
  ber_sequence_end(&ptr, varbind_list);
  ber_sequence_end(&ptr, pdu_sequence);
  ber_sequence_end(&ptr, sequence);
  u64 size = ptr - buffer;
  buffer = buffer_resize(buffer, size);
  return buffer;
}