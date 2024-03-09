#pragma once


#define STRINGIZE_KEY(x) #x
#define STRINGIZE(x) STRINGIZE_KEY(x)

#define null 0

#define true 1
#define false 0

typedef _Bool bool;

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

typedef u8 byte;

typedef u16 wchar_t;

#define MAX_U64 (((u64)-1) / sizeof(u64))
#define MAX_I64 (((i64)-1) / sizeof(i64))
#define MAX_I32 2147483647
#define MIN_I32 (-2147483647 - 1)
#define MAX_I16 32767
#define MIN_I16 -32768

#define PATH_MAX 260

#ifdef SDK_LIB
#define export __declspec(dllexport)
#else
#define export __declspec(dllimport)
#endif