#include <sdk/taskmanager.h>
#include <sdk/net.h>
#include <sdk/tcp.h>
#include <sdk/snmp.h>
#include <sdk/memory.h>
#include <sdk/udp.h>
#include <sdk/unity.h>

typedef struct printers_query_t printers_query_t;
typedef struct printers_query_t {
  task_t task;
  udp_t udp;
  u32 ip4;
  u32 ip4_end;
  u64 tasks_count;
} printers_query_t;
void printers_query_destroy(printers_query_t* this) {
  queue_remove((queue_t*)this);
  memory_free(this);
}
void printers_query_service(printers_query_t* this) {
  while (this->tasks_count < TASKS_LOOP_MAX) {
    // console_log(
    //   "%d.%d.%d.%d",
    //   (u8)this->ip4,
    //   *((u8*)&this->ip4 + 1),
    //   *((u8*)&this->ip4 + 2),
    //   *((u8*)&this->ip4 + 3)
    // );
    // tcp_t* tcp = tcp_new(&taskmanager);
    // tcp_ip4_connect(tcp, );
    if (this->ip4 == this->ip4_end)
      printers_query_destroy(this);
    this->ip4 = ip4_increment(this->ip4);
    ++this->tasks_count;
  }
}
void printers_query_start(taskmanager_t* taskmanager, u32 ip4_start, u32 ip4_end) {
  if (!ip4_lessequal(ip4_start, ip4_end))
    return;
  printers_query_t* this = memory_alloc(sizeof(printers_query_t));
  this->task.context = this;
  this->task.handle = (callback_t)printers_query_service;
  this->task.destroy = (callback_t)printers_query_destroy;
  this->ip4 = ip4_start;
  this->ip4_end = ip4_end;
  this->tasks_count = 0;
  queue_push(&taskmanager->services, &this->task.queue);
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

  printers_query_start(
    &taskmanager, ip4_from_cstr(argv[1]), ip4_from_cstr(argv[2])
  );

  taskmanager_run(&taskmanager);
  console_color(ANSI_RESET);
  return 0;
}