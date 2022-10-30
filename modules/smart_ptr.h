#pragma once
#include <stdio.h>
#include <malloc.h>

void free_stack(void* ptr) {
  // union {
  //   void** real_ptr;
  //   void* ptr;
  // } conv;
  // conv.ptr = ptr;
  free(*(void**)ptr);
  // *conv.real_ptr = 0;
}

#define smart __attribute__ ((cleanup(free_stack)))