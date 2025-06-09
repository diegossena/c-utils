#include <sdk/http.h>

void http_request(char** raw, const char* method, const char* path) {
  raw = string_copy(raw, method);
  *raw++ = ' ';
  raw = string_copy(raw, path);
  raw = string_copy(raw, " HTTP/1.1\r\n");
  return raw;
}