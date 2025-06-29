#ifndef TYPES_H
#define TYPES_H

#if WIN32 || _WIN32 || __WIN32__
#define PLATFORM_WINDOWS
#else
#error "This platform is not supported"
#endif

#if _WIN64 || __x86_64__
#define PLATFORM_X64 1
#else
#define PLATFORM_X32 1
#endif

#define STRINGIZE_KEY(x) #x
#define STRINGIZE(x) STRINGIZE_KEY(x)

#define true 1
#define false 0

#define null ((void*)0)

#define MAX_U8 (u8)255U
#define MIN_U8 (u8)0U
#define MAX_I8 (i8)(-127)
#define MIN_I8 (i8)-128
#define MAX_I16 32767
#define MIN_I16 -32768
#define MAX_U16 65535U
#define MAX_I32 2147483647L
#define MIN_I32 (-MAX_I32 - 1)
#define MAX_U32 4294967295UL
#define MAX_U64 18446744073709551615ULL
#define MAX_I64 9223372036854775807LL
#define MIN_I64 (-MAX_I64 - 1)
#define MAX_F32 3.40282e+38F
#define MIN_F32 -MAX_F32

typedef _Bool bool;
typedef short unsigned int wchar_t;

typedef signed char i8; // -128 ~ 127
typedef signed short i16; // -32768 ~ 32767
typedef signed int i32; // -2147483648 ~ 2147483647
typedef signed long long i64; // -9223372036854775808 ~ 9223372036854775807

typedef unsigned char u8; // 0 ~ 255
typedef unsigned short u16; // 0 ~ 65535
typedef unsigned int u32; // 0 ~ 4294967295
typedef unsigned long long u64; // 0 ~ 18446744073709551615

typedef float f32; // -3.40282e+38 ~ 3.40282e+38
typedef double f64; // -1.79769e+308 ~ 1.79769e+308

typedef void (*function_t)();

typedef struct rgba_t {
  f32 r, g, b, a;
} rgba_t;

#endif