#pragma once
#include <sdk/types.h>
#include <sdk/stream.h>
#include <sdk/net.tcp.h>

class application application;

// run main loop and free on exit
i32 app_run();