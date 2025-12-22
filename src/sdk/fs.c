#include <sdk/fs.h>

// FILE, fopen, fclose, fseek, fclose, ftell
#include <stdio.h>

/**
 * @param mode 'r' | 'w' | 'rw' | 'rb'
 */
inline FILE* fs_open(const char* path, const char* mode) { return fopen(path, mode); }
inline void fs_close(FILE* this) { fclose(this); }
inline i32 fs_read(FILE* this, u8* buffer, u64 size) { return fread(buffer, sizeof(u8), size, this); }

i32 fs_size(FILE* this) {
  i32 code;
  code = fseek(this, 0, SEEK_END);
  if (code != ERR_SUCCESS)
    goto onerror;
  i32 file_size = ftell(this);
  if (file_size == -1)
    goto onerror;
  code = fseek(this, 0, SEEK_SET);
  if (code != ERR_SUCCESS)
    goto onerror;
  return file_size;
onerror:
  fclose(this);
  return ERR_UNKNOWN;
}