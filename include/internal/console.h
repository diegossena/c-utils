#pragma once

#include "sdk/console.h"

extern bool console_has_ansi;

error_code console_write_str(log_level, const char* message, u64 message_length);
