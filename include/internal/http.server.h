#pragma once

#include "sdk/types.h"
#include "sdk/http.server.h"

class http_route {
  const char* path;
  http_request_handler get;
  http_request_handler post;
  http_request_handler delete;
  http_request_handler put;
  http_request_handler patch;
} http_route;

class http_server {
  http_request_handler* middlewares;
  http_route* routes;
} http_server;