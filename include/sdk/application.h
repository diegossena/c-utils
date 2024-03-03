#pragma once
#include <sdk/types.h>

interface app_queue app_queue;
interface handle handle;

class application application;

// run main loop and free on exit
void app_run();

void app_inicialize();
void app_shutdown();