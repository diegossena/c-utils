#pragma once
#include <stdio.h>
#include <errno.h>
#include <stdint.h>

void Error(const char* message, int32_t code) {
  switch (code) {
    case 10049: fprintf(stderr, "Error: WSAEADDRNOTAVAIL\r\n"); break;
    default:
      fprintf(stderr, "Error: %s\r\n", strerror(code));
  }
  fprintf(stderr, "message: \"%s\"\r\n", message);
  fprintf(stderr, "errno: %d\r\n", code);
  exit(code);
}