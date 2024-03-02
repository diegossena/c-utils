#pragma once

#include "sdk/console.h"

void console_inicialize();

error_code console_write_str(log_level, const char* message, u64 message_length);
