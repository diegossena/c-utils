#include "sdk/http/client.h"
#include "sdk/net/socket.h"

interface http_client_request {
  const char* host, path;
  u16 port, timeout;
  http_method method;
} http_client_request;
/*
map<string, string> headers;
*/
// interface http_client_response {
//   u16 statusCode;
//   string* statusText;
//   u64 content_length;
//   map headers;

//   u64 __socket;
// } http_client_response;

// void http_request(http_client_request request) {
//   net_socket socket;
//   error_last = socket_new(
//     &socket, (socket_options) {
//     .host = request.host,
//       .port = request.port,
//       .timeout = 0
//   });
// }