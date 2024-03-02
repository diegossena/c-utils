#pragma once

#include <sdk/types.h>
#include <sdk/string.h>
#include <sdk/map.h>

typedef enum http_method {
  HTTP_GET,
  HTTP_POST,
  HTTP_DELETE,
  HTTP_PATCH,
  HTTP_PUT
} http_method;

interface http_client_request http_client_request;
/*
map<string, string> headers;
*/
interface http_client_response http_client_response;

http_client_request* http_request_new();
void http_request_free(http_client_request*);

void http