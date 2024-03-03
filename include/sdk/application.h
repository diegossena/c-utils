#pragma once
#include <sdk/types.h>

interface app_queue app_queue;
interface handle handle;

class application application;

// run main loop and free on exit
i32 app_run();