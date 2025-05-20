#ifndef HTTP_H
#define HTTP_H

#include <sdk/types.h>

typedef struct http_header_t {
  const char* name;
  const char* value;
} http_header_t;

/**
 * https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Status
 */
typedef enum http_status_t {
  // Successful responses (200 – 299)
  HTTP_OK = 200,
  HTTP_CREATED = 201,
  HTTP_ACCEPTED = 202,
  HTTP_NO_CONTENT = 204,
  // Client error responses (400 – 499)
  HTTP_NOT_FOUND = 404
} http_status_t;

typedef enum http_method_t {
  HTTP_GET, HTTP_POST, HTTP_DELETE, HTTP_PATCH
} http_method_t;

typedef struct http_request_t {
  const char* hostname;
  u16 port;

  u8 method; // http_method_t
  const char* path;

  function_t onconnect;

  http_header_t* headers;
  u8 headers_length;

} http_request_t;

#endif