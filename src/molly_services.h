#ifndef MOLLY_SERVICES_H
#define MOLLY_SERVICES_H

#include "types.h"
#include "platform_services.h"

void mollyOnStartupCall(f32 aspect_ratio);
void mollyRenderLoop(f64 delta_time, PlatformInput keyboard_input);
void mollyMouseScroll(f32 yoffset);

#endif