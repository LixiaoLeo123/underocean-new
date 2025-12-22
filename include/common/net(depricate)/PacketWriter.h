//
// Created by 15201 on 11/22/2025.
//

#ifndef UNDEROCEAN_PACKETWRITER_H
#define UNDEROCEAN_PACKETWRITER_H
#include <cstdint>
#include <vector>
#include <winsock2.h>

class PacketWriter{  //NOTE: life cycle must cover the pa
public:
    PacketWriter() { packet_.reserve(MAX_PACKET_SIZE); }
    [[nodiscard]] bool canWrite (int bytes) const {
        return (packet_.size() + bytes < MAX_PACKET_SIZE);
    }
    PacketWriter& writeInt32(std::uint32_t host_val) {
        std::uint32_t net_val = htonl(host_val);
        packet_.insert(packet_.end(), reinterpret_cast<std::uint8_t*>(&net_val), reinterpret_cast<std::uint8_t*>(&net_val) + sizeof(net_val));
        return *this;
    }
    PacketWriter& writeInt16(std::uint16_t host_val) {
        std::uint16_t net_val = htons(host_val);
        packet_.insert(packet_.end(), reinterpret_cast<std::uint8_t*>(&net_val), reinterpret_cast<std::uint8_t*>(&net_val) + sizeof(net_val));
        return *this;
    }
    PacketWriter& writeInt32(std::int32_t host_val) {
        writeInt32(static_cast<std::uint32_t>(host_val));
        return *this;
    }
    PacketWriter& writeInt16(std::int16_t host_val) {
        writeInt16(static_cast<std::uint16_t>(host_val));
        return *this;
    }
    PacketWriter& writeInt8(std::uint8_t host_val) {  //no conversion needed
        packet_.push_back(host_val);
        return *this;
    }
    PacketWriter& writeInt8(std::int8_t host_val) {
        writeInt8(static_cast<std::uint8_t>(host_val));
        return *this;
    }
    PacketWriter& writeStr(char* str, std::size_t length) {  //no null-terminator
        packet_.insert(packet_.end(), reinterpret_cast<std::uint8_t*>(str), reinterpret_cast<std::uint8_t*>(str) + length);
        return *this;
    }
    std::vector<std::uint8_t>* takePacket() {  //read-only
        return &packet_;
    }
    void clearBuffer() {
        packet_.clear();
    }
private:
    static constexpr int MAX_PACKET_SIZE = 512;
    std::vector<std::uint8_t> packet_;
};
#endif //UNDEROCEAN_PACKETWRITER_H