#pragma once

#include "Common/MultiarchIncludes.hpp"

namespace Remote {

enum class preset_ops : uint8_t
{
    NOOP              = 0,
    SETPIXEL          = 1,
    FADEOUT           = 2,
    FADEIN            = 3,
    BRIGHT            = 4,
    SETPIXEL_NOSHOW   = 5,
    SHOW              = 6,
    PRESET            = 7,
};

struct __attribute__((packed)) led_packet
{
    preset_ops special_ops;
    uint16_t pos;
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

};