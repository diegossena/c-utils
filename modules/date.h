#pragma once

#include <stdint.h>
#include <sys/time.h>

uint64_t date_now() {
  struct timeval now;
  gettimeofday(&now, NULL);
  return ((uint64_t)now.tv_sec) * 1000 + now.tv_usec / 1000;
}