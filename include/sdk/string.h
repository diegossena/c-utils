#ifndef SDK_STRING_H
#define SDK_STRING_H

#include <sdk/types.h>

SDK_EXPORT u64 string_length(const char* str);
SDK_EXPORT i32 cstr_format(char* stream, const char* format, ...);
SDK_EXPORT i32 cstr_format_va(char* this, u64 size, const char* format, void* va_list_p);

#endif