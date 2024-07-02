#include <sdk/types.h>
#ifdef SDK_ERROR_H
#include <src/sdk/error.c>
#endif
#ifdef SDK_MEMORY_H
#include <src/sdk/memory.c>
#include <src/sdk/memory.win32.c>
#endif
#ifdef SDK_BUFFER_H
#include <src/sdk/buffer.c>
#endif
#ifdef SDK_STRING_H
#include <src/sdk/string.c>
#endif
#ifdef SDK_QUEUE_H
#include <src/sdk/queue.c>
#endif
#ifdef SDK_NET_H
#include <src/sdk/net.c>
#include <src/sdk/net.win32.c>
#endif
#ifdef SDK_CONSOLE_H
#include <src/sdk/console.c>
#include <src/sdk/console.win32.c>
#endif
#ifdef SDK_TASKMANAGER_H
#include <src/sdk/taskmanager.c>
#endif