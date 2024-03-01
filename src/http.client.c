#include "sdk/http/client.h"
#include "sdk/net/socket.h"

void http_request(http_client_request request) {
  net_socket socket;
  error_last = socket_new(
    &socket, (socket_options) {
    .host = "google.com.br",
      .port = 80,
      .timeout = 0
  });
}