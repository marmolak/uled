#include <Arduino.h>

#include <FastLED.h>
#include <SPI.h>
#include <EtherCard.h>
#include <IPAddress.h>

#include "Shared/Resources.hpp"
#include "RemoteHandler/RemoteHandler.hpp"
#include "RemoteProtocol/RemoteProtocol.hpp"
#include "Presets/cyberpunk.hpp"

// nice hack bro!
void (*reset_me)(void) = 0;

const static unsigned int idle_period_secs = 30 * 1000;

// ethernet
const static byte my_ip[] = { 192, 168, 32, 230 };
const static byte my_mac[] = { 0x71, 0x69, 0x69, 0x2D, 0x30, 0x31 };

byte Ethernet::buffer[sizeof(Remote::led_packet) * 20];

// stripe
#define LED_PIN     5

#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

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
  ether.udpServerListenOnPort(&Remote::udp_parse, 1337);

  // stripe init
  delay(3000);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(Shared::leds, Shared::NUM_LEDS).setCorrection(TypicalLEDStrip);
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

  Presets::CyberPunk::run(Shared::NUM_LEDS, Shared::leds);
}
