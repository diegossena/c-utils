#ifdef SDK_SNOWFLAKE_H

#include <sdk/date.h>

#define SNOWFLAKE_MAX_SEQUENCE 1023ULL


static u64 __snowflake_node = 0;
static u64 __snowflake_epoch = 1577836800000;
static u64 __snowflake_updatedat = 0;


u64 snowflake_uid() {
  u64
    current_time = date_now(),
    uid_time = current_time - __snowflake_epoch,
    sequence = uid_time == __snowflake_updatedat
    ? (sequence + 1) & SNOWFLAKE_MAX_SEQUENCE
    : 0;

  __snowflake_updatedat = uid_time;

  return uid_time << 22 | __snowflake_node << 10 | sequence;
}

#undef SNOWFLAKE_MAX_SEQUENCE

#endif