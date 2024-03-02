#pragma once
#include <sdk/types.h>

#define math_min(a, b) (b < a ? b : a)

u64 math_next2pow(u64 number);
u64 math_jenkins_hash(const byte* data, u64 size);
