#include "packet.h"

bool NetPacket::from_bytes(const std::vector<char> &in_packet)
{
    const uint32_t *size_ptr = reinterpret_cast<const uint32_t *>(in_packet.data());
    uint32_t packet_size = *size_ptr;
    if (packet_size != in_packet.size())
    {
        return false;
    }

    ++size_ptr;

    uint32_t payload_size = *size_ptr;
    if (payload_size > packet_size)
    {
        return false;
    }

    payload.assign(in_packet.data() + 8, in_packet.data() + 8 + payload_size);
    // ...
    return true;
}

std::vector<char> NetPacket::to_bytes()
{
    uint32_t packet_size = payload.size() + signature.size() + pubkey.size() + 3 * sizeof(uint32_t);
    std::vector<char> result;
    result.resize(packet_size);
    uint32_t *ptr = reinterpret_cast<uint32_t *>(result.data());
    *ptr = packet_size;
    ++ptr;
    *ptr = payload.size();
    std::copy(payload.begin(), payload.end(), result.begin() + 8);
    return result;
}
