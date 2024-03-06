#pragma once
#include <sdk/types.h>

typedef struct application application;

void app_inicialize();
// run main loop and free on exit
i32 app_run();