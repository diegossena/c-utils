#include <sdk/console.h>
#include <sdk/unity.h>

i32 main(i32 argc, char** argv) {
  console_color(ANSI_FORE_LIGHTGREEN);
  for (u64 i = 1LLU << 52; i < (1LLU << 63); i++) {
    console_write_bin(i);
    console_write(" ");
    console_write_bin(i);
    console_newline();
  }
  console_color(ANSI_RESET);
  return 0;
}