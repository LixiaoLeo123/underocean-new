//
// Created by 15201 on 11/22/2025.
//

#ifndef UNDEROCEAN_PACKETREADER_H
#define UNDEROCEAN_PACKETREADER_H
#include <cassert>
#include <cstdint>
#include <vector>
#include <winsock2.h>

class PacketReader {
public:
    explicit PacketReader(std::vector<std::uint8_t>&& packet)
        :packet_(std::move(packet)){}
    bool canRead(int bytes) const {
        return (index_ + bytes) <= packet_.size();
    }
    std::int8_t nextInt8() {   //must add check
        assert(index_ + 1 <= packet_.size());
        return static_cast<std::int8_t>(packet_[index_++]);
    }
    std::int16_t nextInt16() {
        assert(index_ + 2 <= packet_.size());
        std::uint16_t val = (static_cast<std::uint16_t>(packet_[index_]) << 8) |
                             static_cast<std::uint16_t>(packet_[index_ + 1]);
        index_ += 2;
        return static_cast<std::int16_t>(ntohs(val));
    }
    std::int32_t nextInt32() {
        assert(index_ + 4 <= packet_.size());
        std::uint32_t val = (static_cast<std::uint32_t>(packet_[index_]) << 24) |
                            (static_cast<std::uint32_t>(packet_[index_ + 1]) << 16) |
                            (static_cast<std::uint32_t>(packet_[index_ + 2]) << 8)  |
                             static_cast<std::uint32_t>(packet_[index_ + 3]);
        index_ += 4;
        return static_cast<std::int32_t>(ntohl(val));
    }
    std::uint8_t nextUInt8() {
        assert(index_ + 1 <= packet_.size());
        return packet_[index_++];
    }

    std::uint16_t nextUInt16() {
        assert(index_ + 2 <= packet_.size());
        std::uint16_t val = (static_cast<std::uint16_t>(packet_[index_]) << 8) |
                             static_cast<std::uint16_t>(packet_[index_ + 1]);
        index_ += 2;
        return ntohs(val);
    }

    std::uint32_t nextUInt32() {
        assert(index_ + 4 <= packet_.size());
        std::uint32_t val = (static_cast<std::uint32_t>(packet_[index_]) << 24) |
                            (static_cast<std::uint32_t>(packet_[index_ + 1]) << 16) |
                            (static_cast<std::uint32_t>(packet_[index_ + 2]) << 8)  |
                             static_cast<std::uint32_t>(packet_[index_ + 3]);
        index_ += 4;
        return ntohl(val);
    }
    void jumpBytes(int bytes) {
        index_ += bytes;
    }
private:
    std::vector<std::uint8_t> packet_;
    int index_ = 0;  //index that should read next
};
#endif //UNDEROCEAN_PACKETREADER_H