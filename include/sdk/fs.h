#include <sdk/types.h>

typedef struct {
  u64 dev;
  u64 mode;
  u64 nlink;
  u64 uid;
  u64 gid;
  u64 rdev;
  u64 ino;
  u64 size;
  u64 blksize;
  u64 blocks;
  u64 flags;
  u64 gen;
  u64 atim;
  u64 mtim;
  u64 ctim;
  u64 birthtime;
} fs_stats;

#include <sdk/inl.h>