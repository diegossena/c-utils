#ifndef NET_PROMISE_H
#define NET_PROMISE_H

#include <sdk/types.h>
#include <sdk/task.h>
#include <sdk/net.h>
#include <sdk/promise.h>

export void _socket_cancel(u64 fd);
export u64 socket_promise_new(task_t*, socket_type_t);


#endif