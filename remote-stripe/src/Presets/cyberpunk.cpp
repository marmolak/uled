/* Basically rewrite of:

    for (uint16_t p = 0, i = 0; p < leds_count; p += step, ++i)
    {
        for (uint16_t x = 0; x < step; ++x)
        {
            const auto pixel = CRGB(rs, gs, bs);
            leds[p + x] = pixel;

            FastLED.show();
            delay(50);
        }

        rs = (rs - ((rs - rf) / step * i));
        gs = (gs - ((gs - gf) / step * i));
        bs = (bs - ((bs - bf) / step * i));
    }

    // stuck forever
    do {
        // light up
        for (uint16_t p = 40; p < 240; ++p) {
            FastLED.setBrightness(p);
            delay(50);
            FastLED.show();
            delay(50);
        }

        delay(1000);

        // fade
        for (uint16_t p = 239; p > 40; --p) {
            FastLED.setBrightness(p);
            delay(50);
            FastLED.show();
            delay(50);
        }

        delay(1000);
    } while (true);

to cooperative multitasking with state machine.
*/

#include <avr/power.h>

#include "Presets/cyberpunk.hpp"
#include "Common/Config/Config.hpp"


namespace Presets {

CyberPunk cp;

/* public */
void CyberPunk::start()
{
    reset_internal_state();

    FastLED.setBrightness(40);
    internal_state.current = internal_state::cyber_state::INIT_LEDS_1;
}

/* private */
void CyberPunk::reset_internal_state()
{
    delay_50ms.expire();
    delay_100ms.expire();
    delay_1000ms.expire();

    internal_state.i = 1u;
    internal_state.p = 0u;
    internal_state.x = 0u;
    internal_state.rs = 96u;
    internal_state.gs = 29u;
    internal_state.bs = 100u;
}

/* private */
void CyberPunk::init_leds(const uint16_t leds_count, CRGB leds[])
{
    uint8_t &rs = internal_state.rs;
    uint8_t &gs = internal_state.gs;
    uint8_t &bs = internal_state.bs;

    // innter loop
    if (internal_state.current == internal_state::cyber_state::INIT_LEDS_1)
    {
        if (!delay_50ms.isExpired())
        {
            return;
        }

        if (internal_state.x >= internal_state.step)
        {
            internal_state.current = internal_state::cyber_state::INIT_LEDS_2;
            return;
        }

        const auto pixel = CRGB(rs, gs, bs);
        leds[internal_state.p + internal_state.x] = pixel;

        FastLED.show();
        delay_50ms.restart();

        ++internal_state.x;

        return;
    }

    rs = (rs - ((rs - internal_state.rf) / internal_state.step * internal_state.i));
    gs = (gs - ((gs - internal_state.gf) / internal_state.step * internal_state.i));
    bs = (bs - ((bs - internal_state.bf) / internal_state.step * internal_state.i));

    internal_state.i += 1;
    internal_state.p += internal_state.step;

    // outer loop
    if (internal_state.p >= leds_count)
    {
        internal_state.current = internal_state::cyber_state::FADEOUT_INIT;
        return;
    }

    // switch state back to inner loop
    internal_state.current = internal_state::cyber_state::INIT_LEDS_1;
    internal_state.x = 0;
}

/* private */
void CyberPunk::fade()
{
    switch (internal_state.current)
    {
        case internal_state::cyber_state::FADEOUT_INIT:
        {
            reset_internal_state();
            internal_state.p = 40;
            internal_state.current = internal_state::cyber_state::FADEOUT;
            return;   
        }
        break;

        case internal_state::cyber_state::FADEIN_INIT:
        {
            reset_internal_state();
            internal_state.p = 239;
            internal_state.current = internal_state::cyber_state::FADEIN;
            return;   
        }
        break;

        case internal_state::cyber_state::FADEOUT:
        {
            if (!delay_100ms.isExpired())
            {
                return;
            }

            if (!delay_1000ms.isExpired())
            {
                return;
            }

            if (internal_state.x == 666u)
            {
                reset_internal_state();
                internal_state.current = internal_state::cyber_state::FADEIN_INIT;
                return;
            }

            if (internal_state.p >= 240u)
            {
                delay_1000ms.restart();
                internal_state.x = 666u;
                return;
            }

            FastLED.setBrightness(internal_state.p);
            FastLED.show();
            delay_100ms.restart();

            ++internal_state.p;
            return;   
        }
        break;

        case internal_state::cyber_state::FADEIN:
        {
            if (!delay_100ms.isExpired())
            {
                return;
            }

            if (!delay_1000ms.isExpired())
            {
                return;
            }

            if (internal_state.x == 666u)
            {
                internal_state.current = internal_state::cyber_state::FADEOUT_INIT;
                return;
            }

            if (internal_state.p < 40u)
            {
                delay_1000ms.restart();
                internal_state.x = 666;
                return;
            }

            FastLED.setBrightness(internal_state.p);
            FastLED.show();
            delay_100ms.restart();

            --internal_state.p;
            return;   
        }
        break;
    }
}

/* public */
void CyberPunk::run(const uint16_t leds_count, CRGB leds[])
{
    switch (internal_state.current)
    {
        case internal_state::cyber_state::NOTHING:
        {
            return;
        }
        break;

        case internal_state::cyber_state::INIT_LEDS_1:
        case internal_state::cyber_state::INIT_LEDS_2:
        {
            init_leds(leds_count, leds);
            return;
        }
        break;

        case internal_state::cyber_state::FADEOUT_INIT:
        case internal_state::cyber_state::FADEOUT:
        case internal_state::cyber_state::FADEIN_INIT:
        case internal_state::cyber_state::FADEIN:
        {
            fade();
            return;
        }
        break;
    }

    return;
    

}

} // end of namespace