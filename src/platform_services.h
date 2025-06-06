#ifndef MOLLY_PLATFORM_SERVICES_H
#define MOLLY_PLATFORM_SERVICES_H

#include "types.h"

f64 platform_get_time();
f64 platform_measure_time_elapsed(bool reset = false);
void platform_disable_mouse_cursor();
void platform_enable_mouse_cursor();
void platform_request_quit();

#endif // MOLLY_PLATFORM_SERVICES_H