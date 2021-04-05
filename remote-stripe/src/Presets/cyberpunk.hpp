#pragma once

#include <stdint.h>
#include <FastLED.h>
#include <AsyncDelay.h>

namespace Presets {

class CyberPunk
{
    public:
        void start();
        void stop();

        void run(const uint16_t leds_count, CRGB leds[]);

    private:
        void init_leds(const uint16_t leds_count, CRGB leds[]);
        void reset_internal_state();

        void fade();

        AsyncDelay delay_50ms   { 50u,   AsyncDelay::MILLIS };
        AsyncDelay delay_100ms  { 100u,  AsyncDelay::MILLIS };
        AsyncDelay delay_1000ms { 1000u, AsyncDelay::MILLIS };

        struct internal_state
        {
            enum class cyber_state : uint8_t
            {
                NOTHING         = 0u,
                INIT_LEDS_1     = 1u,
                INIT_LEDS_2     = 2u,
                FADEOUT_INIT    = 3u,
                FADEOUT         = 4u,
                FADEIN_INIT     = 5u,
                FADEIN          = 6u,
            };
            cyber_state current = cyber_state::NOTHING;

            // start color
            uint8_t rs = 96u;
            uint8_t gs = 29u;
            uint8_t bs = 100u;

            // end color
            const uint8_t rf = 255u;
            const uint8_t gf = 165u;
            const uint8_t bf = 0u;

            const uint16_t step = 20;

            uint16_t i = 1u;
            uint16_t p = 0u;
            uint16_t x = 0u;

        } internal_state;
};

extern CyberPunk cp;


} // end namespace