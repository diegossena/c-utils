#include "sdk/snowflake.h"
#include "sdk/date.h"

#include <stdio.h> // TODO: remove this line

#define SNOWFLAKE_MAX_SEQUENCE 1023ULL


u64 snowflake_node = 0;
u64 snowflake_epoch = 1577836800000;
u64 last_timestamp = 0;


u64 snowflake_uid() {
  u64
    current_time = date_now(),
    uid_time = current_time - snowflake_epoch,
    sequence = uid_time == last_timestamp
    ? (sequence + 1) & SNOWFLAKE_MAX_SEQUENCE
    : 0;

  last_timestamp = uid_time;

  return uid_time << 22 | snowflake_node << 10 | sequence;
}