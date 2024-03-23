#pragma once

#include "./tilemap.h"

void tilemap_introduction_load(tilemap_t* this) {
  this->width = 64;
  this->height = 15;
  this->layers = (
    "tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt"
    "tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt"
    "tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt"
    "tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt"
    "ttttttttttggggggtttttttttttttttttttttttttttttttttttttttttttttttt"
    "ggggggggggggggggtttttttttttttttttttttttttttttttttttttttttttttttt"
    "ggggggggggggggggtttttttttttttttttttttttttttttttttttttttttttttttt"
    "ttttttgggttttgggtttttttttttttttttttttttttttttttttttttttttttttttt"
    "tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt"
    "tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt"
    "tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt"
    "tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt"
    "tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt"
    "tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt"
    "tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt"
    );
}