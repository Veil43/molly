#pragma once

#include "types.h"

enum class eMovement {
    kForward,
    kBackward,
    kLeft,
    kRight
};

enum class eKeyCode {
    kW,
    kA,
    kS,
    kD,
    kESC,
    kLCTRL,
};

struct InputKey {
    u16 is_down;
    u16 was_down;
};

struct Input {
    f32 mouse_x;
    f32 mouse_prevx;
    f32 mouse_y;
    f32 mouse_prevy;
    InputKey w_key;
    InputKey a_key;
    InputKey s_key;
    InputKey d_key;
    InputKey esc_key;
    InputKey lctrl_key;
    InputKey rmb;
    InputKey lmb;
};