#include <avr/power.h>

#include "Presets/cyberpunk.hpp"
#include "Common/Config/Config.hpp"

namespace Presets {

/** @brief CyberPunk (purple/orange) like preset 
 * 
 */
void CyberPunk::run(const uint16_t leds_count, CRGB leds[])
{
    FastLED.setBrightness(100);

    // start color
    uint8_t rs = 96;
    uint8_t gs = 29;
    uint8_t bs = 100;

    // end color
    const uint8_t rf = 255;
    const uint8_t gf = 165;
    const uint8_t bf = 0;

    const uint16_t step = 20;

    // power save mode

    // 1 MHz
    clock_prescale_set(clock_div_8);

    power_adc_disable();
    power_twi_disable();
    

    for (uint16_t p = 0, i = 0; p < leds_count; p += step, ++i)
    {
        for (uint16_t x = 0; x < step; ++x)
        {
            const auto pixel = CRGB(rs, gs, bs);
            leds[p + x] = pixel;

            FastLED.show();
            delay(50 / 8);
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
            delay(50 / 8);
            FastLED.show();
            delay(50 / 8);
        }

        delay(1000 / 8);

        // fade
        for (uint16_t p = 240; p > 40; --p) {
            FastLED.setBrightness(p);
            delay(50 / 8);
            FastLED.show();
            delay(50 / 8);
        }

        delay(1000 / 8);
    } while (true);
}

} // end of namespace