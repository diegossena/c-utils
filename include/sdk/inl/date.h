#pragma once

#include <sdk/date.h>

#include <sys/time.h>

u64 date_now() {
  struct timeval ts;
  gettimeofday(&ts, null);
  return ts.tv_sec * 1000LLU + ts.tv_usec / 1000LLU;
}