#include "sdk/http/client.h"
#include "sdk/net/socket.h"

#include "internal/memory.h"

interface http_client_request {
  const char* host, * path;
  u16 port, timeout;
  http_method method;
} http_client_request;
/*
map<const char*, const char*> headers;
*/
interface http_client_response {
  u16 statusCode;
  string* statusText;
  u64 content_length;
  map* headers;

  net_socket* socket;
} http_client_response;

http_client_request* http_request_new() {
  http_client_request* this = memory_alloc(sizeof(http_client_request));
  this->method = HTTP_GET;
  this->host = "localhost";
  this->path = "/";
  this->timeout = 0;
}
void http_request_free(http_client_request* this) {
  memory_free(this);
}

void http_request_method_set(http_client_request* this, http_method method) {
  this->method = method;
}