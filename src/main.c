#include <stdio.h>
#include "String_t.h"

int main(int argc, char** argv) {
  String test = string_new("Hello World");

  string_append(&test,
    string_ref(" 1,"),
    string_ref(" 2,"),
    string_ref(" 3")
  );
  // Hello World 1 12345
  printf("test: '%s'\n", string_ptr(test));

  return 0;
}
