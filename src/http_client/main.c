#include <sdk/types.h>
#include <sdk/console.h>
#include <sdk/http.h>
#include <sdk/unity.h>


i32 main(i32 argc, char** argv) {
  char request_raw [] =
    "GET / HTTP/1.1\r\n"
    "Host: www.google.com\r\n"
    "\r\n";
  return 0;
}