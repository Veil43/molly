#ifndef MOLLY_SERVICES_H
#define MOLLY_SERVICES_H

#include "types.h"
#include "platform_services.h"

void mollyToyOnStartupCall(f32 aspect_ratio);
void mollyToyRenderLoop(f64 delta_time, PlatformInput keyboard_input);
void mollyMouseScroll(f32 yoffset);

#endif