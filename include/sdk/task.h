#pragma once

#include <sdk/types.h>

interface task_t task_t;

typedef void (*task_handle)(void* context);

void task_destroy(void*);