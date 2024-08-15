#ifdef SDK_DEVELOPMENT

#include <sdk/types.h>
#include <sdk/console.h>

#define assert(value) { \
  if(value) { \
  } else { \
    console_log("AssertionError: " #value  " ("__FILE__ ":" STRINGIZE(__LINE__)")"); \
  } \
}

#else

#define assert(value)

#endif