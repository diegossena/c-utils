#include <sdk/snmp.h>

SDK_EXPORT snmp_t* snmp_new(taskmanager_t* taskmanager) {
  snmp_t* this = memory_alloc0(sizeof(snmp_t));
  _snmp_constructor(this, taskmanager);
  this->__udp._promise.destroy = (function_t)snmp_free;
  return this;
}
SDK_EXPORT void snmp_free(snmp_t* this) {
  _snmp_deconstructor(this);
  memory_free(this);
}
SDK_EXPORT void snmp_pdu_constructor(snmp_pdu_t* this) {
  queue_constructor(&this->varbinds);
}
SDK_EXPORT snmp_request_t* snmp_send(snmp_t* snmp, snmp_pdu_t* pdu) {
  snmp_request_t* this = memory_alloc0(sizeof(snmp_request_t));
  // pdu->bytes
  char bytes[BUFFER_DEFAULT_SIZE];
  char* buffer = bytes;
  u8* sequence = ber_sequence_start(&buffer, ASN1_TYPE_SEQUENCE);
  ber_write_var_integer(&buffer, pdu->version);
  ber_write_str(&buffer, pdu->community, pdu->community_length);
  u8* pdu_sequence = ber_sequence_start(&buffer, pdu->type);
  ber_write_var_integer(&buffer, pdu->request_id);
  ber_write_var_integer(&buffer, pdu->error);
  ber_write_var_integer(&buffer, pdu->error_index);
  u8* varbind_list = ber_sequence_start(&buffer, ASN1_TYPE_SEQUENCE);
  queue_foreach(varbind_t, &pdu->varbinds, it) {
    u8* varbind = ber_sequence_start(&buffer, ASN1_TYPE_SEQUENCE);
    ber_write_oid_null(&buffer, it->oid, it->oid_length);
    ber_sequence_end(&buffer, varbind);
  }
  ber_sequence_end(&buffer, varbind_list);
  ber_sequence_end(&buffer, pdu_sequence);
  ber_sequence_end(&buffer, sequence);
  // buffer
  u16 size = buffer - bytes;
  buffer = buffer_new(size);
  memory_copy(buffer, bytes, size);
  // udp_send
  _udp_send_constructor(&this->send, &snmp->__udp);
  this->send.address.net_port = SNMP_PORT;
  this->send.data = buffer;
  this->send.length = size;
  this->send.callback = (function_t)buffer_free;
  this->send.context = this->send.data;
  // promise
  this->__timer = timer_new((function_t)__snmp_onrequest, this, snmp->timeout, 0);
  return this;
}
SDK_EXPORT void _snmp_constructor(snmp_t* this, taskmanager_t* taskmanager) {
  this->timeout = NET_DEFAULT_TIMEOUT;
  // pending
  queue_constructor(&this->__pending);
  // udp
  udp_constructor(&this->__udp, taskmanager);
  this->__udp.onmessage = __snmp_onmessage;
  this->__udp.context = this;
  udp_bind(&this->__udp, 0);
  // udp._task
  this->__udp._promise.destroy = (function_t)_snmp_deconstructor;
  this->__udp._promise.context = this;
}
SDK_EXPORT void _snmp_deconstructor(snmp_t* this) {
  udp_deconstructor(&this->__udp);
  queue_foreach(snmp_request_t, &this->__pending, it) {
    memory_free(it);
  }
}
SDK_EXPORT void __snmp_onrequest(snmp_request_t* this) {
  timer_clear(this->__timer);
  _udp_send_deconstructor(&this->send);
  queue_remove(&this->queue);
  if (this->onend) {
    this->onend(this->context);
  }
  memory_free(this);
}
SDK_EXPORT void __snmp_onmessage(udp_message_t* udp_message) {
  snmp_t* this = (snmp_t*)udp_message->udp->context;
  snmp_message_t snmp_message;
  snmp_pdu_constructor(&snmp_message.pdu);
  snmp_message.udp_message = udp_message;
  char* ptr = udp_message->data;
  ber_field_t field;
  ber_read_var(&ptr);
  // version
  ptr += 2; // 0x02 0x01
  snmp_message.pdu.version = ber_read_u64(ptr, 1);
  ++ptr;
  // community
  field = ber_read_var(&ptr);
  snmp_message.pdu.community = (char*)field.cstr;
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
  char* varbind_list_end = (char*)varbind_list.cstr + varbind_list.size;
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
