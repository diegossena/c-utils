#ifndef FS_H
#define FS_H

#include <sdk/types.h>
#include <sdk/error.h>

inline FILE* fs_open(const char* path, const char* mode);
inline void fs_close(FILE* this);
inline i32 fs_read(FILE* this, u8* buffer, u64 size);

i32 fs_size(FILE* this);

#endif