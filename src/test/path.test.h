#include <sdk/path.h>

void path_test() {
  string_t path;
  string_new(&path);
  string_append_cstr(&path, "./test");
  path_join_cstr(&path, "main.c");
  path_resolve(&path);
  path_dirname(&path);
  string_free(&path);
}