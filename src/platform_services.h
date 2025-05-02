#ifndef MOLLY_PLATFORM_SERVICES_H
#define MOLLY_PLATFORM_SERVICES_H

#include "types.h"

#define PLATFORM_DECL_SCROLL_CALLBACK(name) void name(void* window, double xoffset, double yoffset)
typedef PLATFORM_DECL_SCROLL_CALLBACK(PlatformScrollCallbackType);

// input should be on molly's side
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

f64 platformGetTime();
f64 platformMeasureTimeElapsed(bool reset = false);
void platformDisableMouseCursor(void* platformWindow = nullptr);
void platformEnableMouseCursor(void* platformWindow = nullptr);

#endif // MOLLY_PLATFORM_SERVICES_H