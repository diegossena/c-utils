#include <sdk/taskmanager.h>
#include <sdk/net.h>
#include <sdk/tcp.h>
#include <sdk/snmp.h>
#include <sdk/memory.h>
#include <sdk/snmp.h>
#include <sdk/unity.h>

typedef struct printers_query_t printers_query_t;
typedef struct printers_query_t {
  snmp_t snmp;
  u32 ip4;
  u32 ip4_end;
  u64 tasks_count;
} printers_query_t;

void printers_query_onmessage(snmp_message_t* this) {
  console_write("%s", ip4_to_cstr(this->udp_message->address.ip4));
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
  if (this->tasks_count < TASKS_LOOP_MAX) {
    snmp_send_t send = {
      .snmp = &this->snmp,
      .address = {
        .family = NET_FAMILY_IPV4,
        .net_port = net_port_from_short(161)
      }
    };
    do {
      console_log(
        "%d.%d.%d.%d",
        (u8)this->ip4,
        *((u8*)&this->ip4 + 1),
        *((u8*)&this->ip4 + 2),
        *((u8*)&this->ip4 + 3)
      );
      send.address.ip4 = this->ip4;
      snmp_send(&send);
      if (this->ip4 == this->ip4_end) {
        return printers_query_deconstructor(this);
      }
      this->ip4 = ip4_increment(this->ip4);
      ++this->tasks_count;
    } while (this->tasks_count < TASKS_LOOP_MAX);
  }
  snmp_service(&this->snmp);
}
void printers_query_constructor(taskmanager_t* taskmanager, u32 ip4_start, u32 ip4_end) {
  if (!ip4_lessequal(ip4_start, ip4_end))
    return;
  printers_query_t* this = memory_alloc(sizeof(printers_query_t));
  // props
  this->ip4 = ip4_start;
  this->ip4_end = ip4_end;
  this->tasks_count = 0;
  // snmp
  snmp_constructor(&this->snmp, taskmanager);
  this->snmp.udp.service.context = this;
  this->snmp.udp.service.handle = (function_t)printers_query_service;
  this->snmp.udp.service.destroy = (function_t)printers_query_deconstructor;
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
  console_color(ANSI_RESET);
  return 0;
}