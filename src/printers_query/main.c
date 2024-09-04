#include <sdk/taskmanager.h>
#include <sdk/net.h>
#include <sdk/tcp.h>
#include <sdk/snmp.h>
#include <sdk/memory.h>
#include <sdk/snmp.h>
#include <sdk/snowflake.h>
#include <sdk/unity.h>

typedef struct printers_query_t printers_query_t;
typedef struct printers_query_t {
  snmp_t snmp;
  u32 ip4;
  u32 ip4_end;
} printers_query_t;

void printers_query_onmessage(snmp_message_t* this) {
  ip4_string_t ip4_cstr;
  ip4_to_cstr(this->udp_message->address.ip4, ip4_cstr);
  console_write("%s", ip4_cstr);
  queue_foreach(varbind_t, &this->pdu.varbinds, it) {
    console_write_cstr(" - ");
    switch (it->value.type) {
      case ASN1_TYPE_OCTETSTRING:
        console_write_str(it->value.cstr, it->value.size);
        break;
      case ASN1_TYPE_COUNTER: {
        console_log("ASN1_TYPE_COUNTER[%d]=%u", it->value.size, it->value.u64);
      } break;
      case ASN1_TYPE_INTEGER: {
        console_log("ASN1_TYPE_INTEGER[%d]=%u", it->value.size, it->value.i64);
      } break;
      case ASN1_TYPE_OID: {
        console_write_buffer(it->value.cstr, it->value.size);
      } break;
      default:
        console_write("%x[%d]=", it->value.type, it->value.size);
        console_write_buffer(it->value.cstr, it->value.size);
    }
  }
  console_newline();
}
void printers_query_deconstructor(printers_query_t* this) {
  snmp_deconstructor(&this->snmp);
  memory_free(this);
}
void printers_query_service(printers_query_t* this) {
  if (this->ip4 == this->ip4_end) {
    if (queue_is_empty(&this->snmp.pending)) {
      return printers_query_deconstructor(this);
    }
  } else {
    taskmanager_t* taskmanager = this->snmp.udp._service.taskmanager;
    if (taskmanager->tasks_count < TASKS_LOOP_MAX) {
      byte_t community [] = "public";
      snmp_pdu_t pdu = {
        .version = SNMP_VERSION_1,
        .community = community,
        .community_length = sizeof(community) - 1,
        .type = PDU_TYPE_GET_REQUEST,
      };
      snmp_pdu_constructor(&pdu);
      const u8 serial_oid [] = { OID_PREFIX, 6, 1, 2, 1, 43, 5, 1, 1, 17, 1 };
      varbind_t serial_varbind = varbind_from_const_u8(serial_oid);
      queue_push(&pdu.varbinds, &serial_varbind.queue);
      const u8 brand_oid [] = { OID_PREFIX, 6, 1, 2, 1, 43, 8, 2, 1, 14, 1, 1 };
      varbind_t brand_varbind = varbind_from_const_u8(brand_oid);
      queue_push(&pdu.varbinds, &brand_varbind.queue);
      const u8 model_oid [] = { OID_PREFIX, 6, 1, 2, 1, 25, 3, 2, 1, 3, 1 };
      varbind_t model_varbind = varbind_from_const_u8(model_oid);
      queue_push(&pdu.varbinds, &model_varbind.queue);
      do {
        console_log(
          "%d.%d.%d.%d",
          (u8)this->ip4,
          *((u8*)&this->ip4 + 1),
          *((u8*)&this->ip4 + 2),
          *((u8*)&this->ip4 + 3)
        );
        pdu.request_id = snowflake_uid();
        udp_send_t* udp_send = udp_send_new(&this->snmp.udp);
        udp_send->address.ip4 = this->ip4;
        udp_send->address.net_port = SNMP_PORT;
        udp_send->data = snmp_pdu_to_buffer(&pdu);
        udp_send->length = buffer_length(udp_send->data);
        snmp_request_await(&this->snmp, pdu.request_id);
        if (this->ip4 == this->ip4_end)
          break;
        this->ip4 = ip4_increment(this->ip4);
      } while (taskmanager->tasks_count < TASKS_LOOP_MAX);
    }
  }
  snmp_service(&this->snmp);
}
void printers_query_constructor(taskmanager_t* taskmanager, u32 ip4_start, u32 ip4_end) {
  if (!ip4_lessequal(ip4_start, ip4_end))
    return;
  printers_query_t* this = memory_alloc(sizeof(printers_query_t));
  this->ip4 = ip4_start;
  this->ip4_end = ip4_end;
  // snmp
  snmp_constructor(&this->snmp, taskmanager);
  this->snmp.timeout = 1000;
  this->snmp.onmessage = printers_query_onmessage;
  this->snmp.udp._service.context = this;
  this->snmp.udp._service.handle = (function_t)printers_query_service;
  this->snmp.udp._service.destroy = (function_t)printers_query_deconstructor;
}

i32 main(i32 argc, char** argv) {
  console_color(ANSI_FORE_LIGHTBLUE);
  if (argc != 3) {
    console_log("exe [ip1] [ip2]");
    console_color(ANSI_RESET);
    return 0;
  }
  taskmanager_t taskmanager;
  taskmanager_constructor(&taskmanager);
  printers_query_constructor(
    &taskmanager, ip4_from_cstr(argv[1]), ip4_from_cstr(argv[2])
  );
  taskmanager_run(&taskmanager);
  console_color(ANSI_FORE_LIGHTGREEN);
  console_write_cstr("SUCCESS\n");
  console_color(ANSI_RESET);
  return 0;
}