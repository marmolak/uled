#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <netdb.h>

#include <iostream>

#include <cstdlib>
#include <cstring>
#include <thread>
#include <vector>
#include <chrono>

#include "RemoteProtocol/RemoteProtocol.hpp"
#include "Common/Config/Config.hpp"

using namespace std::chrono_literals;
using namespace std;

void preset(const int sockfd, struct sockaddr_in *servaddr)
{
	const Remote::led_packet preset
	{
		.special_ops = Remote::preset_ops::PRESET,
	};
	sendto(sockfd, (void *) &preset, sizeof(preset), 0, (struct sockaddr *) servaddr, sizeof(struct sockaddr));
}

int main(int argc, char **argv)
{
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd <= 0) { 
		return EXIT_FAILURE; 
	}

	hostent *host = (struct hostent *) gethostbyname((char *) "192.168.32.230");
	struct sockaddr_in servaddr
	{
		.sin_family = AF_INET,
		.sin_port 	= htons(Config::port),
		.sin_addr 	= *((struct in_addr *)host->h_addr),
		.sin_zero   = { 0 }
	};

	if (argc >= 2)
	{
		const bool ok = (strcmp(argv[1], "--preset") == 0);
		if (!ok) {
			return EXIT_FAILURE;
		}

		preset(sockfd, &servaddr);
		return EXIT_SUCCESS;
	}

	const Remote::led_packet bright
	{
		.special_ops = Remote::preset_ops::BRIGHT,
		.pos = 100,
	};
	sendto(sockfd, (void *) &bright, sizeof(bright), 0, (struct sockaddr *) &servaddr, sizeof(struct sockaddr));

	uint8_t rs = 96;
	uint8_t gs = 29;
	uint8_t bs = 100;

	const uint8_t rf = 255;
	const uint8_t gf = 165;
	const uint8_t bf = 0;

	const uint8_t step = 20;

	for (uint16_t p = 0, i = 0;
	     p < Config::NUM_LEDS;
	     p += step, ++i
	)
	{
		for (uint16_t x = 0; x < step; ++x)
		{	
			const vector<Remote::led_packet> leds { Remote::led_packet {
				.special_ops = Remote::preset_ops::SETPIXEL,
				.pos = static_cast<uint16_t>(p + x),
				.r = rs,
				.g = gs,
				.b = bs,
			}};

			sendto(sockfd, (void *) leds.data(), leds.size() * sizeof(Remote::led_packet), 0, (struct sockaddr *) &servaddr, sizeof(struct sockaddr));
			std::this_thread::sleep_for(50ms);
		}

		rs = (rs - ((rs - rf) / step * i));
		gs = (gs - ((gs - gf) / step * i));
		bs = (bs - ((bs - bf) / step * i));
	}


	do {
		for (uint16_t p = 40; p < 240; ++p) {

			const Remote::led_packet bright
			{
				.special_ops = Remote::preset_ops::BRIGHT,
				.pos = p,
			};
			sendto(sockfd, (void *) &bright, sizeof(bright), 0, (struct sockaddr *) &servaddr, sizeof(struct sockaddr));
			std::this_thread::sleep_for(50ms);

			const Remote::led_packet x 
			{
				.special_ops = Remote::preset_ops::SHOW,
				.pos = p,
			};
			sendto(sockfd, (void *) &x, sizeof(x), 0, (struct sockaddr *) &servaddr, sizeof(struct sockaddr));
			std::this_thread::sleep_for(50ms);
		}

		std::this_thread::sleep_for(1s);

		for (uint16_t p = 239; p > 40; --p) {

			const Remote::led_packet bright
			{
				.special_ops = Remote::preset_ops::BRIGHT,
				.pos = p,
			};
			sendto(sockfd, (void *) &bright, sizeof(bright), 0, (struct sockaddr *) &servaddr, sizeof(struct sockaddr));
			std::this_thread::sleep_for(50ms);

			const Remote::led_packet x 
			{
				.special_ops = Remote::preset_ops::SHOW,
				.pos = p,
			};
			sendto(sockfd, (void *) &x, sizeof(x), 0, (struct sockaddr *) &servaddr, sizeof(struct sockaddr));
			std::this_thread::sleep_for(50ms);
		}
		std::this_thread::sleep_for(1s);
	} while (true);

	return EXIT_SUCCESS; 
}
