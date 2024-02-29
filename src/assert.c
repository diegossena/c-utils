#include "sdk/assert.h"
#include "sdk/console.h"

void _assert(const char* expression, const char* file, u64 line) {
  console_error(
    "AssertionError [%s]: %s\n at %s:%llu\n",
    expression, file, line
  );
}