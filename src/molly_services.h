#ifndef MOLLY_SERVICES_H
#define MOLLY_SERVICES_H

#include "types.h"
#include "input.h"

void molly_on_startup_call(f32 aspect_ratio);
void molly_render_loop(Input keyboard_input);
void molly_on_shutdown_call();
void molly_mouse_scroll(f32 yoffset);

#endif