#include <sdk/time.h>

#include <sys/time.h>

export u64 time_now() {
  struct timeval ts;
  gettimeofday(&ts, 0);
  return ts.tv_sec * 1000LLU + ts.tv_usec / 1000LLU;
}