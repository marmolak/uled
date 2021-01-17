#include "Presets/cyberpunk.hpp"

namespace Presets {

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

    for (uint16_t p = 0, i = 0; p < leds_count; p += step, ++i)
    {
        for (uint16_t x = 0; x < step; ++x)
        {
            const auto pixel = CRGB(rs, gs, bs);
            leds[p + x] = pixel;

            FastLED.show();
            delay(50);
        }

        rs = (rs - ((rs - rf) / 20 * i));
        gs = (gs - ((gs - gf) / 20 * i));
        bs = (bs - ((bs - bf) / 20 * i));
    }

    // stuck forever
    do {
        // light up
        for (uint16_t p = 40; p < 240; ++p) {
            FastLED.setBrightness(p);
            delay(50);
        }

        delay(100);

        // fade
        for (uint16_t p = 240; p > 40; --p) {
            FastLED.setBrightness(p);
            delay(50);
        }
    } while (true);
}

} // end of namespace