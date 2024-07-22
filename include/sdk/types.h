#pragma once

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

typedef u8 byte_t;

typedef void (*callback_t)(void* context);
typedef struct listener_t {
  void* context;
  callback_t callback;
} listener_t;

// inlining

#if defined(__clang__) || defined(__gcc__)
/** @brief Inline qualifier */
#define SDK_INLINE __attribute__((always_inline)) inline

/** @brief No-inline qualifier */
#define SDK_NOINLINE __attribute__((noinline))
#elif defined(_MSC_VER)
/** @brief Inline qualifier */
#define SDK_INLINE __forceinline
/** @brief No-inline qualifier */
#define SDK_NOINLINE __declspec(noinline)
#else
/** @brief Inline qualifier */
#define SDK_INLINE static inline
/** @brief No-inline qualifier */
#define sdk_noinline
#endif

#ifdef SDK_UNITY
#define SDK_EXPORT SDK_INLINE
#else
#define SDK_EXPORT
#endif