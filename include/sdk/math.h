#pragma once
#include <sdk/types.h>

u64 math_next2pow(u64 number);

u64 math_jenkins_hash(const byte* data, u64 size);