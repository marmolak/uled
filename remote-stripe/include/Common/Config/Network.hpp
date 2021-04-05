#pragma once
#include <stdint.h>

namespace Common { namespace Config { namespace Network {

// ethernet
const uint8_t my_ip[]   = { 192, 168, 32, 230 };
const uint8_t my_gw[]   = { 192, 168, 32, 1 };
const uint8_t my_dns[]  = { 192, 168, 32, 31 };
const uint8_t my_mask[] = { 255, 255, 255, 0 };
const uint8_t my_mac[]  = { 0x71, 0x69, 0x69, 0x3D, 0x31, 0x31 };

}}} // end of namespaces