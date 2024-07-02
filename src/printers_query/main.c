#include <sdk/taskmanager.h>
#include <sdk/net.h>
#include <sdk/snmp.h>
#include <sdk/unity.h>

i32 main(i32 argc, char** argv) {
  taskmanager_t taskmanager;
  taskmanager_constructor(&taskmanager);
  console_color(ANSI_FORE_LIGHTBLUE);
  console_log("TEST");
  console_color(ANSI_RESET);
  taskmanager_run(&taskmanager);
  return 0;
}