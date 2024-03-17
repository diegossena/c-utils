#pragma once

#include "./routines.h"
#include "../entities/entity.h"

typedef struct showdialog_t {
  routine_t routine;
  // splited by '\n' and end with '\0'
  char* lines;
} showdialog_t;