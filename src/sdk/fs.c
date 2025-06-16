#include <sdk/fs.h>

// FILE, fopen, fclose, fseek, fclose, ftell
#include <stdio.h>

u8* fs_readfile_sync(const char* path, i32* size) {
  FILE* file = fopen(path, "rb");
  u8* bytes = null;
  i32 file_size = 0;
  if (file == null) {
    error(ERR_NOT_FOUND, "fs_readfile_sync fopen");
    return null;
  }
  if (fseek(file, 0, SEEK_END) != 0) {
    error(ERR_UNKNOWN, "fs_readfile_sync fseek SEEK_END");
    goto onclose;
  }
  file_size = ftell(file);
  if (file_size == -1) {
    error(ERR_UNKNOWN, "fs_readfile_sync ftell");
    goto onclose;
  }
  if (fseek(file, 0, SEEK_SET) != 0) {
    error(ERR_UNKNOWN, "fs_readfile_sync ftell SEEK_SET");
    goto onclose;
  }
  bytes = memory_alloc(file_size);
  if (bytes == null) {
    error(ERR_NOT_ENOUGH_MEMORY, "fs_readfile_sync memory_alloc");
    goto onclose;
  }
  i32 readed = fread(bytes, sizeof(u8), file_size, file);
  if (readed != file_size) {
    error(ERR_UNKNOWN, "fs_readfile_sync fread %d", readed);
    memory_free(bytes);
    bytes = 0;
    file_size = 0;
  }
onclose:
  fclose(file);
  *size = file_size;
  return bytes;
}

u8* fs_readfilen_sync(const char* path, u32 size) {
  FILE* file = fopen(path, "rb");
  if (file == null)
    return null;
  u8* bytes = memory_alloc(size);
  if (bytes == null) {
    goto onclose;
  }
  i32 readed = fread(bytes, sizeof(u8), size, file);
  if (readed != size) {
    memory_free(bytes);
    bytes = null;
  }
onclose:
  fclose(file);
  return bytes;
}