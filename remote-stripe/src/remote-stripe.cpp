#include <Arduino.h>

#include <FastLED.h>
#include <SPI.h>
#include <EtherCard.h>
#include <IPAddress.h>

#include "Presets/cyberpunk.hpp"

// nice hack bro!
void (*reset_me)(void) = 0;

const static byte idle_period_secs = 30;

// ethernet
const static byte my_ip[] = { 192, 168, 32, 230 };
const static byte my_mac[] = { 0x71, 0x69, 0x69, 0x2D, 0x30, 0x31 };


enum class preset_ops : uint8_t
{
    NOOP      = 0,
    SETPIXEL  = 1,
    FADEOUT   = 2,
    FADEIN    = 3,
    BRIGHT    = 4,
    SETSLEN   = 5,
};

struct __attribute__((packed)) led_packet
{
  preset_ops special_ops;
  int16_t pos;
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

byte Ethernet::buffer[sizeof(led_packet) * 20];

// stripe
#define LED_PIN     5


const uint16_t NUM_LEDS = 244;

#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];




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

    if (led_data->pos >= NUM_LEDS) {
      led_data->pos %= NUM_LEDS;
    }

    switch (led_data->special_ops)
    {
      case preset_ops::SETPIXEL:
      {
        const auto pixel = CRGB(led_data->r, led_data->g, led_data->b);
        leds[led_data->pos] = pixel;

        FastLED.show();
        break;
      }

      case preset_ops::FADEOUT:
      {
        const auto pixel = CRGB(led_data->r, led_data->g, led_data->b);

        for (int16_t pos = 0; pos < led_data->pos; ++pos)
        {
          leds[pos] = pixel;
        }
        FastLED.show();

        delay(50);

        for (int16_t pos = 0; pos < led_data->pos; ++pos)
        {
          leds[pos] = CRGB::Black;
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
    } // end switch
    
    point_data += sizeof(led_packet);

    //Serial.println(String(led_data->pos) + ":" + String(led_data->r) + ":" + String(led_data->g) + ":" + String(led_data->b));
  }

  
}

void setup()
{
  Serial.begin(9600);

  // Init ethernet
  int ret = ether.begin(sizeof Ethernet::buffer, my_mac, SS);

  if (ret == 0)
  {
    Serial.println(F("Failed to access Ethernet controller"));
    Serial.flush();
    delay(1000);
    reset_me();
  }

  ether.staticSetup(my_ip, NULL);
  ether.udpServerListenOnPort(&udp_parse, 1337);

  // stripe init
  delay(3000);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  Serial.println(F("All DONE"));
  Serial.flush();
}

static unsigned long idle_time;
static bool preset = false;

void loop()
{
  if (!preset) {
    const unsigned long start_time = millis();

    ether.packetLoop(ether.packetReceive());

    const unsigned long stop_time = millis();
  
    idle_time += (stop_time - start_time);
  } 

  if ((idle_time < idle_period_secs))
  {
    return;
  }

  // if we just
  preset = true;

  Presets::CyberPunk::run(NUM_LEDS, leds);
}
