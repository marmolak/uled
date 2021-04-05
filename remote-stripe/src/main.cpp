#include <Arduino.h>

#include <FastLED.h>
#include <SPI.h>
#include <EtherCard.h>
#include <IPAddress.h>

#include "Common/Config/Config.hpp"
#include "Common/Config/Network.hpp"
#include "Shared/Resources.hpp"
#include "RemoteHandler/RemoteHandler.hpp"
#include "RemoteProtocol/RemoteProtocol.hpp"

#include "Presets/cyberpunk.hpp"

// stripe
#define LED_PIN     5

#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

byte Ethernet::buffer[sizeof(Remote::led_packet) * 20u];

namespace { 
  // nice hack bro!
  void (*reset_me)(void) = 0;

  const unsigned int idle_period_secs = 30 * 1000;

} // end of namespace

void setup()
{
  Serial.begin(9600);

  // Init ethernet
  const int ret = ether.begin(sizeof Ethernet::buffer, Common::Config::Network::my_mac, SS);
  if (ret == 0)
  {
      Serial.println(F("Failed to access Ethernet controller"));
      Serial.flush();
      delay(1000);
      reset_me();
  }

  ether.staticSetup(Common::Config::Network::my_ip, Common::Config::Network::my_gw, Common::Config::Network::my_dns, Common::Config::Network::my_mac);
  ether.udpServerListenOnPort(&Remote::udp_parse, Config::port);

  // stripe init
  delay(3000);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(Shared::leds, Config::NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  Serial.println(F("All DONE"));
  Serial.flush();
}

void loop()
{
    ether.packetLoop(ether.packetReceive());

    Presets::cp.run(Config::NUM_LEDS, Shared::leds);
}
