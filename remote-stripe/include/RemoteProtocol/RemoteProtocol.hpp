#pragma once

#include "Common/MultiarchIncludes.hpp"

namespace Remote {

enum class preset_ops : uint8_t
{
    NOOP              = 0u,
    SETPIXEL          = 1u,
    FADEOUT           = 2u,
    FADEIN            = 3u,
    BRIGHT            = 4u,
    SETPIXEL_NOSHOW   = 5u,
    SHOW              = 6u,
    PRESET            = 7u,
    PRESET_STOP       = 8u,
};

struct __attribute__((packed)) led_packet
{
    preset_ops special_ops;
    uint16_t pos { 0 };
    uint8_t r { 0 };
    uint8_t g { 0 };
    uint8_t b { 0 };
};

};