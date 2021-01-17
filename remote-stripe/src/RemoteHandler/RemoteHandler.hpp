#pragma once
#include "Common/MultiarchIncludes.hpp"

namespace Remote {

void udp_parse(uint16_t dest_port, uint8_t src_ip[4], uint16_t src_port, const char *data, uint16_t len);

} // end of namespace