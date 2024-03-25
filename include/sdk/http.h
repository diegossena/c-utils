#pragma once

typedef enum http_method {
  HTTP_GET,
  HTTP_POST,
  HTTP_DELETE,
  HTTP_PATCH,
  HTTP_PUT
} http_method;

#include <sdk/inl.h>