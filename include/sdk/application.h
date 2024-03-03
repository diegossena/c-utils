#pragma once
#include <sdk/types.h>

interface app_queue app_queue;
interface app_task app_task;

class application application;

// run main loop and free on exit
i32 app_run();