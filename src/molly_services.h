#ifndef MOLLY_SERVICES_H
#define MOLLY_SERVICES_H

#include "types.h"
#include "platform_services.h"

void molly_on_startup_call(f32 aspect_ratio);
void molly_render_loop(PlatformInput keyboard_input);
void molly_mouse_scroll(f32 yoffset);

#endif