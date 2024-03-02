#pragma once

#include <sdk/types.h>
#include <sdk/net.socket.h>

class net_server net_server;

typedef void (*on_connection)(const net_socket*);

net_server* server_new(on_connection);
void server_listen(u16 port);

void server_free();