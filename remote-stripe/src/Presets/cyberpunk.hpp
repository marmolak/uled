#pragma once

#include <Arduino.h>
#include <FastLED.h>

namespace Presets {

class CyberPunk
{
    public:
        static void run(const uint16_t leds_count, CRGB leds[]);
};


} // end namespace