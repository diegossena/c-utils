#ifndef FS_H
#define FS_H

#include <sdk/types.h>
#include <sdk/error.h>

u8* fs_readfile_sync(const char* path, i32* size);
u8* fs_readfilen_sync(const char* path, u32 size);

#endif