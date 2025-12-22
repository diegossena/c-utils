#ifdef DEBUG

#include <sdk/types.h>
#include <sdk/console.h>

#define assert(value) { \
  if(value) { \
  } else { \
    console_write("AssertionError: " #value  " ("__FILE__ ":" STRINGIFY(__LINE__)")\n"); \
  } \
}

#else

#define assert(value)

#endif