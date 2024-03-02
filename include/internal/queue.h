#pragma once

#include "sdk/types.h"

interface queue queue;

interface queue {
  struct queue* next;
  struct queue* prev;
} queue;