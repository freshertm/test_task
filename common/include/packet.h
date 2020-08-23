#pragma once

#include <stdint.h>
#include <vector>
#include <exception>
#include <string>

/*
       = Packet structure =
4 bytes              - packet size
4 bytes              - payload size
(payload size) bytes - payload
4 bytes              - signature size
(sign size) bytes    - signature
4 bytes              - pubkey size
(pubkey size) bytes  - public key

*/

struct NetPacket
{
    uint32_t packet_size;
    std::string payload;
    std::vector<char> signature;
    std::vector<char> pubkey;

    bool from_bytes(const std::vector<char> &);
    std::vector<char> to_bytes();
};