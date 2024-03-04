#include "sdk/date.h"
#include "sdk/error.h"

#include <sys/time.h>

u64 date_now() {
  struct timeval ts;
  gettimeofday(&ts, 0);
  return ((u64)ts.tv_sec * 1000) + ts.tv_usec;
}