#include <sdk/sdk.h>

void path_test() {
  string_t* path1, * path2;
  path1 = path_resolve("./test");
  path2 = path_join(string_cstr(path1), string_length(path1), "main.c", 6);
  console_log("path1=%s", string_cstr(path1));
  console_log("path2=%s", string_cstr(path2));
  string_free(path1);
  string_free(path2);
}