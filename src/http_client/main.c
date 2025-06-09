#include <sdk/types.h>
#include <sdk/console.h>
#include <sdk/http.h>
#include <sdk/unity.h>

void onconnect() {}

i32 main(i32 argc, char** argv) {
  const http_header_t headers [] = {
    { "Content-Length", "100" }
  };
  const u8 headers_length = sizeof(headers) / sizeof(http_header_t);
  http_request_t request = {
    .hostname = "www.google.com.br",
    .port = 80,
    .method = HTTP_GET,
    .headers = &headers,
    .headers_length = headers_length,
    .onconnect = onconnect,
  };
  console_log("test");
  return 0;
}