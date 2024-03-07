#include <sdk/sdk.h>

void path_test() {
  string_t* path = string_new();
  string_append_cstr(path, "./test");
  path_join_cstr(path, "main.c");
  path_resolve(path);
  console_log("path='%s'; ", string_cstr(path), string_length(path));
  path_dirname(path);
  console_log("path=%s", string_cstr(path));
  string_free(path);
}