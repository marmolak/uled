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

using namespace std::chrono_literals;
using namespace std;



const uint16_t port = 1337;
const int leds_count = 240;

enum class preset_ops : uint8_t
{
  NOOP              = 0,
  SETPIXEL          = 1,
  FADEOUT           = 2,
  FADEIN            = 3,
  BRIGHT            = 4,
  SETPIXEL_NOSHOW   = 5,
  SHOW              = 6
};

struct __attribute__((packed)) led_packet
{
   preset_ops special_ops = preset_ops::SETPIXEL;
   uint16_t pos;
   uint8_t r;
   uint8_t g;
   uint8_t b;

};


int main()
{
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd <= 0) { 
		return EXIT_FAILURE; 
	}

	hostent *host = (struct hostent *) gethostbyname((char *) "192.168.32.230");
	struct sockaddr_in servaddr {
		.sin_family 	= AF_INET, 
    		.sin_port 	= htons(port),
		.sin_addr 	= *((struct in_addr *)host->h_addr)
	};
	memset(&(servaddr.sin_zero), 0, sizeof(servaddr.sin_zero));


	uint8_t i = 0;


	std::srand(std::time(nullptr));


	/*
	const uint8_t r1 = rand() % 100;
	const uint8_t g1 = rand() % 40;
	const uint8_t b1 = rand() % 255;

	const uint8_t r2 = rand() % 100;
	const uint8_t g2 = rand() % 40;
	const uint8_t b2 = rand() % 255;
	*/

	const led_packet bright
	{
		.pos = 100,
		.special_ops = preset_ops::BRIGHT,
	};
	sendto(sockfd, (void *) &bright, sizeof(bright), 0, (struct sockaddr *) &servaddr, sizeof(struct sockaddr));

	uint8_t rs = 96;
	uint8_t gs = 29;
	uint8_t bs = 100;

	const uint8_t rf = 255;
	const uint8_t gf = 165;
	const uint8_t bf = 0;

	for (uint16_t p = 0,
	     i = 0 
	     ;
	     p < leds_count;
	     p+=20, ++i
	)
	{

		for (uint16_t x = 0; x < 20; ++x)
		{	
			const vector<led_packet> leds { led_packet {
				.pos = static_cast<uint16_t>(p + x),
				.r = rs,
				.g = gs,
				.b = bs,
				//.special_ops = preset_ops::FADEOUT,
			}};



			sendto(sockfd, (void *) leds.data(), leds.size() * sizeof(led_packet), 0, (struct sockaddr *) &servaddr, sizeof(struct sockaddr));
			std::this_thread::sleep_for(50ms);


		}
		rs = (rs - ((rs - rf) / 20 * i));
		gs = (gs - ((gs - gf) / 20 * i));
		bs = (bs - ((bs - bf) / 20 * i));

		continue;
 

		vector<led_packet> leds;
		leds.emplace_back(led_packet
		{
			.pos = p,
			.r = 96,
			.g = 29,
			.b = 100,
			//.special_ops = preset_ops::FADEOUT,
		});
		leds.emplace_back(led_packet
		{
			.pos = i,
			.r = 255,
			.g = 165,
			.b = 0,
			//.special_ops = preset_ops::FADEOUT,
		});


		sendto(sockfd, (void *) leds.data(), leds.size() * sizeof(led_packet), 0, (struct sockaddr *) &servaddr, sizeof(struct sockaddr));
		std::this_thread::sleep_for(50ms);

		continue;


		/*const led_packet bright
		{
			.pos = b,
			.special_ops = preset_ops::BRIGHT,
		};
		sendto(sockfd, (void *) &bright, sizeof(bright), 0, (struct sockaddr *) &servaddr, sizeof(struct sockaddr)); */

	}


	do {
		for (uint16_t p = 40; p < 240; ++p) {

			const led_packet bright
			{
				.pos = p,
				.special_ops = preset_ops::BRIGHT,
			};
			sendto(sockfd, (void *) &bright, sizeof(bright), 0, (struct sockaddr *) &servaddr, sizeof(struct sockaddr));
			std::this_thread::sleep_for(50ms);
			const led_packet x 
			{
				.pos = p,
				.special_ops = preset_ops::SHOW,
			};
			sendto(sockfd, (void *) &x, sizeof(x), 0, (struct sockaddr *) &servaddr, sizeof(struct sockaddr));
		}

		for (uint16_t p = 240; p > 40; --p) {

			const led_packet bright
			{
				.pos = p,
				.special_ops = preset_ops::BRIGHT,
			};
			sendto(sockfd, (void *) &bright, sizeof(bright), 0, (struct sockaddr *) &servaddr, sizeof(struct sockaddr));
			std::this_thread::sleep_for(50ms);
			const led_packet x 
			{
				.pos = p,
				.special_ops = preset_ops::SHOW,
			};
			sendto(sockfd, (void *) &x, sizeof(x), 0, (struct sockaddr *) &servaddr, sizeof(struct sockaddr));
		}
			std::this_thread::sleep_for(50ms);
	} while (true);

	return EXIT_SUCCESS; 
	std::this_thread::sleep_for(1s);

	for (int16_t p = leds_count * 100; p > 0; --p)
	{
		const uint8_t rpos = rand() % 254;
		const uint8_t r3 = rand() % 254;
		const uint8_t g3 = rand() % 254;
		const uint8_t b3 = rand() % 254;

		led_packet led  {
			.pos = rpos,
			.r = r3,
			.g = g3,
			.b = 0,
		};

		sendto(sockfd, (void *) &led, sizeof(led), 0, (struct sockaddr *) &servaddr, sizeof(struct sockaddr));
		std::this_thread::sleep_for(30ms);
	}
}
