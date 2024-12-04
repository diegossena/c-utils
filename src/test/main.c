#include <sdk/tcp.h>
#include <sdk/unity.h>

i32 main() {
  if (true) {
    char text[TEXT_SIZE + 1];
    string_format(text, TEXT_SIZE, "test %s", "test");
    console_log("text '%s'", text);
  }
  return 0;
}