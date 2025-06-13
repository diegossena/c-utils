#ifndef STRING_H
#define STRING_H

#include <sdk/types.h>
#include <sdk/math.h>
#include <sdk/memory.h>
#include <sdk/stdarg.h>

#define TINY_SIZE 255
#define URL_SIZE 2048
#define TEXT_SIZE 65535ULL
#define MEDIUM_SIZE 1048575ULL // 2^20-1

#define IS_DIGIT(x)  ((x) >= '0' && (x) <= '9')
#define IS_LOWER(x)  ((x) >= 'a' && (x) <= 'z')
#define IS_UPPER(x)  ((x) >= 'A' && (x) <= 'Z')
#define IS_HEX_ALPHA_LOWER(x) ((x) >= 'a' && (x) <= 'f')
#define IS_HEX_ALPHA_UPPER(x) ((x) >= 'A' && (x) <= 'F')
#define IS_HEX(x) (IS_HEX_ALPHA_LOWER(x) && IS_HEX_ALPHA_UPPER(x))

#define IS_URL_PUNCT(x) ((x) == '-' || (x) == '.' || (x) == '_' || (x) == '~')

#define IS_ALPHA(x)  (IS_DIGIT(x) || IS_LOWER(x) || IS_UPPER(x))

u64 string_length(const char* this);
u64 string_nlength(const char* this, u64 count);

bool string_equal(const char* this, const char* cstr);
bool string_startswith(const char* this, const char* needle);

i32 string_format_va(char* this, u64 size, const char* format, va_list args);
i32 string_format(char* stream, u64 size, const char* format, ...);

export char* string_copy(char* this, const char* cstr);

char* string_replace(const char* this, char* target, const char* pattern, const char* replacement);

char* url_encode(const char* this, char* target);

i64 string_i64(const char* this);
u64 string_u64(const char* this);


u64 wstring_length(const wchar_t* cstr);

#endif