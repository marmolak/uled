#include <FastLED.h>

#include "Common/Config/Config.hpp"

#include "RemoteProtocol/RemoteProtocol.hpp"
#include "RemoteHandler/RemoteHandler.hpp"
#include "Shared/Resources.hpp"

// add presets
#include "Presets/cyberpunk.hpp"

namespace Remote {

void udp_parse(uint16_t dest_port, uint8_t src_ip[4], uint16_t src_port, const char *data, uint16_t len)
{
  if (len < sizeof(led_packet))
  {
      return;
  }

  // Yeah.. i'm pig. Just blame me. 
  char *point_data = data;

  const uint16_t count = len / sizeof(led_packet);

  for (uint16_t p = 0; p < count; ++p)
  {
    auto led_data = reinterpret_cast<led_packet *const>(point_data);

    if (led_data->pos >= Config::NUM_LEDS)
    {
        led_data->pos %= Config::NUM_LEDS;
    }

    switch (led_data->special_ops)
    {
      case preset_ops::SETPIXEL:
      {
        const auto pixel = CRGB(led_data->r, led_data->g, led_data->b);
        Shared::leds[led_data->pos] = pixel;

        FastLED.show();
        break;
      }

      case preset_ops::FADEOUT:
      {
        const auto pixel = CRGB(led_data->r, led_data->g, led_data->b);

        for (int16_t pos = 0; pos < led_data->pos; ++pos)
        {
            Shared::leds[pos] = pixel;
        }
        FastLED.show();

        delay(50);

        for (int16_t pos = 0; pos < led_data->pos; ++pos)
        {
            Shared::leds[pos] = CRGB::Black;
        }
        FastLED.show();

        break;
      }
 
      case preset_ops::BRIGHT:
      {
        if (led_data->pos >= 255) {
          led_data->pos %= 255;
        }

        FastLED.setBrightness(led_data->pos);
        break;
      }

      case preset_ops::SETPIXEL_NOSHOW:
      {
        const auto pixel = CRGB(led_data->r, led_data->g, led_data->b);
        Shared::leds[led_data->pos] = pixel;
        break;
      }

      case preset_ops::SHOW:
      {
            FastLED.show();
            break;
      }

      case preset_ops::PRESET:
      {
          // we should support more presets but it doesn't matter now.
          Presets::CyberPunk::run(Config::NUM_LEDS, Shared::leds);
          break;
      }
    } // end switch
    
    point_data += sizeof(led_packet);
  }
}


}