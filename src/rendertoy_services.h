#ifndef RENDERTOY_SERVICES_H
#define RENDERTOY_SERVICES_H

#include "types.h"
#include "platform_services.h"

void renderToyOnStartupCall(f32 aspect_ratio);
void renderToyRenderLoop(f64 delta_time, PlatformInput keyboard_input);
void renderToyMouseScroll(f32 yoffset);

#endif