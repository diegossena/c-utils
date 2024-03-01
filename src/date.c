#include "sdk/date.h"
#include "sdk/error.h"

#include <pthread_time.h>

u64 date_now() {
  struct timespec ts;
  if (__pthread_clock_gettime(CLOCK_REALTIME, &ts) != 0) {
    error("__pthread_clock_gettime", ERR_UNKNOWN);
    return 0;
  }
  return (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
}