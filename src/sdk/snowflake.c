#include <sdk/snowflake.h>

#define SNOWFLAKE_MAX_SEQUENCE 1023ULL
#define SNOWFLAKE_EPOCH 1577836800000ULL
#define SNOWFLAKE_NODE 0ULL

u64 __snowflake_updated_at = 0;
u64 __snowflake_sequence = 0;

export u64 snowflake_uid() {
  u64 uid_time = date_now() - SNOWFLAKE_EPOCH;
  if (uid_time == __snowflake_updated_at) {
    ++__snowflake_sequence & SNOWFLAKE_MAX_SEQUENCE;
  } else {
    __snowflake_sequence = 0;
  }
  __snowflake_updated_at = uid_time;
  return uid_time << 22 | SNOWFLAKE_NODE << 10 | __snowflake_sequence;
}

#undef SNOWFLAKE_EPOCH
#undef SNOWFLAKE_NODE
#undef SNOWFLAKE_MAX_SEQUENCE