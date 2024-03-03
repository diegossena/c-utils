#pragma once

#include "sdk/types.h"

interface queue_t queue_t;

interface queue_t {
  queue_t* next;
} queue_t;