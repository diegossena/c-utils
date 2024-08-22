#include <sdk/snmp.h>

SDK_EXPORT void snmp_constructor(snmp_t* this, taskmanager_t* taskmanager) {
  hashset_constructor(&this->pending_requests);
  // udp
  udp_constructor(&this->udp, taskmanager);
  this->udp.onmessage = snmp_onmessage;
  this->udp.context = this;
  // service
  this->udp._service.handle = (function_t)snmp_service;
  this->udp._service.destroy = (function_t)snmp_deconstructor;
  this->udp._service.context = this;
}
SDK_EXPORT void snmp_deconstructor(snmp_t* this) {
  udp_deconstructor(&this->udp);
  hashset_deconstructor(&this->pending_requests);
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
  udp_service(&this->udp);
}
SDK_EXPORT void snmp_onmessage(udp_message_t* udp_message) {
  snmp_t* this = (snmp_t*)udp_message->udp->context;
  if (hashset_contains(&this->pending_requests, udp_message->address.ip4)) {
    if (this->onmessage) {
      snmp_message_t snmp_message;
      snmp_message.udp_message = udp_message;
      byte_t* buffer = udp_message->data;
      snmp_pdu_constructor(&snmp_message.pdu);
      ber_field_t field;
      ber_read_var(&buffer);
      // version
      buffer += 2; // 0x02 0x01
      snmp_message.pdu.version = ber_read_u64(buffer, 1);
      ++buffer;
      // community
      field = ber_read_var(&buffer);
      snmp_message.pdu.community.data = field.cstr;
      snmp_message.pdu.community.length = field.size;
      // type
      field = ber_read_var(&buffer);
      snmp_message.pdu.type = field.type;
      // request_id
      buffer += 2;
      snmp_message.pdu.request_id = ber_read_u64(buffer, 1);
      ++buffer;
      // error
      buffer += 2;
      snmp_message.pdu.error = ber_read_u64(buffer, 1);
      ++buffer;
      // error_index
      buffer += 2;
      snmp_message.pdu.error_index = ber_read_u64(buffer, 1);
      ++buffer;
      // varbind_list
      ber_field_t varbind_list = ber_read_var(&buffer);
      byte_t* varbind_list_end = (byte_t*)varbind_list.cstr + varbind_list.size;
      while (buffer < varbind_list_end) {
        varbind_t* varbind = memory_alloc(sizeof(varbind_t));
        *varbind = ber_read_oid(&buffer);
        queue_push(&snmp_message.pdu.varbinds, &varbind->queue);
      }
      this->onmessage(&snmp_message);
      queue_foreach(varbind_t, &snmp_message.pdu.varbinds, it) {
        memory_free(it);
      }
    }
  }
}
SDK_EXPORT snmp_send_t* snmp_send_new(udp_t* udp) {
  snmp_send_t* this = memory_alloc(sizeof(snmp_send_t));
  _udp_send_constructor(&this->udp_send, udp);
  hashset_add(&props->snmp->pending_requests, props->ip4);
  props->snmp->updated_at = date_now();
  udp_send(&send_props);
}
SDK_EXPORT void snmp_onwrite(udp_send_t* this) {
  snmp_send_t* writer = (snmp_send_t*)this->context;
  writer->snmp->updated_at = date_now();
  console_log("snmp_onwrite %x", this->callback);
  if (this->callback) {
    this->callback(this->context);
  }
  memory_free(writer);
}

SDK_EXPORT void snmp_pdu_constructor(snmp_pdu_t* this) {
  queue_constructor(&this->varbinds);
}
SDK_EXPORT u64 snmp_pdu_to_buffer(snmp_pdu_t* this, byte_t* stream) {
  byte_t* ptr = stream;
  u8* sequence = ber_sequence_start(&ptr, ASN1_TYPE_SEQUENCE);
  ber_write_var_integer(&ptr, this->version);
  ber_write_str(&ptr, this->community.data, this->community.length);
  u8* pdu_sequence = ber_sequence_start(&ptr, this->type);
  ber_write_var_integer(&ptr, this->request_id);
  ber_write_var_integer(&ptr, this->error);
  ber_write_var_integer(&ptr, this->error_index);
  u8* varbind_list = ber_sequence_start(&ptr, ASN1_TYPE_SEQUENCE);
  queue_foreach(varbind_t, &this->varbinds, it) {
    u8* varbind = ber_sequence_start(&ptr, ASN1_TYPE_SEQUENCE);
    ber_write_oid_null(&ptr, &it->oid);
    ber_sequence_end(&ptr, varbind);
  }
  ber_sequence_end(&ptr, varbind_list);
  ber_sequence_end(&ptr, pdu_sequence);
  ber_sequence_end(&ptr, sequence);
  return ptr - stream;
}