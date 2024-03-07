#include <sdk/sdk.h>

void process_test() {
  string_t* path = process_execpath();
  console_log("execpath='%s'", string_cstr(path));
  string_free(path);
}