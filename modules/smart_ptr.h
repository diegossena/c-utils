#pragma once
#include <stdio.h>
#include <malloc.h>

void free_stack(void* ptr) {
  free(*(void**)ptr);
}

#define smart __attribute__ ((cleanup(free_stack)))