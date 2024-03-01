#pragma once

#include <sdk/types.h>
#include <sdk/string.h>
#include <sdk/map.h>

typedef enum http_method {
  GET,
  POST,
  DELETE,
  PATCH,
  PUT
} http_method;

interface http_client_request {
  u16 port, timeout;
  http_method method;
  const char* path;
} http_client_request;
/*
map<string, string> headers;
*/
interface http_client_response {
  u16 statusCode;
  string statusText;
  u64 content_length;
  map headers;

  u64 __socket;
} http_client_response;

void http_request(http_client_request, http_client_response*);

void http_response_free();