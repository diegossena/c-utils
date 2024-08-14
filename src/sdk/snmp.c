#include <sdk/snmp.h>

SDK_EXPORT void snmp_constructor(snmp_t* this, taskmanager_t* taskmanager) {
  this->timeout = DEFAULT_TIMEOUT;
  hashset_constructor(&this->requests);
  this->updated_at = date_now();
  // udp
  udp_constructor(&this->udp, taskmanager);
  this->udp.onmessage = __snmp_onmessage;
  this->udp.context = this;
  // service
  this->udp.service.handle = (function_t)snmp_service;
  this->udp.service.destroy = (function_t)snmp_deconstructor;
  this->udp.service.context = this;
}
SDK_EXPORT void snmp_deconstructor(snmp_t* this) {
  udp_deconstructor(&this->udp);
  hashset_free(&this->requests);
}
SDK_EXPORT snmp_t* snmp_new(taskmanager_t* taskmanager) {
  snmp_t* this = memory_alloc0(sizeof(snmp_t));
  snmp_constructor(this, taskmanager);
  this->service.destroy = (function_t)snmp_free;
  return this;
}
SDK_EXPORT void snmp_free(snmp_t* this) {
  snmp_deconstructor(this);
  memory_free(this);
}
SDK_EXPORT void snmp_send(snmp_send_t* props) {
  udp_send_t send_props = {
    .udp = &props->snmp->udp,
    .address = props->address,
    .data = props->pdu,
    .callback = __snmp_onwrite,
    .context = props->snmp
  };
  hashset_add(&props->snmp->requests, props->address.ip4);
  props->snmp->updated_at = date_now();
  udp_send(&send_props);
}
SDK_EXPORT void snmp_pdu_constructor(snmp_pdu_t* this) {
  queue_constructor(&this->varbinds);
}
SDK_EXPORT buffer_t snmp_pdu_to_buffer(snmp_pdu_t* this) {
  byte_t* buffer_end = buffer_shared;
  u8* sequence = ber_sequence_start(&buffer_end, ASN1_TYPE_SEQUENCE);
  ber_write_var_integer(&buffer_end, this->version);
  ber_write_str(&buffer_end, this->community.data, this->community.length);
  u8* pdu_sequence = ber_sequence_start(&buffer_end, this->type);
  ber_write_var_integer(&buffer_end, this->request_id);
  ber_write_var_integer(&buffer_end, this->error);
  ber_write_var_integer(&buffer_end, this->error_index);
  u8* varbind_list = ber_sequence_start(&buffer_end, ASN1_TYPE_SEQUENCE);
  queue_foreach(varbind_t, &this->varbinds, it) {
    u8* varbind = ber_sequence_start(&buffer_end, ASN1_TYPE_SEQUENCE);
    ber_write_oid_null(&buffer_end, &it->oid);
    ber_sequence_end(&buffer_end, varbind);
  }
  ber_sequence_end(&buffer_end, varbind_list);
  ber_sequence_end(&buffer_end, pdu_sequence);
  ber_sequence_end(&buffer_end, sequence);
  return (buffer_t) { buffer_shared, buffer_end - buffer_shared };
}
SDK_EXPORT void snmp_service(snmp_t* this) {
  udp_service(&this->udp);
  u64 elapsed = date_now() - this->updated_at;
  if (elapsed > this->timeout) {
    this->udp.service.destroy(this);
  }
}
SDK_EXPORT void __snmp_onmessage(udp_message_t* udp_message) {
  snmp_t* this = (snmp_t*)udp_message->udp->context;
  if (hashset_contains(&this->requests, udp_message->address.ip4)) {
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
        queue_remove(&it->queue);
        memory_free(it);
      }
    }
    hashset_remove(&this->requests, udp_message->address.ip4);
    if (!this->requests.length) {
      snmp_free(this);
    }
  }
}
SDK_EXPORT void __snmp_onwrite(udp_writer_t* this) {
  snmp_t* snmp = (snmp_t*)this->context;
  snmp->updated_at = date_now();
}