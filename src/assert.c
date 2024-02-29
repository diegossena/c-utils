#include "sdk/assert.h"

#include <stdio.h> // TODO: replace this library

void _assert(const char* expression, const char* file, u64 line) {
  fprintf(
    stderr,
    "AssertionError [%s]: %s\n at %s:%llu\n",
    expression,
    file,
    line
  );
}