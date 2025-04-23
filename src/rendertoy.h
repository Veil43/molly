#ifndef RENDERTOY_H
#define RENDERTOY_H

// Here we have the API for communicatin with rendertoy
#include "types.h"

void renderToyOnStartupCall(f32 aspect_ratio);
void renderToyRenderLoop(f32 delta_time);
#endif // RENDERTOY_H