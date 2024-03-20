#include <sdk/process.h>

void process_test() {
  string_t execpath = {};
  console_log("process_test");
  process_execpath(&execpath);
  console_log("!process_test");
  path_join_cstr(&execpath, "..");
  console_log("execpath='%s'", string_cstr(&execpath));
  path_join_cstr(&execpath, "vs_2d_paint.cso");
  console_log("execpath='%s'", string_cstr(&execpath));
  path_resolve(&execpath);
  console_log("execpath='%s'", string_cstr(&execpath));
  string_deconstructor(&execpath);
}