#include <sdk/snowflake.h>

#define SNOWFLAKE_MAX_SEQUENCE 1023ULL

static u64 __snowflake_node = 0;
static u64 __snowflake_epoch = 1577836800000;
static u64 __snowflake_updated_at = 0;
static u64 __snowflake_sequence = 0;

SDK_EXPORT u64 snowflake_uid() {
  u64
    current_time = date_now(),
    uid_time = current_time - __snowflake_epoch;
  if (uid_time == __snowflake_updated_at) {
    ++__snowflake_sequence & SNOWFLAKE_MAX_SEQUENCE;
  } else {
    __snowflake_sequence = 0;
  }
  __snowflake_updated_at = uid_time;
  return uid_time << 22 | __snowflake_node << 10 | __snowflake_sequence;
}

#undef SNOWFLAKE_MAX_SEQUENCE