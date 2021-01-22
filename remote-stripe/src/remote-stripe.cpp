#include <Arduino.h>

#include <FastLED.h>
#include <SPI.h>
#include <EtherCard.h>
#include <IPAddress.h>

#include "Common/Config/Config.hpp"
#include "Shared/Resources.hpp"
#include "RemoteHandler/RemoteHandler.hpp"
#include "RemoteProtocol/RemoteProtocol.hpp"


// nice hack bro!
void (*reset_me)(void) = 0;

const static unsigned int idle_period_secs = 30 * 1000;

// ethernet
const static byte my_ip[]   = { 192, 168, 32, 230 };
const static byte my_gw[]   = { 192, 168, 32, 1 };
const static byte my_dns[]  = { 192, 168, 32, 31 };
const static byte my_mask[] = { 255, 255, 255, 0 };
const static byte my_mac[]  = { 0x71, 0x69, 0x69, 0x3D, 0x31, 0x31 };

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
  const int ret = ether.begin(sizeof Ethernet::buffer, my_mac, SS);
  if (ret == 0)
  {
    Serial.println(F("Failed to access Ethernet controller"));
    Serial.flush();
    delay(1000);
    reset_me();
  }

  ether.staticSetup(my_ip, my_gw, my_dns, my_mac);
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
}
