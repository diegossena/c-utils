#pragma once

#include <sdk/string.h>

extern const char path_sep;

string_t* path_absolute(const char*);
string_t* path_join(const char*, const char*)