#include <stdio.h>
#include "String_t.h"

int main(int argc, char** argv) {
  String test = string_new("Hello World");

  printf("test: '%s'\n", string_ptr(test));

  return 0;
}