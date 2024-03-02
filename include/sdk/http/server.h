#pragma once

#include <sdk/types.h>

class http_server http_server;
class http_route http_route;

interface http_message http_message;
interface http_server_request http_server_request;
interface http_server_response http_server_response;

typedef void (*http_request_handler)(const http_server_request*, const http_server_response*);

http_route* http_route_new(const char* path);
void http_route_get(http_route*, http_request_handler);
void http_route_post(http_route*, http_request_handler);
void http_route_delete(http_route*, http_request_handler);
void http_route_patch(http_route*, http_request_handler);
void http_route_put(http_route*, http_request_handler);
void http_route_head(http_route*, http_request_handler);

http_server* http_server_new();
void http_server_free(http_server*);
void http_server_use(http_server*, http_request_handler);
void http_server_route(http_server*, http_route*);
void http_server_listen(http_server*, u64 port);
