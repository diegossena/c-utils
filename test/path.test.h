#include <sdk/sdk.h>

void path_test() {
  string_t* path = path_absolute("./build.bat");
  console_log("path=%s", string_cstr(path));
  console_log("path.length=%d", string_length(path));
  string_free(path);
}