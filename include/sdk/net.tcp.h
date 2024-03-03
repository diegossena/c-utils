#pragma once

#include "sdk/application.h"
#include "sdk/stream.h"
#include "sdk/net.h"
#include "sdk/handle.h"
#include "sdk/error.h"

/** @brief Represents a TCP stream or TCP server.
 *
 */
class net_tcp net_tcp;

/** @brief Externally-defined function to create a net_tcp, provided by the consumer
 * of this library.
 * @param this A pointer which holds the created net_tcp object.
 * @returns True on successful creation; otherwise false.
 */
net_tcp* net_tcp_new();
/** @brief function to create a net_tcp object
 * @param this A pointer which holds the created net_tcp object as provided.
 */
void net_tcp_free(net_tcp*);

void net_tcp_on_connect(net_tcp*, const net_connect_callback);

error_code net_tcp_connect(net_tcp*, net_connect_opt*);
error_code net_tcp_listen(net_tcp*, net_address*);
