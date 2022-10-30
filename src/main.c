#include <stdio.h>
#include "Array_t.h"
#include "String_t.h"

int main(int argc, char** argv) {
  String test = string_new("Hello World");
  // Hello World 1 12345
  printf("test: '%s'\n", string_ptr(test));

  return 0;
}
