#ifndef MOLLY_PLATFORM_SERVICES_H
#define MOLLY_PLATFORM_SERVICES_H

#include "types.h"

struct PlatformKey {
    u16 is_down;
    u16 was_down;
};

struct PlatformInput {
    f32 mouse_x;
    f32 mouse_prevx;
    f32 mouse_y;
    f32 mouse_prevy;
    PlatformKey w_key;
    PlatformKey a_key;
    PlatformKey s_key;
    PlatformKey d_key;
    PlatformKey esc_key;
};

f64 platform_get_time();
f64 platform_measure_time_elapsed(bool reset = false);
void platform_disable_mouse_cursor();
void platform_enable_mouse_cursor();
void platform_request_quit();

#endif // MOLLY_PLATFORM_SERVICES_H