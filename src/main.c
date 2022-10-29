#include <stdio.h>
#include "string_t.h"

int main(int argc, char** argv) {

  string_t test = string_empty;

  printf("test: '%s'\n", string_ptr(test));

  return 0;
}