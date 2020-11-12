# uled
UDP driven LED stripe

packets just sends udp frames to led stripe.

Compile with:
CPPFLAGS="-std=gnu++17" make packets

remote-stripe is not so clever prototype for arduino boards.
It works on ENC28J60 and some chinese arduino nano clone.
