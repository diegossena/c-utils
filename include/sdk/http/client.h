#pragma once

#include <sdk/types.h>
#include <sdk/http/http.h>
#include <sdk/string.h>
#include <sdk/map.h>

interface http_client_request http_client_request;
/*
map<string, string> headers;
*/
interface http_client_response http_client_response;

http_client_request* http_request_new();
void http_request_free(http_client_request*);

void http_request_method_set(http_client_request*, http_method);
void http_request_host_set(http_client_request*, const char*);

void http_request_fetch(http_client_request*);