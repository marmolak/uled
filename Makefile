all:
	g++ -std=gnu++17 -Wall -Wextra -I./remote-stripe/include -o packets packets.cpp

clean:
	rm -f packets
