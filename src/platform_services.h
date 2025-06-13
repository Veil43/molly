#ifndef MOLLY_PLATFORM_SERVICES_H
#define MOLLY_PLATFORM_SERVICES_H

#include "types.h"

f64 platform_get_time();
f64 platform_measure_time_elapsed(bool reset = false);
f32 platform_get_screen_aspect_ratio();
void platform_get_screen_dimensions(i32& width, i32& height, f32& aspect_ratio);
void platform_disable_mouse_cursor();
void platform_enable_mouse_cursor();
void platform_request_quit();

#endif // MOLLY_PLATFORM_SERVICES_H