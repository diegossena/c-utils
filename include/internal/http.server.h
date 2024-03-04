#pragma once

#include "sdk/types.h"
#include "sdk/http.server.h"

typedef struct http_route {
  const char* path;
  http_request_handler get;
  http_request_handler post;
  http_request_handler delete;
  http_request_handler put;
  http_request_handler patch;
} http_route;

typedef struct http_server {
  http_request_handler* middlewares;
  http_route* routes;
  u64 _socket;
} http_server;