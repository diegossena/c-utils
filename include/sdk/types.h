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

typedef void (*fn_any_t)();

#define MAX_I16 32767
#define MIN_I16 -32768
#define MAX_U16 0xffffU
#define MAX_I32 2147483647L
#define MIN_I32 (-MAX_I32 - 1)
#define MAX_U32 0xffffffffUL
#define MAX_U64 0xffffffffffffffffull
#define MAX_I64 9223372036854775807ll
#define MIN_I64 (-MAX_I64 - 1)
#define MAX_F32 3.40282e+38F
#define MIN_F32 -MAX_F32

// Inlining
#if defined(__clang__) || defined(__gcc__)
/** @brief Inline qualifier */
#define sdk_inline __attribute__((always_inline)) inline

/** @brief No-inline qualifier */
#define sdk_no_inline __attribute__((noinline))
#elif defined(_MSC_VER)

/** @brief Inline qualifier */
#define sdk_inline __forceinline

/** @brief No-inline qualifier */
#define sdk_no_inline __declspec(noinline)
#else

/** @brief Inline qualifier */
#define sdk_inline static inline

/** @brief No-inline qualifier */
#define sdk_no_inline
#endif