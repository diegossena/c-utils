#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <sdk/types.h>
#include <sdk/net.h>

typedef void (*onconnection_t)(socket_t this);

typedef struct tcp_server_t {
  socket_t socket;
  socket_t accept;
  onconnection_t onconnection;
  char buffer[1024];
} tcp_server_t;

void tcp_listen(tcp_server_t* this, u16 port);
error_t tcp_read(socket_t this, char* target, u64 size);
error_t tcp_send(socket_t this, const char* buffer, u64 size);

#endif