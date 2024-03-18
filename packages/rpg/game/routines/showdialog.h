#pragma once

#include "./routines.h"
#include "../entities/entity.h"

typedef struct showdialog_t {
  routine_t routine;
  // splited by '\n' and end with '\0'
  char* lines;
} showdialog_t;

// void title_screen_push(game_context_t* context) {
//   routine_t* this = memory_alloc(sizeof(routine_t));
//   this->context = context;
//   this->update = title_screen_update;
//   queue_push(&context->routines, &this->queue);
// }