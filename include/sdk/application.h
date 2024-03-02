#pragma once
#include <sdk/types.h>

interface app_queue app_queue;
interface handle handle;

class application application;

extern application application_main;

void app_inicialize();

// run main loop and free on exit
void app_run();