#include <FastLED.h>

#include "RemoteProtocol/RemoteProtocol.hpp"
#include "RemoteHandler/RemoteHandler.hpp"
#include "Shared/Resources.hpp"

namespace Remote {

void udp_parse(uint16_t dest_port, uint8_t src_ip[4], uint16_t src_port, const char *data, uint16_t len)
{
  if (len < sizeof(led_packet))
  {
      return;
  }

  char *point_data = data;

  //Serial.println("Something: " + String(len % sizeof(led_packet)));

  const uint16_t count = len / sizeof(led_packet);

  for (uint16_t p = 0; p < count; ++p)
  {
    auto led_data = reinterpret_cast<led_packet *const>(point_data);

    if (led_data->pos >= Shared::NUM_LEDS)
    {
        led_data->pos %= Shared::NUM_LEDS;
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
    } // end switch
    
    point_data += sizeof(led_packet);

    //Serial.println(String(led_data->pos) + ":" + String(led_data->r) + ":" + String(led_data->g) + ":" + String(led_data->b));
  }
}


}