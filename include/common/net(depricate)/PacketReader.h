//
// Created by 15201 on 11/22/2025.
//

#ifndef UNDEROCEAN_PACKETREADER_H
#define UNDEROCEAN_PACKETREADER_H

#include <cassert>
#include <cstdint>
#include <vector>
#include <winsock2.h>  // for ntohl, ntohs

class PacketReader {
public:
    explicit PacketReader(std::vector<std::uint8_t>&& packet)
        : packet_(std::move(packet)) {}

    [[nodiscard]] bool canRead(int bytes) const {
        return (index_ + bytes) <= packet_.size();
    }

    [[nodiscard]] bool hasNext() const {
        return index_ < packet_.size();
    }

    std::int8_t nextInt8() {
        assert(canRead(1));
        return static_cast<std::int8_t>(packet_[index_++]);
    }

    std::int16_t nextInt16() {
        assert(canRead(2));
        std::uint16_t val;
        std::memcpy(&val, &packet_[index_], sizeof(val));
        index_ += 2;
        return static_cast<std::int16_t>(ntohs(val));
    }

    std::int32_t nextInt32() {
        assert(canRead(4));
        std::uint32_t val;
        std::memcpy(&val, &packet_[index_], sizeof(val));
        index_ += 4;
        return static_cast<std::int32_t>(ntohl(val));
    }

    std::uint8_t nextUInt8() {
        assert(canRead(1));
        return packet_[index_++];
    }

    std::uint16_t nextUInt16() {
        assert(canRead(2));
        std::uint16_t val;
        std::memcpy(&val, &packet_[index_], sizeof(val));
        index_ += 2;
        return ntohs(val);
    }

    std::uint32_t nextUInt32() {
        assert(canRead(4));
        std::uint32_t val;
        std::memcpy(&val, &packet_[index_], sizeof(val));
        index_ += 4;
        return ntohl(val);
    }

    void jumpBytes(int bytes) {
        assert(canRead(bytes));
        index_ += bytes;
    }

private:
    std::vector<std::uint8_t> packet_;
    int index_ = 0;  // index of the next byte to read
};

#endif //UNDEROCEAN_PACKETREADER_H
